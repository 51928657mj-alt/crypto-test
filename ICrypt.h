#ifndef ICRYPT_H
#define ICRYPT_H

#include "Types/BitTypes.h"
#include "CryptStruct.h"

class ICrypt {
public:
    virtual ~ICrypt() {
    }

    virtual void Crypt() = 0;
    virtual void Decrypt() = 0;
};

#endif // ICRYPT_H
