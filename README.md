# 硬件实验室 (hardware-lab)

在家搭建的个人硬件实验室。目标是把一个想法走完整条链路：

> **想法（脑海） → 软件 → 电子 → 机械 → 物理现实**

这里存放实验室的一切：学习笔记、工程文件、项目记录与媒体素材，方便在任何一台电脑上继续。

## 目录导航

| 目录 | 用途 |
| --- | --- |
| [docs/](docs/) | 学习路线、工具清单等通用文档 |
| [01-cad-3d-printing/](01-cad-3d-printing/) | CAD 设计与 3D 打印 |
| [02-soldering/](02-soldering/) | 焊接与电子装配 |
| [03-measurement/](03-measurement/) | 测量与调试 |
| [04-arduino-esp32/](04-arduino-esp32/) | Arduino / ESP32 嵌入式 |
| [05-raspberry-pi/](05-raspberry-pi/) | 树莓派 / Linux / 网络 |
| [06-motors-sensors/](06-motors-sensors/) | 电机、传感器与控制 |
| [07-mechanical-tools/](07-mechanical-tools/) | 机械工具与手工 |
| [projects/](projects/) | 【预留】具体作品区 |
| [templates/](templates/) | 笔记与项目模板 |

## 学习主题

| 编号 | 主题 | 对应工具 | 要练成的能力 |
| --- | --- | --- | --- |
| 01 | CAD 与 3D 打印 | 3D 打印机 | 把 CAD 设计变成实体零件 |
| 02 | 焊接与电子装配 | 烙铁站 | 组装和修理电子设备 |
| 03 | 测量与调试 | 万用表 + 示波器 | 学会测量现实 |
| 04 | Arduino 与 ESP32 | 开发板 | 让代码触及物理世界 |
| 05 | 树莓派 / Linux | Raspberry Pi | Linux、网络、边缘计算 |
| 06 | 电机与传感器 | 电机 + 传感器 | 运动、反馈、控制 |
| 07 | 机械工具与手工 | 基本机械工具 | 切割、钻孔、组装、改造 |

## 每日工作流（跨设备）

```text
换新电脑（仅一次）：git clone https://github.com/Echo7023/hardware-lab.git
开始干活：         git pull
干活：             写笔记 / 放代码 / 存照片
收尾：             git add .  →  git commit -m "说明"  →  git push
```

每台电脑首次使用需配置一次凭据（推荐 SSH key，配好后无需再输入密码/Token）。详细步骤见 [docs/setup-ssh.md](docs/setup-ssh.md)。

## 说明

- 大文件（STL / STEP / 视频）暂时不纳入版本管理，规则已在 [.gitignore](.gitignore) 与 [.gitattributes](.gitattributes) 中预留，将来可切换到 Git LFS。
- 当前按「学习主题」组织内容；开始做具体作品后，再启用 `projects/` 的「按项目」组织方式。
