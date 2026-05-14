#ifndef CRYPTGOSTSR_H
#define CRYPTGOSTSR_H

#include "CryptGost.h"

class CryptGostSR : public CryptGost {
public:
    CryptGostSR( QList<u::uint32>* dataIn,  QList<u::uint32> *key,  QList<QList<u::uint8> >* table);
    virtual ~CryptGostSR();

    void Crypt();
    void Decrypt();

    QList<CryptBaseStruct>* GetCryptStruct() const;


private:
    QList<CryptBaseStruct>* m_cryptStructList;

};

#endif // CRYPTGOSTSR_H
