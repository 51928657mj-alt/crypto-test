#ifndef ALGORITHMSR_H
#define ALGORITHMSR_H
#include "algorithmtreeitem.h"

class AlgorithmSR : public AlgorithmTreeItem
{
public:
    AlgorithmSR(QTreeWidget *parent, CryptType MethodType);
    AlgorithmSR(QTreeWidgetItem *parent, CryptType MethodType);
    void selectAlgoritmItem(int index);

private:
    void buildAlgorithmTree();
};

#endif // ALGORITHMSR_H
