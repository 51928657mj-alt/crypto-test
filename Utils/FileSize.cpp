#include "FileSize.h"

#include <QFileInfo>


u::uint64 u::GetFileSize(const QString& path) {
    return QFileInfo(path).size();
}


u::bul u::CheckFileSize(const QString& path, u::uint64 size) {
    return (GetFileSize(path) == size) ? u::yes : u::no;
}


u::bul u::CheckMinFileSize(const QString& path, u::uint64 size) {
    return (GetFileSize(path) >= size) ? u::yes : u::no;
}


u::bul u::CheckMaxFileSize(const QString& path, u::uint64 size) {
    return (GetFileSize(path) <= size) ? u::yes : u::no;
}


u::bul u::CheckRangeFileSize(const QString& path, u::uint64 min, u::uint64 max) {
    if (u::CheckMinFileSize(path, min) != u::yes) {
        return u::no;
    }

    if (u::CheckMaxFileSize(path, max) != u::yes) {
        return u::no;
    }

    return u::yes;
}


u::bul u::CheckDivFileSize(const QString& path, u::size val) {
    return (GetFileSize(path) % val == 0) ? u::yes : u::no;
}
