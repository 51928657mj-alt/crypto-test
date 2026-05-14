#ifndef ALGORITHMIMITO_H
#define ALGORITHMIMITO_H

#include <QTreeWidgetItem>
#include "algorithmtreeitem.h"

class AlgorithmImito : public AlgorithmTreeItem
{
public:
    AlgorithmImito(QTreeWidget *parent, CryptType MethodType, bool isShowFull = true);
    AlgorithmImito(QTreeWidgetItem *parent, CryptType MethodType, bool isShowFull = true);
private:
    void buildAlgorithmTree();
    bool isShowFull;


};

#endif // ALGORITHMIMITO_H
