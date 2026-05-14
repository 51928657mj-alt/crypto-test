#ifndef FILEPATH_H
#define FILEPATH_H


#include <QString>
#include "Types/BitTypes.h"


// Набор функций для работы с файловыми путями
namespace u {
    /// Проверка существования файла
    /// \param[in] path - путь к файлу
    /// \return Флаг существования
    u::bul IsFileExists(const QString& path);

    /// Получить абсолютный полный путь к файлу (без "." и "..")
    /// \note Если родительская директория файла
    ///       не существует, то возвращается пустая строка
    /// \param[in] path - путь к файлу
    /// \return Абсолютный полный путь к файлу
    QString GetFullPath(const QString& path);

    /// Проверка путей файлов на совпадение
    /// \param[in] path1 - первый путь к файлу
    /// \param[in] path2 - второй путь к файлу
    /// \return Флаг совпадения
    u::bul IsSamePath(const QString& path1, const QString& path2);
}


#endif // FILEPATH_H
