# 项目区

这里用于存放**具体的作品项目**。学习主题（`01-` ~ `07-`）继续按能力组织内容；
做过的实际作品在这里按项目登记。

## 项目列表

| 编号 | 项目 | 状态 | 入口 |
| --- | --- | --- | --- |
| 01 | 桌面下理线盒 | 已完成 | [01-cad-3d-printing](../01-cad-3d-printing/) |
| 02 | 电气玩具（卷轴式打磨装置） | **暂停待重启**：硬件已焊完，主控芯片烧毁 | [02-electric-toy](02-electric-toy/) |

> 02 的完整设计文档、接线图、固件在独立仓库
> [Echo7023/electric-toy](https://github.com/Echo7023/electric-toy)，
> 本目录只放**进度、复盘与重启计划**。

## 约定

每个项目一个文件夹：

```text
projects/<项目名>/
├── README.md     # 目标、状态、进度、踩坑复盘
├── cad/          # 3D 模型 / CAD 源文件
├── firmware/     # 固件 / 嵌入式代码
├── bom.md        # 物料清单
└── media/        # 照片、视频记录
```

新建项目时可参考 [templates/project-template.md](../templates/project-template.md)。
