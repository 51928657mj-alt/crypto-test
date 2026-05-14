#include "CryptGostGammFeedBack.h"

#include "SRUtil.h"

CryptGostGammFeedBack::CryptGostGammFeedBack( QList<u::uint32> *dataIn,  QList<u::uint32> *key,  QList<QList<u::uint8> >* table, QList<u::uint32> *iv)
    :CryptGost(dataIn, key, table, iv) {

}

CryptGostGammFeedBack::~CryptGostGammFeedBack() {
    for (int i = 0; i < m_dataIn->size()/2; i++) {
        m_cryptStructList[i].clear();
    }
    m_N1.clear();
    m_N2.clear();
    m_CM5.clear();
}

void CryptGostGammFeedBack::Crypt() {
    m_Encrypt = true;
    CryptOrDecryptCore();
}

void CryptGostGammFeedBack::Decrypt() {
    m_Encrypt = false;
    CryptOrDecryptCore();
}

void CryptGostGammFeedBack::CryptOrDecryptCore() {
    u::uint32 dataN1 = m_iv->at(0);
    u::uint32 dataN2 =  m_iv->at(1);
    u::uint32 tmp;
    const int blocks = m_dataIn->size()/2;
    m_cryptStructList = new QList<CryptBaseStruct>[blocks];
    for (int j = 0; j < m_dataIn->size(); j++) {
        for (int i = 1; i <= 32; i++) {
           CryptBaseStruct cryptStruct;
           SRUtil::Crypt(cryptStruct, i, dataN1, dataN2, m_key, m_table);
           dataN1 = cryptStruct.N1;
           dataN2 = cryptStruct.N2;
           m_cryptStructList[j/2].append(cryptStruct);
        }

        tmp = m_dataIn->at(j) ^ dataN1;
        m_CM5.append(tmp);
        if (m_Encrypt) { // Зашифрование.
                dataN1 = tmp;
            }
         else { // Расшифрование.
                dataN1 = m_dataIn->at(j);
            }
        tmp = m_dataIn->at(j+1) ^ dataN2;
        m_CM5.append(tmp);
            if (m_Encrypt) {
                dataN2 = tmp;
            }
            else {
                dataN2 = m_dataIn->at(j+1);
            }
            j++;
            m_N1.append(dataN1);
            m_N2.append(dataN2);
        }
}


QList<CryptBaseStruct>* CryptGostGammFeedBack::GetCryptStructList() const {
    return m_cryptStructList;
}

QList<u::uint32> CryptGostGammFeedBack::GetCM5() const {
    return m_CM5;
}

QList<u::uint32> CryptGostGammFeedBack::GetN1() const {
    return m_N1;
}

QList<u::uint32> CryptGostGammFeedBack::GetN2() const {
    return m_N2;
}
