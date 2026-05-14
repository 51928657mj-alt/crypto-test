#ifndef CRYPTGOST_H
#define CRYPTGOST_H

#include <QList>
#include "Types/BitTypes.h"
#include "ICrypt.h"


class CryptGost: public ICrypt {

public:
    CryptGost(QList<u::uint32>* dataIn,  QList<u::uint32> *key,  QList<QList<u::uint8> > *table);
    CryptGost(QList<u::uint32>* dataIn,  QList<u::uint32> *key,  QList<QList<u::uint8> > *table, QList<u::uint32>* iv);

    virtual ~CryptGost();

private:
    void CheckSize();
protected:
  const QList<u::uint32>* m_dataIn;
  const QList<u::uint32>* m_key;
  const QList<QList<u::uint8> >* m_table;
  const QList<u::uint32>* m_iv;
};

#endif // CRYPTGOST_H
