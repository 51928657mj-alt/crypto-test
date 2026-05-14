#include "HexUtils.h"

#include <QObject>
#include "Exc/GenericExc.h"

u::uint8 u::ConvertToHex(const QChar data) {
    if (data == '0') {
        return 0x0;
    } else if (data == '1') {
        return 0x1;
    } else if (data == '2') {
        return 0x2;
    } else if (data == '3') {
        return 0x3;
    } else if (data == '4') {
        return 0x4;
    } else if (data == '5') {
        return 0x5;
    } else if (data == '6') {
        return 0x6;
    } else if (data == '7') {
        return 0x7;
    } else if (data == '8') {
        return 0x8;
    } else if (data == '9') {
        return 0x9;
    } else if (data == 'a') {
        return 0xA;
    } else if (data == 'b') {
        return 0xB;
    } else if (data == 'c') {
        return 0xC;
    } else if (data == 'd') {
        return 0xD;
    } else if (data == 'e') {
        return 0xE;
    } else if (data == 'f') {
        return 0xF;
    } else {
        throw GenericExc(QObject::tr("Неизвестный аргумент").arg(data));
    }
}
void u::CreateHexDataInString(const void* data, int index, QString &hex) {
    u::uint8* ldata = (u::uint8*)data;
    unsigned char temp = ldata[index];
    hex.setNum(temp, 16);
    if (hex.length() != 2) {
        hex.insert(0, "0");
    }
}
