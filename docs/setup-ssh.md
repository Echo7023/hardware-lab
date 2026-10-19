# 在 Windows 上配置 SSH key（免密码推拉 GitHub）

用途：配置一次后，`git push` / `git pull` 不再需要输入密码或 Token。

> 前提：Windows 10/11 自带 OpenSSH。若在终端里敲 `ssh` 提示找不到命令，先去
> 「设置 → 应用 → 可选功能 → 添加功能」安装 **OpenSSH 客户端**。

## 步骤

### 1. 检查是否已有密钥

```powershell
Get-ChildItem $env:USERPROFILE\.ssh
```

若已存在 `id_ed25519` 和 `id_ed25519.pub`，说明已有密钥，可直接跳到第 4 步。

### 2. 生成密钥对

```powershell
ssh-keygen -t ed25519 -C "你的邮箱地址"
```

- 提示 `Enter file in which to save the key`：直接回车，默认存到 `C:\Users\<你>\.ssh\id_ed25519`
- 提示 `Enter passphrase`：可设一个口令保护私钥（推荐），也可直接回车留空
- 生成后得到两个文件：`id_ed25519`（私钥，保密）和 `id_ed25519.pub`（公钥）

### 3. （可选）让系统记住口令

如果第 2 步设了口令，可用 ssh-agent 记住它，避免每次输入。以**管理员身份**打开 PowerShell：

```powershell
Get-Service ssh-agent | Set-Service -StartupType Automatic
Start-Service ssh-agent
ssh-add $env:USERPROFILE\.ssh\id_ed25519
```

### 4. 复制公钥

```powershell
Get-Content $env:USERPROFILE\.ssh\id_ed25519.pub | Set-Clipboard
```

这会把公钥内容直接复制到剪贴板。

### 5. 在 GitHub 上登记公钥

1. 打开 https://github.com/settings/keys
2. 点 **New SSH key**
3. Title 随便填（例如 `我的 Windows 电脑`）
4. Key 一栏粘贴刚才复制的公钥
5. 点 **Add SSH key**

### 6. 测试连接

```powershell
ssh -T git@github.com
```

- 第一次会问 `Are you sure you want to continue connecting?`，输入 `yes` 回车
- 看到 `Hi Echo7023! You've successfully authenticated...` 即表示成功

### 7. 用 SSH 克隆仓库

```powershell
git clone git@github.com:Echo7023/hardware-lab.git
```

如果本地已经用 HTTPS 克隆过，切换远程地址即可：

```powershell
git remote set-url origin git@github.com:Echo7023/hardware-lab.git
git remote -v
```

`git remote -v` 输出以 `git@` 开头即为切换成功。

## 常见问题

- **`ssh` 命令找不到**：「设置 → 应用 → 可选功能 → 添加功能」安装 OpenSSH 客户端。
- **22 端口被网络屏蔽**：在 `C:\Users\<你>\.ssh\config` 里加入以下内容，改用 443 端口：

  ```text
  Host github.com
    HostName ssh.github.com
    Port 443
    User git
  ```

- **私钥安全**：`id_ed25519`（没有 `.pub` 后缀的那个）是私钥，**切勿外传或提交到 Git 仓库**。
