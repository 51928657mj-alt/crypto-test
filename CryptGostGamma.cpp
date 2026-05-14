#include "CryptGostGamma.h"

#include "SRUtil.h"

CryptGostGamma::CryptGostGamma( QList<u::uint32> *dataIn,  QList<u::uint32> *key,  QList<QList<u::uint8> >* table, QList<u::uint32> *iv)
    :CryptGost(dataIn, key, table, iv) {

}

u::uint32 CryptGostGamma::GetN5() const {
    return m_N5;
}

u::uint32 CryptGostGamma::GetN6() const {
    return m_N6;
}

u::uint32 CryptGostGamma::GetN1Iv() const {
    return m_N1Iv;
}

u::uint32 CryptGostGamma::GetN2Iv() const {
    return m_N2Iv;
}
QList<u::uint32> CryptGostGamma::GetN3() const {
    return m_N3;
}

QList<u::uint32> CryptGostGamma::GetN4() const {
    return m_N4;
}

QList<u::uint32> CryptGostGamma::GetDataSumN1() const {
    return m_dataSumN1;
}

QList<u::uint32> CryptGostGamma::GetDataSumN2() const {
    return m_dataSumN2;
}

QList<u::uint32> CryptGostGamma::GetCM5() const {
    return m_CM5;
}

QList<CryptBaseStruct> CryptGostGamma::GetCryptIv() const {
    return m_cryptIv;
}

QList<CryptBaseStruct>* CryptGostGamma::GetCryptStructList() const {
    return m_cryptStructList;
}

CryptGostGamma::~CryptGostGamma() {
    for (int i = 0; i < m_dataIn->size()/2; i++) {
        m_cryptStructList[i].clear();
    }
    m_cryptIv.clear();
    m_dataSumN1.clear();
    m_dataSumN2.clear();
}
void CryptGostGamma::Crypt() {
    CryptCore();
}

void CryptGostGamma::Decrypt() {
    CryptCore();
}

void CryptGostGamma::CryptCore() {
    m_N5 = 0x1010101;
    m_N6 = 0x1010104;
    u::uint32 dataN1 = m_N1Iv = m_iv->at(0);
    u::uint32 dataN2 = m_N2Iv = m_iv->at(1);
    for (int i = 1; i <= 32; i++) { //режим простой замены для имитовставки
       CryptBaseStruct cryptStruct;
       SRUtil::Crypt(cryptStruct, i, dataN1, dataN2, m_key, m_table);
       dataN1 = cryptStruct.N1;
       dataN2 = cryptStruct.N2;
       m_cryptIv.append(cryptStruct);
    }
    u::uint32 n3 = m_cryptIv.at(31).N1;
    u::uint32 n4 = m_cryptIv.at(31).N2;
    m_N3.append(m_cryptIv.at(31).N1);
    m_N4.append(m_cryptIv.at(31).N2);
    const int blocks = m_dataIn->size()/2;
    m_cryptStructList = new QList<CryptBaseStruct>[blocks];

    for (int j = 0; j < m_dataIn->size(); j++) {
        n3 = n3 + m_N5;
        u::uint32 temp = n4 + m_N6;
        if((n4 <= temp) && (m_N6 <= temp))
            n4 = temp;
        else
            n4 = temp + 1;

    m_dataSumN1.append(n3);
    m_dataSumN2.append(n4);

    dataN1 = n3;
    dataN2 = n4;

    for (int i = 1; i <= 32; i++) { //режим простой замены
       CryptBaseStruct cryptStruct;
       SRUtil::Crypt(cryptStruct, i, dataN1, dataN2, m_key, m_table);
       dataN1 = cryptStruct.N1;
       dataN2 = cryptStruct.N2;
       m_cryptStructList[j/2].append(cryptStruct);
    }
    m_N3.append(n3);
    m_N4.append(n4);
    m_CM5.append(m_cryptStructList[j/2].at(31).N1 ^ m_dataIn->at(j));
    m_CM5.append(m_cryptStructList[j/2].at(31).N2 ^ m_dataIn->at(j+1));
    j++;
  }
}
