# 换电脑接入指南（新机器第一次用）

> 这份文档就是写给「换了台电脑、不知道从哪下手」的你的。
> 单位电脑 ↔ 家里电脑之间靠 GitHub 仓库同步内容；**最后一定会有一台是"新机器"**。
>
> 顺序照着做一遍，之后就只剩「开工 pull、收尾 push」两件事。

---

## 第 0 步：怎么在"还没配好的新电脑"上读到这份文档

新电脑还没 git、还没 SSH，所以**先别急着敲命令**。用下面任意一种方式把它打开：

- **最简单**：用手机或新电脑浏览器直接看
  <https://github.com/Echo7023/hardware-lab/blob/main/docs/new-machine-setup.md>
- 或者：新电脑先用 **HTTPS** 把仓库拉下来（会弹浏览器登录，不需要先配 SSH），再打开本文件
- 如果这台机器**以前配过**：先 `git pull`，确保你读到的是最新版

> 看完再回到下面，按顺序做。

---

## 第 1 步：确认装了 git

```powershell
git --version
```

没有输出就装：<https://git-scm.com/download/win>（一路默认即可）。

---

## 第 2 步：生成并登记 SSH key

> 完整说明和常见问题见 [setup-ssh.md](setup-ssh.md)，这里是精简版。

**每台电脑一套密钥，不要从别的机器复制私钥。**

```powershell
ssh-keygen -t ed25519 -C "hardware-lab"
```

- 提示保存路径：直接回车（默认 `C:\Users\<你>\.ssh\id_ed25519`）
- 提示口令：可留空；设了口令就要再配 ssh-agent（见 [setup-ssh.md](setup-ssh.md)）

把公钥复制到剪贴板：

```powershell
Get-Content $env:USERPROFILE\.ssh\id_ed25519.pub | Set-Clipboard
```

打开 <https://github.com/settings/keys> → **New SSH key** → 粘贴 → **Add SSH key**。

**如果这台机器的网络封 22 端口**（公司 / 学校网络常见），先创建 `C:\Users\<你>\.ssh\config`：

```text
Host github.com
  HostName ssh.github.com
  Port 443
  User git
```

验证：

```powershell
ssh -T git@github.com
```

看到 `Hi Echo7023! You've successfully authenticated...` 就成功了。
（后面那句 `does not provide shell access` 是正常的，不用管。）

---

## 第 3 步：配置 git 身份（两台机必须完全一致）

> 不做这一步，`git commit` 会直接报 `Author identity unknown`。

```powershell
git config --global user.name "Echo7023"
git config --global user.email "Echo7023@users.noreply.github.com"
```

检查：

```powershell
git config --global --get user.name
git config --global --get user.email
```

**两台电脑的姓名和邮箱必须一模一样**，否则提交作者对不上，GitHub 贡献图会认不出是同一个人。

---

## 第 4 步：克隆仓库

建议放在与另一台机器相同的层级目录，找起来一致（例如 `C:\MY\Self Projects\`）。

```powershell
git clone git@github.com:Echo7023/hardware-lab.git
```

如果第 0 步是用 HTTPS 克隆的，现在把远程切成 SSH：

```powershell
git remote set-url origin git@github.com:Echo7023/hardware-lab.git
git remote -v
```

`git remote -v` 输出以 `git@` 开头即为切换成功。

---

## 第 5 步：验证接入成功

```powershell
git status
git log --oneline -3
git ls-remote --heads origin
```

三条都没报错 = 这台机器已经接上了。

---

## 日常纪律（每次开工 / 收尾）

```text
开工（每次）：git pull            ← 先拿到另一台电脑推上来的内容
干活：        写笔记 / 放代码 / 存照片（照片放 media/，见下）
收尾（每次）：git add .  →  git commit -m "说明"  →  git push
```

**关键就一句：收尾必须 push。**

一个人两台机，冲突只会发生在「在这台干了活没 push，就跑去另一台干活」。
只要守住「开工 pull、收尾 push」，基本不会遇到冲突。

- 素材（照片 / 视频 / 模型）**只放本地，不进仓库**：放进 `media/` 子目录，
  [.gitignore](../.gitignore) 已把它整体排除，`git add .` 不会误收。
- 每次开工先看 [README.md](../README.md) 顶部的「当前状态」，就知道接着干什么。

---

## 从单位回家 / 从家回单位，具体做什么

**到另一台电脑（开工）**

1. `git pull` —— 把这边的新内容拉下来
2. 打开 [README.md](../README.md)，看顶部「当前状态」的「下一步」

**离开前（收尾）**

1. `git add .` → `git commit -m "这次干了什么"` → `git push`
2. `git status` 确认显示 `working tree clean`，且与 origin 同步

**真遇到冲突怎么办**

- 别慌，先 `git status` 看标红的文件
- 简单情况：手动改好 → `git add` → `git commit`
- 不确定的话，把 `git status` 和报错原文发出来，一起理

---

## 常见坑

| 现象 | 原因 / 怎么办 |
| --- | --- |
| `Author identity unknown` | 第 3 步没做，或两台机姓名 / 邮箱不一致 |
| `Permission denied (publickey)` | 公钥没加到 GitHub，或没建 443 端口的 `config` |
| 提示输密码 / 输 Token | 远程还是 HTTPS，执行第 4 步的 `set-url` |
| `git pull` 提示 diverged | 两边各有一个提交，别硬来，叫人一起看 |
| 照片视频被 `git add .` 收进去 | 应该放 `media/` 目录（已被 .gitignore 排除） |

---

## 一分钟自查清单

- [ ] `git --version` 有输出
- [ ] 生成了 SSH key，公钥已登记到 GitHub
- [ ] `ssh -T git@github.com` 显示 `successfully authenticated`
- [ ] `git config --global user.name` / `user.email` 与另一台机一致
- [ ] 仓库已克隆，`git remote -v` 是 `git@` 开头
- [ ] `git pull` 和 `git push` 都跑得通
