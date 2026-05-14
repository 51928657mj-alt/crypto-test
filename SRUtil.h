#ifndef SRUTIL_H
#define SRUTIL_H

#include "Types/BitTypes.h"
#include "CryptStruct.h"

class SRUtil {
public:
    SRUtil();
    virtual ~SRUtil();

    static void Crypt(CryptBaseStruct& cryptStruct, u::int8 cycle,
                       u::uint32 _n1,  u::uint32 _n2, const QList<u::uint32> *key, const QList<QList<u::uint8> > *table);

    static u::uint32 CryptCycle(CryptBaseStruct& cryptStruct, u::int8 cycle, u::uint32 _n1, u::uint32 _n2,
                         const QList<u::uint32> *key, const QList<QList<u::uint8> > *table );
};

#endif // SRUTIL_H
