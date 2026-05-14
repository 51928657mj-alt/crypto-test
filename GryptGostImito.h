#ifndef GRYPTGOSTIMITO_H
#define GRYPTGOSTIMITO_H

#include <QList>
#include "Types/BitTypes.h"
#include "CryptStruct.h"

class GryptGostImito {
public:
    GryptGostImito();
    virtual ~GryptGostImito();

    void ComputeImito(QList<u::uint32> *dataIn, QList<u::uint32> *key, QList<QList<u::uint8> > *table);
    QList<CryptBaseStruct>* GetCryptStruct();
    QList<u::uint32> GetCM3();
    QList<u::uint32> GetCM4();
    QList<u::uint32> GetCM5();
    QList<u::uint32> GetN1();
    QList<u::uint32> GetN2();
    u::uint32 GetImito();

private:
    QList<CryptBaseStruct>* m_cryptStructList;
    QList<u::uint32> m_CM3;
    QList<u::uint32> m_CM4;
    QList<u::uint32> m_CM5;
    QList<u::uint32> m_N1;
    QList<u::uint32> m_N2;
    u::uint32 m_imito;
    int m_blocks;
};

#endif // GRYPTGOSTIMITO_H
