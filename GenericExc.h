#ifndef GENERICEXC_H
#define GENERICEXC_H

#include <QString>
#include "Types/BitTypes.h"

/// Класс общего исключения
class GenericExc {
public:
    static const u::uint32 ERR_NOT_SET; ///< Код ошибки не установлен

public:
    /// Конструктор
    /// \param[in] what - описание ошибки
    /// \param[in] err - код ошибки
    GenericExc(const QString& what, u::uint32 err = ERR_NOT_SET);

    GenericExc(const GenericExc& r);

    virtual ~GenericExc();

    GenericExc& operator=(const GenericExc& r);

public:
    /// Получить описание ошибки
    /// \return Описание ошибки
    virtual const QString& GetWhat() const {
        return what_;
    }

    /// Получить код ошибки
    /// \return Код ошибки
    virtual u::uint32 GetErr() const {
        return err_;
    }

private:
    QString what_;
    u::uint32 err_;
};


#endif // GENERICEXC_H
