# BEFORECODING.md

Read and follow this file before implementing any code.  
It defines what code in this repository should look like.

## 1. Language and Standard

- **Use C++20 only**.
- Prefer modern C++ patterns (`constexpr`, RAII, strong enums, range-for, etc.).
- Prefer Qt signal/slot integration over ad-hoc callback wiring when both are viable.
- Prefer Qt-provided types/utilities over `std` equivalents in application code.  
  C++20 requirement is about language features; it does not require using `std` library features everywhere.
- Templates are allowed, but keep them simple:
  - prefer `concept` for constraints;
  - avoid complex SFINAE-heavy implementations.
- Avoid lambda usage unless the function is truly one-off and very short.

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
  - one Scene should generally represent a narrative scene (not every camera shot);
  - Scene should support Scene nesting where useful for layered animation/composition.
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

## 9. Minimal Public Interfaces

- Keep public APIs as small as possible; every extra public method is a maintenance burden and a leaky abstraction.
- Prefer a **single unified entry point** over multiple near-identical methods (e.g., `setState(State)` instead of separate `start()` / `pause()` / `resume()` / `stop()` methods).
- Prefer **writable Q_PROPERTYs** over paired `Q_INVOKABLE` getter/setter methods — property bindings and the NOTIFY signal carry state changes for free.
- Use a generic `getValue` / `setValue` pattern for configuration access rather than one typed accessor per field; add a `Q_PROPERTY` only when QML binding is genuinely needed for a specific field.
- Avoid thin wrapper methods whose only body is `emit someSignal(...)` — emit the signal (or set the property) directly from the logical call site.
- Internal helpers that are not part of the externally observable contract belong in `private` (or the `.cpp` file); do not promote them to `public` just for convenience.

## 10. Runtime/Data Placement for QML Story Presentation

- Keep reusable QML constants (for example: expressions/emoji maps, animation script constants) in JSON resources first, with in-QML fallback defaults.
- Placeholder/non-real resources are only used when real assets (image/video/audio) are unavailable.
- Settings panels should be presented within the current scene flow (with scene animation), not through unrelated stack-page push navigation.

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
