# qt-galgame-by-ai

基于 **C++20 + Qt6** 的 Galgame（视觉小说）引擎项目。

## 项目简介

本项目提供一个可扩展的视觉小说运行框架，核心目标包括：

- 场景与 Item 组织管理
- 资源统一加载与缓存
- 配置与运行时参数管理
- 面向后续对话系统、动画系统与小游戏能力扩展

## 目录结构

```text
.
├── CMakeLists.txt
├── include/          # 头文件
├── src/              # 源码实现
├── resources/        # 配置与QML/资源文件
└── scripts/          # 环境安装脚本
```

## 构建要求

- CMake >= 3.16
- 支持 C++20 的编译器
- Qt6（Core / Qml / Quick / Gui / Multimedia）

Ubuntu/Debian 可先执行：

```bash
./scripts/install_qt6_deps_ubuntu.sh
```

## 构建与运行

```bash
mkdir -p build
cd build
cmake ..
cmake --build .
./bin/qt-galgame-by-ai
```

> 若 CMake 找不到 Qt6，请设置 `Qt6_DIR` 或 `CMAKE_PREFIX_PATH`。

## 开发约定

开始开发前请先阅读并遵循：

- `BEFORECODING.md`

## 许可证

当前仓库未单独声明开源许可证，请在使用前确认项目约束。
