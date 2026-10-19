# 04 · Arduino 与 ESP32

**对应工具**：Arduino + ESP32
**核心价值**：让代码触及物理世界 —— 用程序驱动真实的电子元件。

## 环境状态（2026-10 装机完成）

| 层 | 状态 | 说明 |
| --- | --- | --- |
| Arduino IDE | 已装 | IDE 2.x |
| ESP32 板卡支持包 | 已装 | **3.3.10-cn**（国内镜像版） |
| CH340 USB 驱动 | 已装 | 板载 USB 芯片是 CH340，本机驱动已就绪 |
| ESP32 开发板 | 待接入 | 套件下单中 → [docs/shopping-list.md](../docs/shopping-list.md) |

### 装机时踩的坑：官方包下载超时 → 改用国内镜像

**症状**：`Failed to install platform: 'esp32:esp32:3.3.12'` / `Download failed: ... 185.199.110.133 ... timed out`
**原因**：官方包走 GitHub 发布资源（`objects.githubusercontent.com`），本机网络连不上这个 CDN。

**解法（已生效，换电脑照做）**

1. IDE → 文件 → 首选项 → 附加开发板管理器网址，填入：

   ```text
   https://jihulab.com/esp-mirror/espressif/arduino-esp32/-/raw/gh-pages/package_esp32_index_cn.json
   ```

2. 开发板管理器搜 `esp32`，选**带 `-cn` 后缀**的版本安装（如 `3.3.10-cn`），装完**重启 IDE**。
3. **关掉「自动更新」** —— 自动更新会去取非 `-cn` 的包，同样会失败。

**核对安装是否完整**（两条路径都该存在）：

```text
%LOCALAPPDATA%\Arduino15\packages\esp32\hardware\esp32\3.3.10-cn\   ← 核心 + boards.txt
%LOCALAPPDATA%\Arduino15\packages\esp32\tools\esp-x32\2601-cn\bin\  ← 编译器工具链
```

> 版本差异说明：想要 3.3.12 但装成了 3.3.10-cn，**对 ESP32 Dev Module 入门没有影响**，不必重装。

### 板子插上电脑后的三个坑

上传前先做两个选择：开发板选 **ESP32 Dev Module**，端口选设备管理器里新出现的 **`USB-SERIAL CH340 (COMx)`**。然后上传示例 `Blink` 验证。

| 现象 | 怎么办 |
| --- | --- |
| 端口列表里没有 COM 口 | 先确认 USB 线**能传数据**（有些线只供电），驱动已就绪，优先换线 |
| 报 `Timed out waiting for packet header` | 按住板子 **BOOT** 键再点上传，出现进度后松手 |
| 报端口被占用 / 打不开 | 先关掉串口监视器再上传 |

## 学习目标

- [ ] GPIO：点亮第一颗 LED
- [ ] 数字输入输出、按键、PWM
- [ ] 模拟输入：读取电位器、光敏等
- [ ] 串口通信与调试打印
- [ ] 总线：I2C / SPI 接传感器与屏幕
- [ ] ESP32：Wi-Fi / 蓝牙，做一个联网小玩意

## 笔记与素材

- 代码放到本项目下的 `firmware/` 或 `code/`
- 笔记放到 `notes/`
- 照片、视频放到 `media/`（只留本地，不进仓库）
