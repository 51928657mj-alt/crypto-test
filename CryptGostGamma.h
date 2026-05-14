#ifndef CRYPTGOSTGAMMA_H
#define CRYPTGOSTGAMMA_H

#include "CryptGost.h"

class CryptGostGamma : public CryptGost {

public:
    CryptGostGamma(QList<u::uint32> *dataIn, QList<u::uint32> *key, QList<QList<u::uint8> > *table, QList<u::uint32> *iv);
    virtual ~CryptGostGamma();

    void Crypt();
    void Decrypt();
    u::uint32 GetN5() const;
    u::uint32 GetN6() const;

    u::uint32 GetN1Iv() const;
    u::uint32 GetN2Iv() const;
    QList<u::uint32> GetN3() const;
    QList<u::uint32> GetN4() const;
    QList<u::uint32> GetDataSumN1() const;
    QList<u::uint32> GetDataSumN2() const;
    QList<u::uint32> GetCM5() const;
    QList<CryptBaseStruct> GetCryptIv() const;
    QList<CryptBaseStruct>* GetCryptStructList() const;

private:
    void CryptCore();

private:
    u::uint32 m_N5; //const
    u::uint32 m_N6; //const
    u::uint32 m_N1Iv;
    u::uint32 m_N2Iv;
    QList<u::uint32> m_dataSumN1;
    QList<u::uint32> m_dataSumN2;
    QList<u::uint32> m_N3;
    QList<u::uint32> m_N4;
    QList<u::uint32> m_CM5;
    QList<CryptBaseStruct> m_cryptIv;
    QList<CryptBaseStruct>* m_cryptStructList;

};

#endif // CRYPTGOSTGAMMA_H
