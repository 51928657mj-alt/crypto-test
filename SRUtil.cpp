#include "SRUtil.h"

#include <QObject>
#include "Exc/GenericExc.h"

SRUtil::SRUtil() {
}

SRUtil::~SRUtil() {
}

u::uint32 SRUtil::CryptCycle(CryptBaseStruct& cryptStruct, u::int8 cycle,
                                 u::uint32 _n1,  u::uint32 _n2, const QList<u::uint32> *key, const QList<QList<u::uint8> > *table) {
    QList<u::uint8> itemTable;
    u::uint32 cm1, r = 0;
    u::uint8 j, q, qt;
    cryptStruct.ActiveKey = cycle;
    cryptStruct.CM1 = _n1 + key->at(cryptStruct.ActiveKey);        // Суммируем с ключом
    cm1 = cryptStruct.CM1;
    for(j = 0; j < 8; j++) {
        q = cm1 & 0xF;
        itemTable.append(j);
        itemTable.append(q);
        cryptStruct.ActiveTableList.append(itemTable);    // Запоминаем местоположение тетрады
        itemTable.clear();
        qt = table->at(j).at(q);                       // Меняем тетрады по таблице замены
        r |= qt << (4*j);
        cm1 = cm1 >> 4;
    }
    cryptStruct.R1 = r;
    r = ((r>>21) | (r<<11));                    // Циклический сдвиг влево на 11.
    cryptStruct.R2 = r;
    r = r ^ _n2;
    cryptStruct.CM2 = r;                                  // Ксорим со второй половинкой
    return r;
}

void SRUtil::Crypt(CryptBaseStruct& cryptStruct, u::int8 cycle,
                      u::uint32 _n1, u::uint32 _n2, const QList<u::uint32> *key, const QList<QList<u::uint8> > *table) {
    switch(cycle) {
        case 1:
        case 9:
        case 17: {
            cryptStruct.N2 = _n1;
            cryptStruct.N1 = CryptCycle(cryptStruct, 0, _n1, _n2, key, table);
            break;
        }
        case 2:
        case 10:
        case 18:
        case 31: {
            cryptStruct.N2 = _n1;
            cryptStruct.N1 = CryptCycle(cryptStruct, 1, _n1, _n2, key, table);
            break;
        }
        case 3:
        case 11:
        case 19:
        case 30: {
            cryptStruct.N2 = _n1;
            cryptStruct.N1 = CryptCycle(cryptStruct, 2, _n1, _n2, key, table);
            break;
        }
        case 4:
        case 12:
        case 20:
        case 29: {
            cryptStruct.N2 = _n1;
            cryptStruct.N1 = CryptCycle(cryptStruct, 3, _n1, _n2, key, table);
            break;
        }
        case 5:
        case 13:
        case 21:
        case 28: {
            cryptStruct.N2 = _n1;
            cryptStruct.N1 = CryptCycle(cryptStruct, 4, _n1, _n2, key, table);
            break;
        }
        case 6:
        case 14:
        case 22:
        case 27: {
            cryptStruct.N2 = _n1;
            cryptStruct.N1 = CryptCycle(cryptStruct, 5, _n1, _n2, key, table);
            break;
        }
        case 7:
        case 15:
        case 23:
        case 26: {
            cryptStruct.N2 = _n1;
            cryptStruct.N1 = CryptCycle(cryptStruct, 6, _n1, _n2, key, table);
            break;
        }
        case 8:
        case 16:
        case 24:
        case 25: {
            cryptStruct.N2 = _n1;
            cryptStruct.N1 = CryptCycle(cryptStruct, 7, _n1, _n2, key, table);
            break;
        }
        case 32: {
            cryptStruct.N1 = _n1;
            cryptStruct.N2 = CryptCycle(cryptStruct, 0, _n1, _n2, key, table);
            break;
        }
        default: {
            throw GenericExc(QObject::tr("Неверный номер цикла: \"%0").arg(cycle));
         }
    }
}
