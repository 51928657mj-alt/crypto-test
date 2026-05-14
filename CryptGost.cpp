#include "CryptGost.h"

#include <QObject>
#include "Exc/GenericExc.h"

CryptGost::CryptGost(QList<u::uint32> *dataIn, QList<u::uint32> *key,  QList<QList<u::uint8> > *table)
    : m_dataIn(dataIn)
    , m_key(key)
    , m_table(table) {
    CheckSize();
}

CryptGost::CryptGost(QList<u::uint32>* dataIn,  QList<u::uint32> *key,  QList<QList<u::uint8> > *table, QList<u::uint32>* iv)
    : m_dataIn(dataIn)
    , m_key(key)
    , m_table(table)
    , m_iv(iv) {
    CheckSize();
    if (m_iv->size() != 2) {
        throw GenericExc(QObject::tr("Размер синхропосылки должен равняться 32 байта"));
    }
}

void CryptGost::CheckSize() {
    if (m_key->size() != 8) {
        throw GenericExc(QObject::tr("Размер ключа должен равняться 32 байта"));
    }
    if (m_table->size() != 8) {
        throw GenericExc(QObject::tr("Размер таблицы замен должен равняться 64 байта"));
    }
    for (int i = 0; i < 8; i++) {
        if (m_table->at(i).size() != 16) {
            throw GenericExc(QObject::tr("Размер таблицы замен должен равняться 64 байта"));
        }
    }
}

CryptGost::~CryptGost() {

}





