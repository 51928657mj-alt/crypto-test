#ifndef CRYPTSTRUCT_H
#define CRYPTSTRUCT_H

#include "Types/BitTypes.h"
#include <QList>

struct CryptBaseStruct {
    u::uint32 N1;
    u::uint32 N2;
    u::uint32 CM1;
    u::uint32 R1;
    u::uint32 R2;
    u::uint32 CM2;
    u::uint8 ActiveKey;
    QList<QList<u::uint8> > ActiveTableList;

};

#endif // CRYPTSTRUCT_H
