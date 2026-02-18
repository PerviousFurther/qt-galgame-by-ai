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
 * - std::mutex       → Use QMutex instead
 * - std::thread      → Use QThread instead (preferred)
 * - std::chrono      → Use QElapsedTimer instead
 * - std::iostream    → Use QDebug / qDebug() / qWarning() instead
 * - std::fstream     → Use QFile instead
 * 
 * ALLOWED std:: USAGE:
 * - std::function    → Qt doesn't provide a direct alternative
 * - Standard exceptions (std::runtime_error, etc.)
 * - Standard library algorithms (std::find, std::sort, etc.) when needed
 * 
 * REASONS:
 * 1. Consistency with Qt framework conventions
 * 2. Better integration with Qt's meta-object system
 * 3. Automatic memory management with Qt's parent-child relationship
 * 4. Better QML integration
 * 5. Cross-platform compatibility
 * 6. Reduced dependency on platform-specific STL implementations
 * 
 * Always prefer Qt equivalents when available!
 * ==========================================================
 */

#ifndef CODINGSTANDARDS_H
#define CODINGSTANDARDS_H

// This header is intentionally empty - it serves as documentation
// Include this file at the top of new files to see the coding standards

#endif // CODINGSTANDARDS_H