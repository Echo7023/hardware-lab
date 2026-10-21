/**********************************************************************
 *  电气玩具 · 圆柱顶端往复打磨 —— 主控程序（SDCC 版本 main_sdcc.c）
 * ------------------------------------------------------------------
 *  芯片  ：STC8G1K08（SOP16 封装，14 个可用 IO）
 *  主频  ：24MHz（在 STC-ISP 烧录时设置，代码内无需配置时钟）
 *  编译  ：SDCC 4.5（免费开源，安装于 C:\MY\Program Files\SDCC）
 *          方式一：双击同目录的 build.bat（推荐）
 *          方式二：命令行 sdcc -mmcs51 --model-small main_sdcc.c
 *          再用 packihx main_sdcc.ihx > main_sdcc.hex
 *  日期  ：2026-10-14
 * ------------------------------------------------------------------
 *  本文件为 SDCC 编译版（原 Keil 语法版已随整理移除，可从 git 历史找回）；
 *  与 Keil 写法的主要差异：
 *    1. 不用 STC8G.H 头文件，改为本文件内直接声明寄存器
 *       （__sfr/__sbit，地址对照 STC 官方资料与 STC8G.H 核实）
 *    2. sbit          -> __sbit __at (位地址)
 *    3. interrupt N   -> __interrupt (N)
 *    4. _nop_()       -> 本文件内的 nop 小函数 adc_settle()
 *    5. 已修复滑条占空比计算的 16 位溢出（改为 32 位计算）
 * ------------------------------------------------------------------
 *  【引脚分配】已对照官方手册核实（SOP16 真实存在的脚）
 *     P1.0 / ADC0  <- 直滑电位器滑条（调速）           SOP16 第 1 脚
 *     P1.1         <- 模式按键（按下=低，内部弱上拉）   SOP16 第 2 脚
 *     P3.2 / INT0  <- 编码器 A 相                      SOP16 第 11 脚
 *     P3.3 / INT1  <- 编码器 B 相                      SOP16 第 12 脚
 *     P5.4         -> TB6612 PWMA（软件 PWM 20kHz）    SOP16 第 5 脚
 *     P5.5         -> TB6612 AIN1                      SOP16 第 7 脚
 *     P3.7         -> TB6612 AIN2                      SOP16 第 16 脚
 *   注：P5.4 出厂默认为普通 I/O 口，STC-ISP 里【不要】勾选
 *       "把 P5.4 设置为复位脚"，否则 PWM 无输出。
 * ------------------------------------------------------------------
 *  【功能】（2026-10-12 确认版）
 *   1. 模式键循环切换：待机 -> ±5cm 往复 -> ±2cm 往复 -> 随机 -> 待机
 *   2. 开机位置 = 本轮零位；绝对边界 ±5cm，任何情况不许超出
 *   3. 滑条无级调速：PWM 0~80%（80% 上限保护 6V 电机跑 7.4V）
 *   4. 死区 0.3cm；接近目标 1.5cm 内按剩余距离比例减速（防越界方案 A）
 *   5. 堵转保护：电机通电但编码器 5 秒无脉冲 -> 停机，按模式键恢复
 *   6. 不使用 PID（位式控制：滑条越快，往复切换越快）
 *   7. 软启动：占空比按 1%/ms 爬升，换向时清零重爬，抑制电流冲击
 **********************************************************************/

/* ================== 0. 寄存器声明（SDCC 语法） ================== */
/* 字节寄存器 */
__sfr __at (0x88) TCON;       /* 定时器控制 */
__sfr __at (0x89) TMOD;
__sfr __at (0x8A) TL0;
__sfr __at (0x8B) TL1;
__sfr __at (0x8C) TH0;
__sfr __at (0x8D) TH1;
__sfr __at (0x8E) AUXR;       /* B7=T0x12, B6=T1x12 */
__sfr __at (0x90) P1;
__sfr __at (0x91) P1M1;
__sfr __at (0x92) P1M0;
__sfr __at (0xA8) IE;
__sfr __at (0xB0) P3;
__sfr __at (0xB1) P3M1;
__sfr __at (0xB2) P3M0;
__sfr __at (0xBC) ADC_CONTR;
__sfr __at (0xBD) ADC_RES;
__sfr __at (0xBE) ADC_RESL;
__sfr __at (0xC8) P5;
__sfr __at (0xC9) P5M1;
__sfr __at (0xCA) P5M0;
__sfr __at (0xDE) ADCCFG;

/* 位寄存器：引脚 */
__sbit __at (0x91) BTN_MODE;  /* P1.1 模式按键 */
__sbit __at (0xB2) ENC_A;     /* P3.2 编码器 A 相 */
__sbit __at (0xB3) ENC_B;     /* P3.3 编码器 B 相 */
__sbit __at (0xCC) PWM_OUT;   /* P5.4 PWM 输出 -> PWMA */
__sbit __at (0xCD) DRV_AIN1;  /* P5.5 方向位 1 -> AIN1 */
__sbit __at (0xB7) DRV_AIN2;  /* P3.7 方向位 2 -> AIN2 */

/* 位寄存器：定时器/中断控制位 */
__sbit __at (0x88) IT0;
__sbit __at (0x8A) IT1;
__sbit __at (0x8C) TR0;
__sbit __at (0x8E) TR1;
__sbit __at (0xA8) EX0;
__sbit __at (0xA9) ET0;
__sbit __at (0xAA) EX1;
__sbit __at (0xAB) ET1;
__sbit __at (0xAF) EA;

#define ADC_CH_POT  0     /* 电位器接 P1.0，对应 ADC 通道 0 */

/* ================== 1. 参数（可按需调整） ================== */
/* 脉冲当量：编码器 11 线 × 双边沿(×2) × 减速比 56 = 1232 脉冲/电机输出圈
 * 卷轴齿轮 z22 / 主齿轮 z26 -> 卷轴 1232 × 22/26 ≈ 1042 脉冲/卷轴圈
 * 卷轴筒 Ø20 -> 周长 π×2.0 ≈ 6.28cm -> 1042 / 6.28 ≈ 166 脉冲/cm
 * 校准方法：跑一次 5cm 模式用尺量纱布实际走了多少，按比例改这个值 */
#define PULSES_PER_CM   166

#define BOUND5          (5 * PULSES_PER_CM)      /* ±5cm 边界(脉冲) */
#define BOUND2          (2 * PULSES_PER_CM)      /* ±2cm 目标(脉冲) */
#define DEADZONE        (PULSES_PER_CM * 3 / 10) /* 死区 0.3cm */
#define SOFTZONE        (PULSES_PER_CM * 3 / 2)  /* 软区 1.5cm */
#define RAND_SPAN       (2 * BOUND5 + 1)         /* 随机目标总跨度 */

#define DUTY_MAX        80    /* 滑条最大占空比 80% */
#define MOVE_MIN        20    /* 软区减速下限 20%（低于此值电机可能转不动） */
#define STALL_MS        5000  /* 堵转判定时间 5 秒 */
#define STALL_MIN_DUTY  20    /* 滑条 >=20% 才启用堵转判定（防低占空比误判） */

#define PWM_PERIOD      1200  /* 20kHz @24MHz：24M/1200 = 20k */

/* ================== 2. 模式编号 ================== */
#define M_IDLE  0   /* 待机 */
#define M5      1   /* ±5cm 往复 */
#define M2      2   /* ±2cm 往复 */
#define MRAND   3   /* 随机 */

/* ================== 3. 全局变量 ================== */
volatile int          pos        = 0;      /* 当前位置(脉冲)，开机=0 零位 */
volatile unsigned int tick_ms    = 0;      /* 1ms 系统节拍 */
volatile unsigned int rnd        = 0xACE1; /* 随机数发生器 */
volatile unsigned int pwm_on     = 0;      /* PWM 高电平计数值(0~1200) */
unsigned char         pwm_high   = 0;      /* PWM 当前相位 */

int           target      = 0;             /* 当前目标位置(脉冲) */
unsigned char mode        = M_IDLE;        /* 当前模式 */
unsigned char stall_flag  = 0;             /* 堵转标志(1=已保护停机) */
unsigned char duty_slider = 0;             /* 滑条占空比 0~80 */
unsigned char duty_target = 0;             /* 目标占空比 */
unsigned char duty_actual = 0;             /* 实际占空比(斜坡后) */
signed char   last_dir    = 0;             /* 上次方向 -1/0/+1 */
unsigned int  last_move_ms = 0;            /* 上次编码器有脉冲的时刻 */
int           pos_last    = 0;             /* 堵转检测用位置快照 */
unsigned char btn_cnt     = 0;             /* 按键去抖计数 */
unsigned char btn_latch   = 0;             /* 按键锁定(防连发) */

/* ================== 4. ADC 通道切换小延时 ================== */
static void adc_settle(void)
{
    __asm
    nop
    nop
    nop
    nop
    __endasm;
}

/* ================== 5. 中断服务程序 ================== */
/* 编码器 A 相下降沿：B 相为高 -> 正转+1，否则反转-1
 * 若实测正转时 pos 反而减小，把 INT0/INT1 里的 ++/-- 对调即可 */
void INT0_ISR(void) __interrupt (0)
{
    if (ENC_B) pos++;
    else       pos--;
}

/* 编码器 B 相下降沿：A 相为低 -> 正转+1，否则反转-1 */
void INT1_ISR(void) __interrupt (2)
{
    if (ENC_A) pos--;
    else       pos++;
}

/* 定时器0：1ms 节拍 + 随机数搅动 */
void T0_ISR(void) __interrupt (1)
{
    tick_ms++;
    rnd = (rnd >> 1) ^ ((rnd & 1) ? 0xB400 : 0);   /* 16位伪随机序列 */
}

/* 定时器1：20kHz 软件 PWM（两段式：先高电平段，再低电平段） */
void T1_ISR(void) __interrupt (3)
{
    unsigned int reload;

    if (pwm_high) {                 /* 高电平段结束 -> 拉低 */
        PWM_OUT = 0;
        pwm_high = 0;
        reload = (unsigned int)(65536 - (PWM_PERIOD - pwm_on));
    } else if (pwm_on > 0) {        /* 低电平段结束 -> 拉高 */
        PWM_OUT = 1;
        pwm_high = 1;
        reload = (unsigned int)(65536 - pwm_on);
    } else {                        /* 占空比 0：保持低电平 */
        PWM_OUT = 0;
        reload = (unsigned int)(65536 - PWM_PERIOD);
    }
    TH1 = (unsigned char)(reload >> 8);
    TL1 = (unsigned char)reload;
}

/* ================== 6. 工具函数 ================== */
static int iabs(int x)
{
    return (x < 0) ? -x : x;
}

/* 生成随机目标：|目标| <= 5cm-死区，且与当前位置拉开距离 */
static int random_target(void)
{
    int t;
    unsigned char i;

    for (i = 0; i < 16; i++) {
        t = (int)((unsigned int)rnd % RAND_SPAN) - BOUND5;
        if (t >  (BOUND5 - DEADZONE)) t =  (BOUND5 - DEADZONE);
        if (t < -(BOUND5 - DEADZONE)) t = -(BOUND5 - DEADZONE);
        if (iabs(t - pos) >= DEADZONE * 2) break;   /* 太近就重摇 */
    }
    return t;
}

/* 电机输出：dir=+1 正转 / -1 反转 / 0 自由滑行
 * 换向或启停时占空比清零，防止反向电流冲击（TB6612 电流余量有限） */
static void set_motor(signed char dir, unsigned char duty)
{
    if (dir > 0)      { DRV_AIN1 = 1; DRV_AIN2 = 0; }
    else if (dir < 0) { DRV_AIN1 = 0; DRV_AIN2 = 1; }
    else              { DRV_AIN1 = 0; DRV_AIN2 = 0; }

    if (dir != last_dir) duty_actual = 0;
    last_dir = dir;

    duty_target = (dir == 0) ? 0 : duty;

    if (dir != 0 && duty_target > 0 && duty_actual == 0) {
        last_move_ms = tick_ms;      /* 重新启动，重计堵转窗口 */
    }
}

/* 切换模式：先停稳，再生成新目标 */
static void enter_mode(unsigned char m)
{
    mode = m;
    stall_flag = 0;                  /* 任何模式切换都解除堵转保护 */
    set_motor(0, 0);
    duty_target = 0;
    duty_actual = 0;
    pwm_on = 0;

    if (m == M5)    target = BOUND5;         /* 先向 +5cm 走 */
    if (m == M2)    target = BOUND2;         /* 先向 +2cm 走 */
    if (m == MRAND) target = random_target();
}

/* ================== 7. 周期任务 ================== */
/* 1ms：占空比斜坡（升 1%/ms，降立即），写 PWM 比较值 */
static void ramp_task(void)
{
    if (duty_actual < duty_target) duty_actual++;
    else if (duty_actual > duty_target) duty_actual = duty_target;

    pwm_on = (unsigned int)duty_actual * (PWM_PERIOD / 100);
}

/* 10ms：读滑条 -> 调速占空比 */
static void duty_slider_task(void)
{
    unsigned int adc;
    unsigned char to;

    ADC_RES = 0;
    ADC_RESL = 0;
    ADC_CONTR = 0x80 | ADC_CH_POT;    /* 开ADC电源 + 选通道0(P1.0) */
    adc_settle();
    ADC_CONTR |= 0x40;                /* 启动转换 */

    to = 200;
    while (!(ADC_CONTR & 0x20)) {     /* 等待转换完成(带超时保护) */
        if (--to == 0) break;
    }
    ADC_CONTR &= ~0x20;               /* 清完成标志 */

    adc = ((unsigned int)ADC_RES << 8) | ADC_RESL;   /* 右对齐10位 */
    duty_slider = (unsigned char)((unsigned long)adc * DUTY_MAX / 1023);  /* 32位计算防溢出 */
}

/* 10ms：按键扫描（30ms 去抖，按一次切一个模式） */
static void button_task(void)
{
    if (BTN_MODE == 0) {                    /* 按下(低电平) */
        if (btn_cnt < 100) btn_cnt++;
        if (btn_cnt == 3 && btn_latch == 0) {
            btn_latch = 1;                  /* 锁定：按住不连发 */
            enter_mode((unsigned char)((mode + 1) & 0x03));  /* 0→1→2→3→0 */
        }
    } else {
        btn_cnt = 0;
        btn_latch = 0;
    }
}

/* 10ms：闭环控制核心（位式控制，无 PID） */
static void control_task(void)
{
    int          err;
    unsigned int dist, factor, eff;
    signed char  dir;

    if (mode == M_IDLE || stall_flag) return;  /* 待机/堵转停机 */

    err  = target - pos;
    dist = (err >= 0) ? (unsigned int)err : (unsigned int)(-err);

    if (dist <= DEADZONE) {                    /* 到达目标 -> 换下一个目标 */
        if (mode == M5)    target = (target > 0) ? -BOUND5 : BOUND5;
        if (mode == M2)    target = (target > 0) ? -BOUND2 : BOUND2;
        if (mode == MRAND) target = random_target();
        set_motor(0, 0);                       /* 停一拍再反向，减少冲击 */
        return;
    }

    dir = (err > 0) ? 1 : -1;

    /* 硬限位双保险：绝不允许朝边界外驱动 */
    if (dir > 0 && pos >= BOUND5)  { set_motor(0, 0); return; }
    if (dir < 0 && pos <= -BOUND5) { set_motor(0, 0); return; }

    /* 软区（防越界方案A）：距目标 1.5cm 内按剩余距离比例减速 */
    if (dist >= SOFTZONE) factor = 100;
    else                  factor = dist * 100 / SOFTZONE;

    eff = (unsigned int)duty_slider * factor / 100;
    if (eff < MOVE_MIN && duty_slider >= MOVE_MIN) eff = MOVE_MIN;  /* 减速下限，防停住 */
    if (eff > duty_slider) eff = duty_slider;                        /* 只减速不放大 */

    set_motor(dir, (unsigned char)eff);
}

/* 100ms：堵转保护（通电但 5 秒无脉冲 -> 停机待处理） */
static void stall_task(void)
{
    if (pos != pos_last) {                     /* 编码器有新脉冲 -> 在动 */
        pos_last = pos;
        last_move_ms = tick_ms;
    }

    if (!stall_flag && last_dir != 0 && duty_slider >= STALL_MIN_DUTY
        && (unsigned int)(tick_ms - last_move_ms) >= STALL_MS) {
        stall_flag = 1;                        /* 卡死：停机，等按模式键恢复 */
        set_motor(0, 0);
        duty_target = 0;
        duty_actual = 0;
        pwm_on = 0;
    }
}

/* ================== 8. 系统初始化 ================== */
static void sys_init(void)
{
    /* --- 输出脚先置低电平（防止配置瞬间产生毛刺） --- */
    P5 &= ~0x30;                  /* P5.4 / P5.5 锁存=0 */
    P3 &= ~0x80;                  /* P3.7 锁存=0 */

    /* --- IO 模式（手册：除 P3.0/P3.1 外，上电默认高阻，必须显式设置） --- */
    P1M1 = 0x01; P1M0 = 0x00;     /* P1.0 高阻输入(ADC)；P1.1 准双向(按键上拉) */
    P3M1 = 0x00; P3M0 = 0x80;     /* P3.2/P3.3 准双向(编码器)；P3.7 推挽输出 */
    P5M1 = 0x00; P5M0 = 0x30;     /* P5.4 / P5.5 推挽输出 */

    /* --- 定时器0：1ms 系统节拍（1T，模式0=16位自动重载） --- */
    AUXR |= 0x80;                 /* 定时器0 用 1T 模式 */
    TMOD &= 0xF0;                 /* 定时器0 模式0 */
    TH0 = 0xA2; TL0 = 0x40;       /* 65536-24000 = 41536 = 0xA240 -> 1ms */
    TR0 = 1;  ET0 = 1;

    /* --- 定时器1：软件 PWM 20kHz（1T，模式0） --- */
    AUXR |= 0x40;                 /* 定时器1 用 1T 模式 */
    TMOD &= 0x0F;                 /* 定时器1 模式0 */
    TH1 = (unsigned char)((65536 - PWM_PERIOD) >> 8);
    TL1 = (unsigned char)((65536 - PWM_PERIOD) & 0xFF);
    TR1 = 1;  ET1 = 1;

    /* --- ADC：右对齐 + 最慢时钟（24M/2/16=750kHz，读数最稳） --- */
    ADCCFG = 0x2F;                /* bit5=1 右对齐；低4位=15 分频最大 */
    ADC_CONTR = 0x80 | ADC_CH_POT;/* 打开 ADC 电源，选通道0 */

    /* --- 外部中断：编码器下降沿触发 --- */
    IT0 = 1;  IT1 = 1;            /* 下降沿 */
    EX0 = 1;  EX1 = 1;

    EA = 1;                       /* 开总中断 */
}

/* ================== 9. 主函数 ================== */
void main(void)
{
    unsigned char cnt10 = 0;
    unsigned char cnt100 = 0;
    unsigned int  last_tick;

    sys_init();

    /* 等 ADC 电源稳定 2ms */
    last_tick = tick_ms;
    while ((unsigned int)(tick_ms - last_tick) < 2);

    enter_mode(M_IDLE);           /* 开机默认：待机，电机不转 */

    while (1)
    {
        while (tick_ms == last_tick);   /* 等 1ms 节拍 */
        last_tick = tick_ms;

        ramp_task();                    /* 占空比斜坡 */

        if (++cnt10 >= 10) {            /* 每 10ms */
            cnt10 = 0;
            duty_slider_task();         /* 读滑条 */
            button_task();              /* 扫按键 */
            control_task();             /* 闭环控制 */

            if (++cnt100 >= 10) {       /* 每 100ms */
                cnt100 = 0;
                stall_task();           /* 堵转检测 */
            }
        }
    }
}

/**********************************************************************
 *  【编译与烧录说明】
 *  1. 编译：双击 build.bat（或命令行 sdcc -mmcs51 --model-small
 *     main_sdcc.c 后接 packihx main_sdcc.ihx > main_sdcc.hex）
 *  2. 烧录：STC-ISP 选 STC8G1K08；主频 24MHz；"P5.4/RST 用作复位脚"
 *     保持默认（不要勾选）；载入 main_sdcc.hex，点下载后给板子
 *     断电-上电一次即可。
 *
 *  【首次调试检查】
 *  a. 先只接 USB-TTL 烧录（不接电机动力线），确认可烧录运行。
 *  b. 进 ±5cm 模式，用尺量纱布实际行程：若明显偏差，按比例
 *     调整 PULSES_PER_CM（例：实际只走 4cm，则 166 → 166×5/4≈208）。
 *  c. 若方向定义相反（点模式键后往反方向计数），把 INT0/INT1
 *     两个中断里的 ++/-- 对调。
 *  d. 电机堵转保护触发后：松开工件，按一下模式键即可恢复。
 **********************************************************************/