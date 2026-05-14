#ifndef CRYPTGOSTGAMMFEEDBACK_H
#define CRYPTGOSTGAMMFEEDBACK_H

#include "CryptGost.h"

class CryptGostGammFeedBack : public CryptGost {
public:
    CryptGostGammFeedBack(QList<u::uint32> *dataIn, QList<u::uint32> *key, QList<QList<u::uint8> > *table, QList<u::uint32> *iv);
    virtual ~CryptGostGammFeedBack();

    void Crypt();
    void Decrypt();

    QList<CryptBaseStruct>* GetCryptStructList() const;
    QList<u::uint32> GetCM5() const;
    QList<u::uint32> GetN1() const;
    QList<u::uint32> GetN2() const;

private:
    void CryptOrDecryptCore();

private:
    QList<CryptBaseStruct>* m_cryptStructList;
    QList<u::uint32> m_CM5;
    QList<u::uint32> m_N1;
    QList<u::uint32> m_N2;
    bool m_Encrypt;
};

#endif // CRYPTGOSTGAMMFEEDBACK_H
