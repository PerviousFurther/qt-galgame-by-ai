/**
 * @file CodingStandards.h
 * @brief Coding standards and conventions for the qt-galgame project
 * 
 * ==================== CODING STANDARDS ====================
 * ⚠️ IMPORTANT: Use Qt containers, NOT STL containers! ⚠️
 * 
 * This project uses Qt containers and types consistently throughout.
 * 
 * DO NOT USE:
 * - std::string      → Use QString instead
 * - std::vector      → Use QList or QVector instead
 * - std::map         → Use QMap or QHash instead
 * - std::set         → Use QSet instead
 * - std::variant     → Use QVariant instead
 * - std::shared_ptr  → Use QSharedPointer instead
 * - std::unique_ptr  → Use QScopedPointer instead
 * - Callback logic   → Prefer Qt signals/slots
 * - std::mutex       → Use QMutex instead
 * - std::thread      → Use QThread instead (preferred)
 * 
 * REASONS:
 * 1. Consistency with Qt framework conventions
 * 2. Better integration with Qt's meta-object system
 * 3. Automatic memory management with Qt's parent-child relationship
 * 4. Better QML integration
 * 5. Cross-platform compatibility
 * 
 * EXCEPTIONS:
 * - Standard library algorithms (std::find, std::sort, etc.) can be used
 * - Standard exceptions (std::runtime_error, etc.) can be used
 * - Standard library utilities where Qt doesn't provide alternatives
 * 
 * Always prefer Qt equivalents when available!
 * ==========================================================
 */

#ifndef CODINGSTANDARDS_H
#define CODINGSTANDARDS_H

#include "codingstyle.h" // include/codingstyle.h

#endif // CODINGSTANDARDS_H
