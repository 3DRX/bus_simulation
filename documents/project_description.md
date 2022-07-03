# 工程说明

## 开发语言

- 核心版（OJ版）：C
- 图形版：C++

## GUI库

[QT](https://www.qt.io/)，详细内容见开发环境说明。

### QT版本问题

在开发的过程中，不同小组成员的QT版本并不相同（Qt5和Qt6），因而本项目仅使用两个版本中完全相同的部分。

因此，对于Qt5和Qt6这两个版本而言，本项目的开发环境**完全一样**。

同时，项目的配置文件也在这两个版本下**都可以正常使用**。

## 开发环境说明

- Windows 10：使用 [QtCreator](https://www.qt.io/product/development-tools) IDE 开发
- MacOS 12.4：使用 [NeoVim](https://github.com/neovim/neovim) 0.7.0 开发
- Ubuntu 22.04 LTS：使用 [NeoVim](https://github.com/neovim/neovim) 0.8.0 开发

[CMake 配置文件](../CMakeLists.txt)

### 如何编译

- Windows：使用 QtCreator 打开 CMakeLists.txt 即可在 QtCreator 中编译运行
- Linux/MacOS：
    - 方法1
        - 同Windows
    - 方法2
        - [安装Qt](https://doc.qt.io/qt-5/gettingstarted.html)
        - 运行 [runcode.sh](../runcode.sh)


