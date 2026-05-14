#include "GryptGostImito.h"

#include "SRUtil.h"

GryptGostImito::GryptGostImito() {
}

GryptGostImito::~GryptGostImito() {
    for (int i = 0; i < m_blocks; i++) {
        m_cryptStructList[i].clear();
    }
}

void GryptGostImito::ComputeImito(QList<u::uint32> *dataIn, QList<u::uint32> *key, QList<QList<u::uint8> > *table) {
    u::uint32 dataN1 = 0;
    u::uint32 dataN2 = 0;
    m_blocks = dataIn->size()/2;
    m_cryptStructList = new QList<CryptBaseStruct>[m_blocks];
    for (int j = 0; j < dataIn->size(); j++) {
        m_N1.append(dataIn->at(j));
        m_N2.append(dataIn->at(j+1));
        dataN1 = dataN1^dataIn->at(j);
        dataN2 = dataN2^dataIn->at(j+1);
        m_CM3.append(dataN1);
        m_CM4.append(dataN2);
        for (int j2 = 1; j2 < 3; j2++) {
            for (int j1 = 1; j1 <= 8; j1++) {
                CryptBaseStruct cryptStruct;
                SRUtil::Crypt(cryptStruct, j1, dataN1, dataN2, key, table);
                dataN1 = cryptStruct.N1;
                dataN2 = cryptStruct.N2;
                m_cryptStructList[j/2].append(cryptStruct);
            }
        }
        j++;
        m_CM5.append(dataN1);
        m_CM5.append(dataN2);
    }
    m_imito = dataN1;
}

QList<CryptBaseStruct>* GryptGostImito::GetCryptStruct() {
    return m_cryptStructList;
}

QList<u::uint32> GryptGostImito::GetCM3() {
    return m_CM3;
}

QList<u::uint32> GryptGostImito::GetCM4() {
    return m_CM4;
}

QList<u::uint32> GryptGostImito::GetCM5() {
    return m_CM5;
}

u::uint32 GryptGostImito::GetImito() {
    return m_imito;
}

QList<u::uint32> GryptGostImito::GetN1() {
    return m_N1;
}

QList<u::uint32> GryptGostImito::GetN2() {
    return m_N2;
}
