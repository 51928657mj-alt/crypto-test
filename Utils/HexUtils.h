#ifndef HEXUTILS_H
#define HEXUTILS_H

#include <QChar>
#include "Types/BitTypes.h"
namespace u {
    u::uint8 ConvertToHex(const QChar data);
    void CreateHexDataInString(const void* data, int index, QString &hex);
}

#endif // HEXUTILS_H
