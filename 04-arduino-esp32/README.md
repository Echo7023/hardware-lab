# 04 · Arduino 与 ESP32

**对应工具**：Arduino + ESP32
**核心价值**：让代码触及物理世界 —— 用程序驱动真实的电子元件。

## 环境状态（2026-10 装机完成）

| 层 | 状态 | 说明 |
| --- | --- | --- |
| Arduino IDE | 已装 | IDE 2.x |
| ESP32 板卡支持包 | 已装 | **3.3.12**（官方正式版）；核心与工具链已核对完整 |
| CH340 USB 驱动 | 已装 | 板载 USB 芯片是 CH340，本机驱动已就绪 |
| ESP32 开发板 | 待接入 | 套件下单中 → [docs/shopping-list.md](../docs/shopping-list.md) |

### 装机时踩的坑：官方包下载超时（后来自己好了）

**当时的症状**（2026-10-19）：`Failed to install platform: 'esp32:esp32:3.3.12'` / `Download failed: ... 185.199.110.133 ... timed out`
**原因**：官方包走 GitHub 发布资源（`objects.githubusercontent.com`），那天本机网络连不上这个 CDN。

**备用方案：换国内镜像（留着，官方再超时时用）**

1. IDE → 文件 → 首选项 → 附加开发板管理器网址，填入：

   ```text
   https://jihulab.com/esp-mirror/espressif/arduino-esp32/-/raw/gh-pages/package_esp32_index_cn.json
   ```

2. 开发板管理器搜 `esp32`，选**带 `-cn` 后缀**的版本安装（该源最高是 `3.3.10-cn`），装完**重启 IDE**。

**现状（2026-10-20 核实）**：官方源又能下载了，已换成 **官方 3.3.12**，`-cn` 那个版本已不在本机。

**核对安装是否完整**（两条路径都该存在）：

```text
%LOCALAPPDATA%\Arduino15\packages\esp32\hardware\esp32\3.3.12\   ← 核心 + boards.txt + installed.json
%LOCALAPPDATA%\Arduino15\packages\esp32\tools\esp-x32\2601\bin\  ← 编译器工具链（-cn 版对应 ...\2601-cn\）
```

> **源怎么配（结论）**：首选项里的源只决定「开发板管理器能列出 / 安装哪些版本」，**不影响编译上传** —— 那靠的是本地已装的核心。
> - 已装的 `3.3.12` 只存在于官方索引；jihulab 的 `-cn` 索引最高只到 `3.3.10-cn` → 只留 `-cn` 源也能正常干活，但管理器里**看不到 3.3.12**。
> - 所以：**别在开发板管理器里对 `esp32` 点「移除」**，否则 3.3.12 没了只能重下。
> - 想让管理器也跟得上最新版，就把官方源 `https://espressif.github.io/arduino-esp32/package_esp32_index.json` 一起留着当主源，jihulab 留作官方超时时的备用。

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
