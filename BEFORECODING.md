# BEFORECODING.md

Read and follow this file before implementing any code.  
It defines what code in this repository should look like.

## 1. Language and Standard

- **Use C++20 only**.
- Prefer modern C++ patterns (`constexpr`, RAII, strong enums, range-for, `std::optional`, etc.).
- Templates are allowed, but keep them simple:
  - prefer `concept` for constraints;
  - avoid complex SFINAE-heavy implementations.

## 2. Constants, Static Functions, and Header Boundaries

- For compile-time static constants, prefer `constexpr`.
- Unless template visibility requires header placement:
  - put static functions in `.cpp`;
  - put non-`constexpr` constants in `.cpp`.
- Keep headers minimal and avoid exposing implementation details.

## 3. Configuration vs Hardcoding

- Do not hardcode tunable runtime values in `main` or business logic.
- Application-level tunables (startup scene URL, loop interval, app name, paths, feature switches) must live in `Configuration`, loaded from:
  - command-line arguments, and/or
  - configuration files.
- Hardcoded values are acceptable for:
  - class metadata (class names, static class traits);
  - compile-time constants;
  - fixed identifiers that are not runtime tunables.

## 4. Singleton Initialization

- All singletons must be initialized in `main` with explicit startup ordering.
- Avoid implicit lazy initialization of critical singletons deep in runtime paths.

## 5. Inheritance and Type System Direction

- Avoid excessive type fragmentation; prefer shared abstract base classes.
- `Scene` is an `Item` container (architecture direction):
  - it loads and manages `Item` instances;
  - it creates/connects concrete `Item` instances based on JSON/QML configuration.
- `Item` is the base unit that can be attached to UI, rendered, and handle basic UI interactions.
- New features should preferably be implemented by extending `Item`.
  - If a requirement truly cannot be implemented this way, stop further feature expansion, record completed scope and blockers, and open an issue before proceeding with alternatives.

## 6. Update / fixedUpdate Allocation Discipline

- Never dynamically create per-object allocations every frame in `update` / `fixedUpdate`.
- If dynamic growth is needed:
  - reserve capacity in `initialize`;
  - expand only when reserved capacity is insufficient;
  - reclaim in `uninitialize` / `cleanup`.

## 7. Resource and Lifetime Model

- Prefer static/reused resources where practical.
- Item activation (resource loading) should be event-driven where needed, not only tied to a single “scene active” flag.
- Scene-to-scene transitions should be customizable (e.g., loading bars, fade in/out), controlled by JSON/QML.
- Multiple scene links are allowed; scene/item relationships may form a directed graph.
- Items should support residency/lifetime tracking (resident vs non-resident) when shared across scenes/graph paths.
- Large resources may load asynchronously; synchronize UI-visible state changes through Qt signal/slot flows.

## 8. Forbidden Qt6 Deprecated/Obsolete APIs

Do not use APIs marked Deprecated/Obsolete in Qt6 official documentation.  
Repository-level enforced replacement list:

- `QRegExp` → use `QRegularExpression`
- `QStringRef` / `QByteArrayRef` → use `QStringView` / `QByteArrayView`
- `Q_FOREACH` / `foreach` macros → use range-for
- `QSignalMapper` → use lambda-based direct signal connections
- `QDesktopWidget` → use `QScreen` / `QGuiApplication::screens()`
- `qsrand` / `qrand` → use `QRandomGenerator`
- `QDateTime::fromTime_t` / `toTime_t` → use `fromSecsSinceEpoch` / `toSecsSinceEpoch`

When Qt adds new deprecated/obsolete APIs, update this file and migrate code accordingly.
