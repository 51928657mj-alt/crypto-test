#ifndef FILESIZE_H
#define FILESIZE_H


#include <QString>
#include "Types/BitTypes.h"


// Набор функций для работы с файловыми размерами
namespace u {
    /// Получить размер файла в байтах
    /// \note Если файл не существует, возвращается 0
    /// \param[in] path - путь к файлу
    /// \return Размер файла в байтах
    u::uint64 GetFileSize(const QString& path);

    /// Проверить размер файла
    /// \param[in] path - путь к файлу
    /// \param[in] size - проверяемый размер файла в байтах
    /// \return Флаг успешного проведения проверки
    u::bul CheckFileSize(const QString& path, u::uint64 size);

    /// Проверить минимальный размер файла
    /// \param[in] path - путь к файлу
    /// \param[in] size - проверяемый размер файла в байтах
    /// \return Флаг успешного проведения проверки
    u::bul CheckMinFileSize(const QString& path, u::uint64 size);

    /// Проверить максимальный размер файла
    /// \param[in] path - путь к файлу
    /// \param[in] size - проверяемый размер файла в байтах
    /// \return Флаг успешного проведения проверки
    u::bul CheckMaxFileSize(const QString& path, u::uint64 size);

    /// Проверить размер файла в заданных пределах
    /// \param[in] path - путь к файлу
    /// \param[in] min - минимальный размер файла в байтах
    /// \param[in] max - максимальный размер файла в байтах
    /// \return Флаг успешного проведения проверки
    u::bul CheckRangeFileSize(const QString& path, u::uint64 min, u::uint64 max);

    /// Проверить размер файла на кратность указанному числу
    /// \param[in] path - путь к файлу
    /// \param[in] val - число для проверки кратности
    /// \return Флаг успешного проведения проверки
    u::bul CheckDivFileSize(const QString& path, u::size val);
}


#endif // FILESIZE_H
