#include "FilePath.h"

#include <QFileInfo>


u::bul u::IsFileExists(const QString& path) {
    return QFileInfo(path).exists() ? u::yes : u::no;
}


 QString u::GetFullPath(const QString& path) {
     QFileInfo fi(path);
     if (fi.exists()) {
         return fi.canonicalFilePath();
     }

     QFileInfo di(fi.path());
     if (!di.exists()) {
         return QString();
     }

     return di.canonicalFilePath() + "/" + fi.fileName();
 }


 u::bul u::IsSamePath(const QString& path1, const QString& path2) {
    Qt::CaseSensitivity param = Qt::CaseInsensitive;

#ifdef Q_OS_LINUX
    param = Qt::CaseSensitive;
#endif // Q_OS_LINUX

    return (QString::compare(GetFullPath(path1),
                             GetFullPath(path2), param) == 0) ? u::yes : u::no;
 }
