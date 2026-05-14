#include "algoritmgammfeedback.h"
#include "algorithmsr.h"


AlgoritmGammFeedBack::AlgoritmGammFeedBack(QTreeWidget *parent, CryptType MethodType, bool isShowFull) :
    AlgorithmTreeItem(parent,MethodType,QString("Метод гаммирования"))
{
    this->isShowFull = isShowFull;
    buildAlgorithmTree();
}

AlgoritmGammFeedBack::AlgoritmGammFeedBack(QTreeWidgetItem *parent, CryptType MethodType, bool isShowFull) :
    AlgorithmTreeItem(parent,MethodType,  QString("Метод гаммирования"))
{
    this->isShowFull = isShowFull;
    buildAlgorithmTree();
}

void AlgoritmGammFeedBack::buildAlgorithmTree() {
    /*if (MethodType == CryptType::Crypt)  createStepItem(this,"Обновление T0 новыми входными данными");
    else createStepItem(this,"Обновление T0 новыми зашифрованными данными");*/
    QTreeWidgetItem *stepItem1 = createStepItem(this,"Шифрование N1 и N2\n в режиме простой замены");
    if (this->isShowFull) new AlgorithmSR(stepItem1,MethodType);

    QStringList StepItemTextLists;
    StepItemTextLists.append("N1 суммируется по модулю 2 с 1-ыми 32\nразрядами T0. N2 суммируется по модулю 2\nсо 2-ыми 32 разрядами T0");
    StepItemTextLists.append("1-е 32 разряда CM5 в N1,\n2-ые 32 разряда CM5 в N2");
    createStepItems(this,StepItemTextLists);

    /*QTreeWidgetItem *stepItem2 = createStepItem(this,"Шифрование N1 и N2\n в режиме простой замены");
    if (this->isShowFull) new AlgorithmSR(stepItem2,MethodType);

    createStepItem(this,"k");*/


}
