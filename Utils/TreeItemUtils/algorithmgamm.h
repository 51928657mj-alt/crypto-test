#ifndef ALGORITHMGAMM_H
#define ALGORITHMGAMM_H
#include "algorithmtreeitem.h"

class AlgorithmGamm : public AlgorithmTreeItem
{
public:
    AlgorithmGamm(QTreeWidget *parent, CryptType MethodType, bool isShowFull = true);
    AlgorithmGamm(QTreeWidgetItem *parent, CryptType MethodType, bool isShowFull = true);
private:
    void buildAlgorithmTree();
    bool isShowFull;
};

#endif // ALGORITHMGAMM_H
