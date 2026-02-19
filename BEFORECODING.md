# BEFORECODING.md

在开始实现任何功能前，请先遵守本文件。它定义了仓库代码“应该长什么样”。

> English summary: This document is the mandatory coding contract. Use C++20, modern C++ practices, configuration-driven tunables, explicit singleton initialization in `main`, and avoid Qt deprecated APIs. Prefer extending `Item` for new features and discuss blockers via issue when architecture constraints cannot be met.

## 1. 语言与标准

- **强制使用 C++20**。
- 强烈推荐 modern C++ 写法（`constexpr`、范围 for、RAII、强类型枚举、`std::span`/`std::optional` 等）。
- 不抵制模板；但模板不应过度复杂：
  - 需要约束时优先 `concept`；
  - 避免复杂 SFINAE 体操。

## 2. 常量、静态函数与头文件边界

- 编译期静态常量优先 `constexpr`。
- 除非模板相关（必须在头文件可见），否则：
  - 静态函数放在 `.cpp`；
  - 非 `constexpr` 常量放在 `.cpp`。
- 头文件保持最小暴露面，避免无意义内联与实现细节泄漏。

## 3. 配置与硬编码

- 避免在 `main` 或业务逻辑中硬编码可调参数。
- 所有应用级可调项（启动场景、循环间隔、应用名、路径、开关）应进入 `Configuration`，并通过：
  - 命令行，或
  - 配置文件
  加载。
- 可以硬编码的内容：
  - 类 metadata（类名、静态类属性）；
  - 编译期常量；
  - 与运行配置无关的固定标识。

## 4. 单例初始化规则

- 所有单例必须在 `main` 中完成初始化顺序编排。
- 禁止在“深层运行路径”里隐式懒初始化关键单例。

## 5. 继承与类型体系

- 避免过度细分互斥类型，优先统一抽象基类。
- `Scene` 作为 `Item` 容器（架构方向）：
  - 负责装载 `Item`；
  - 根据 JSON/QML 配置完成连接与实例化。
- `Item` 作为可挂载到 UI 的可渲染/可交互实体基类。
- 后续功能优先通过扩展 `Item`（继承）实现。
  - 若确认无法按此路线实现：先停止继续扩展实现，记录完成点/阻塞点，并提 issue 讨论替代方案后再继续。

## 6. Update / fixedUpdate 内存纪律

- 严禁在 `update` / `fixedUpdate` 这类每帧函数中反复动态创建对象。
- 需要动态数据时：
  - 在 `initialize` 预留容量；
  - 仅在容量不足时扩容；
  - 在 `uninitialize` / `cleanup` 中回收。

## 7. 资源与生命周期

- 推荐资源静态化与复用（本项目不是通用交互引擎）。
- `Item` 激活（资源加载）应可由事件驱动，不应只和 Scene 激活单一绑定。
- 支持 Scene 多连接、可定制切换（读条/淡入淡出等，可由 Item + 配置驱动）。
- `Item`/`Scene` 连接关系可视为有向图：
  - 区分常驻与非常驻；
  - 跟踪生命周期；
  - 可从起点图遍历触发初始化。
- 大资源可异步读取，使用信号槽同步资源状态到 UI。

## 8. Qt6 Deprecated API 禁止项（仓库执行清单）

> 禁止使用 Qt6 官方文档中标记为 Deprecated/Obsolete 的 API。以下为本仓库强制排查清单（随 Qt 版本演进需更新）：

- `QRegExp`（改用 `QRegularExpression`）
- `QStringRef` / `QByteArrayRef`（改用 `QStringView` / `QByteArrayView`）
- `Q_FOREACH` / `foreach` 宏（改用 range-for）
- `QSignalMapper`（改用 lambda + 直接连接）
- `QDesktopWidget`（改用 `QScreen` / `QGuiApplication::screens()`）
- `qsrand` / `qrand`（改用 `QRandomGenerator`）
- `QDateTime::fromTime_t` / `toTime_t`（改用 `fromSecsSinceEpoch` / `toSecsSinceEpoch`）

如遇 Qt 官方文档新增弃用项，必须同步更新本文件并迁移代码。
