#include "CryptGostSR.h"

#include <QObject>
#include "Types/BitTypes.h"
#include "Exc/GenericExc.h"
#include "SRUtil.h"

CryptGostSR::CryptGostSR(QList<u::uint32> *dataIn,  QList<u::uint32> *key,  QList<QList<u::uint8> > *table)
    :CryptGost(dataIn,key, table) {

}

CryptGostSR::~CryptGostSR() {
    for (int i = 0; i < m_dataIn->size()/2; i++) {
        m_cryptStructList[i].clear();
    }
}

QList<CryptBaseStruct>* CryptGostSR::GetCryptStruct() const{
     return m_cryptStructList;
}


void CryptGostSR::Crypt() {
    const int blocks = m_dataIn->size()/2;
    m_cryptStructList = new QList<CryptBaseStruct>[blocks];
    for (int j = 0; j < m_dataIn->size(); j++) {
        u::uint32 dataN1 = m_dataIn->at(j);
        u::uint32 dataN2 = m_dataIn->at(j+1);
         for (int i = 1; i <= 32; i++) {
            CryptBaseStruct cryptStruct;
            SRUtil::Crypt(cryptStruct, i, dataN1, dataN2, m_key, m_table);
            dataN1 = cryptStruct.N1;
            dataN2 = cryptStruct.N2;
            m_cryptStructList[j/2].append(cryptStruct);
        }
       j++;
    }
}

void CryptGostSR::Decrypt() {
    const int blocks = m_dataIn->size()/2;
    m_cryptStructList = new QList<CryptBaseStruct>[blocks];
    for (int j = 0; j < m_dataIn->size(); j++) {
        u::uint32 dataN1 = m_dataIn->at(j);
        u::uint32 dataN2 = m_dataIn->at(j+1);
        for (int i = 1; i <= 32; i++) {
            CryptBaseStruct cryptStruct;
            switch(i) {
                case 1:
                case 16:
                case 24: {
                    cryptStruct.N2 = dataN1;
                    cryptStruct.N1 = SRUtil::CryptCycle(cryptStruct, 0, dataN1, dataN2, m_key, m_table);
                    break;
                }
                case 2:
                case 15:
                case 23:
                case 31: {
                    cryptStruct.N2 = dataN1;
                    cryptStruct.N1 = SRUtil::CryptCycle(cryptStruct, 1, dataN1, dataN2, m_key, m_table);
                    break;
                }
                case 3:
                case 14:
                case 22:
                case 30: {
                    cryptStruct.N2 = dataN1;
                    cryptStruct.N1 = SRUtil::CryptCycle(cryptStruct, 2, dataN1, dataN2, m_key, m_table);
                    break;
                }
                case 4:
                case 13:
                case 21:
                case 29: {
                    cryptStruct.N2 = dataN1;
                    cryptStruct.N1 = SRUtil::CryptCycle(cryptStruct, 3, dataN1, dataN2, m_key, m_table);
                    break;
                }
                case 5:
                case 12:
                case 20:
                case 28: {
                    cryptStruct.N2 = dataN1;
                    cryptStruct.N1 = SRUtil::CryptCycle(cryptStruct, 4, dataN1, dataN2, m_key, m_table);
                    break;
                }
                case 6:
                case 11:
                case 19:
                case 27: {
                    cryptStruct.N2 = dataN1;
                    cryptStruct.N1 = SRUtil::CryptCycle(cryptStruct, 5, dataN1, dataN2, m_key, m_table);
                    break;
                }
                case 7:
                case 10:
                case 18:
                case 26: {
                    cryptStruct.N2 = dataN1;
                    cryptStruct.N1 = SRUtil::CryptCycle(cryptStruct, 6, dataN1, dataN2, m_key, m_table);
                    break;
                }
                case 8:
                case 9:
                case 17:
                case 25: {
                    cryptStruct.N2 = dataN1;
                    cryptStruct.N1 = SRUtil::CryptCycle(cryptStruct, 7, dataN1, dataN2, m_key, m_table);
                    break;
                }
                case 32: {
                    cryptStruct.N1 = dataN1;
                    cryptStruct.N2 = SRUtil::CryptCycle(cryptStruct, 0, dataN1, dataN2, m_key, m_table);
                    break;
                }
                default: {
                    throw GenericExc(QObject::tr("Неверный номер цикла: \"%0").arg(i));
                }

            }
            dataN1 = cryptStruct.N1;
            dataN2 = cryptStruct.N2;
            m_cryptStructList[j/2].append(cryptStruct);
        }
        j++;
    }
}
