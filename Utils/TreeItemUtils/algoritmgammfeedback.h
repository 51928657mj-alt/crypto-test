#ifndef ALGORITMGAMMFEEDBACK_H
#define ALGORITMGAMMFEEDBACK_H

#include "algorithmtreeitem.h"

class AlgoritmGammFeedBack : public AlgorithmTreeItem
{
public:
    AlgoritmGammFeedBack(QTreeWidget *parent, CryptType MethodType, bool isShowFull = true);
    AlgoritmGammFeedBack(QTreeWidgetItem *parent, CryptType MethodType, bool isShowFull = true);

private:

    void buildAlgorithmTree();
    bool isShowFull;
};

#endif // ALGORITMGAMMFEEDBACK_H
