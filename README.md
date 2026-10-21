# 硬件实验室 (hardware-lab)

在家搭建的个人硬件实验室。目标是把一个想法走完整条链路：

> **想法（脑海） → 软件 → 电子 → 机械 → 物理现实**

这里存放实验室的一切：学习笔记、工程文件、项目记录与媒体素材，方便在任何一台电脑上继续。

## 当前状态

> 每次开工 / 收尾顺手更新下面几行，让仓库自己告诉你「接着干什么」。

- **当前阶段**：打基础 —— 01 已实践；02 / 03 器材齐全；04 / 06 只差一块 ESP32
- **设备就绪度**：7 个主题中 3 个已就绪（01 已实践 / 02 / 03），04 / 06 只缺 MCU → [docs/tools.md](docs/tools.md)
- **已完成**：桌面下理线盒（[01](01-cad-3d-printing/)）
- **正在做**：待下单 ESP32 全套套件（36 件）
- **第 2 个实践项目**：电气玩具（卷轴式打磨装置）—— 硬件已焊完，主控芯片烧毁，**暂停待重启** → [projects/02-electric-toy](projects/02-electric-toy/)
- **下一步**：① 下单 → [docs/shopping-list.md](docs/shopping-list.md)；② 家里电脑接入 → [docs/new-machine-setup.md](docs/new-machine-setup.md)；③ 到货前用 02 在这几块洞洞板上练焊点
- **跨设备交接**：换电脑先读 [docs/progress.md](docs/progress.md) —— 详细快照（软件环境已就绪、店家资料怎么拿到手、第 2 个实践项目的重启计划）

## 目录导航

| 目录 | 用途 |
| --- | --- |
| [docs/](docs/) | 学习路线、工具清单、采购清单等通用文档 |
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
换新电脑（仅一次）：git clone git@github.com:Echo7023/hardware-lab.git
开工（每次）：     git pull
干活：             写笔记 / 放代码 / 存照片
收尾（每次）：     git add .  →  git commit -m "说明"  →  git push
```

**两台电脑的纪律**：开工先 `pull`，收尾必 `push`。一个人两台机，只要守住这条就不会冲突——冲突只会发生在「在这台干了活没 push，就跑去另一台干活」。

每台电脑首次使用需各自配置一次 git 身份和 SSH key（**姓名 / 邮箱两台机必须完全一致**，否则提交作者对不上、贡献图会认不出是同一个人）。详细步骤见 [docs/setup-ssh.md](docs/setup-ssh.md)。

## 说明

### 媒体素材规则

- **素材一律留在本地，不推送到 GitHub**：照片、视频、录屏、录音、3D 模型。
- **放哪**：放进 `media/` 子目录（项目里是 `<项目>/media/`）。[.gitignore](.gitignore) 已把该目录整体排除，`git add .` 不会误收。
- **例外**：极小的截图若要随笔记一起入库，别放进 `media/`，单独 `git add` 即可。
- 3D 模型里只有 `*.step` / `*.stp`（文本格式、体积小）适合入库，放 `cad/` 而不要放 `media/`。

### 笔记约定

- **日常速记不套模板**：想到什么直接写，够用就行。
- **主题 `README.md` 只放结论、清单和链接**，不放过程流水账。
- [templates/note-template.md](templates/note-template.md) 是**可选**的归档结构，需要给别人看或做复盘时才用。

### 组织方式

- 当前按「学习主题」组织内容，推进顺序按设备就绪度分批（见 [docs/learning-path.md](docs/learning-path.md)）；开始做具体作品后，再启用 `projects/` 的「按项目」组织方式。
