# Grpoverty 游戏项目

一个使用 C++ 和 EasyX 图形库开发的 2D 游戏项目。

## 项目简介

Grpoverty 是一个具有完整游戏功能的 2D 游戏，包含菜单系统、游戏玩法、排行榜、音乐设置等功能，是当时我们脱贫脱单不脱发组做的一个简单的小游戏。

## 功能特性

- 🎮 完整的游戏菜单系统
- 🏆 排行榜功能，记录玩家成绩
- 🎵 背景音乐和音效系统
- ⚙️ 游戏设置界面
- 🎨 不精美的游戏界面和动画效果
- 👤 玩家角色控制和动画
- 🎯 敌人 AI 和碰撞检测

## 技术栈

- **编程语言**: C++
- **图形库**: EasyX Graphics Library
- **开发环境**: Visual Studio
- **音频**: Windows Multimedia API (Winmm.lib)
- **图像处理**: MSIMG32.LIB

## 系统要求

- Windows 操作系统
- Visual Studio 2019 或更高版本
- EasyX 图形库
- 支持 DirectX 的显卡

## 安装和运行

### 环境配置

1. 安装 Visual Studio
2. 下载并安装 EasyX 图形库
   - 访问 [EasyX 官网](https://easyx.cn/) 下载最新版本
   - 按照官方文档进行安装配置

### 编译运行

1. 克隆或下载项目到本地

```bash
git clone https://github.com/KBchulan/Grpoverty.git
cd Grpoverty
```

2. 使用 Visual Studio 打开 `Grpoverty.sln` 解决方案文件
3. 确保项目配置正确：

   - 平台：x64 或 x86（根据您的系统）
   - 配置：Debug 或 Release
4. 编译并运行项目（F5 或 Ctrl+F5）

## 游戏控制

- **方向键**: 控制角色移动
  - ↑ 上移
  - ↓ 下移
  - ← 左移
  - → 右移
- **鼠标**: 菜单导航和选择

## 项目结构

```
Grpoverty/
├── main.cpp              # 主程序文件
├── Grpoverty.vcxproj     # Visual Studio 项目文件
├── img/                  # 图像资源文件夹
│   ├── player_*.png      # 玩家角色动画帧
│   ├── enemy_*.png       # 敌人动画帧
│   ├── ui_*.png/jpg      # 用户界面元素
│   └── background_*.jpg  # 背景图片
├── mus/                  # 音频资源文件夹
│   ├── bgm*.mp3         # 背景音乐
│   ├── eat.mp3          # 音效文件
│   └── hit.wav          # 碰撞音效
└── rank.txt             # 排行榜数据文件
```

## 游戏界面

游戏包含以下主要界面：

1. **主菜单** (flag = 1) - 游戏启动界面
2. **排行榜** (flag = 2) - 显示玩家成绩
3. **音乐设置** (flag = 3) - 背景音乐选择
4. **设置界面** (flag = 4) - 游戏参数配置
5. **游戏界面** (flag = 5) - 主要游戏玩法
6. **游戏选项** (flag = 6) - 游戏模式选择
7. **制作名单** (flag = 7) - 开发团队信息
8. **暂停界面** (flag = 8) - 游戏暂停菜单

## 开发说明

### 主要类结构

- `Animation`: 动画播放类，处理精灵动画
- `Player`: 玩家角色类，处理玩家输入和渲染
- `paiHang`: 排行榜数据结构

### 关键功能

- **透明图像渲染**: 使用 `putimage_alpha()` 函数
- **动画系统**: 基于帧的动画播放
- **输入处理**: 键盘和鼠标事件处理
- **数据持久化**: 排行榜数据文件读写

## 联系方式

如有问题或建议，请通过以下方式联系：

- 项目 Issues: [GitHub Issues](项目地址/issues)
- 邮箱: 18737519552@163.com
