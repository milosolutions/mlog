#pragma once

#include <QDebug>

/*!
 * Convenience overload for QDebug - it can now print \a string to \a debug
 * sink.
 */
QDebug operator<<(QDebug debug, const std::string &string);
