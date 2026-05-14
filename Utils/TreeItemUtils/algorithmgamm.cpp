#include "algorithmgamm.h"
#include "algorithmsr.h"



AlgorithmGamm::AlgorithmGamm(QTreeWidget *parent, CryptType MethodType, bool isShowFull) :
    AlgorithmTreeItem(parent,MethodType,QString("Метод гаммирования"))
{
    this->isShowFull = isShowFull;
    buildAlgorithmTree();
}

AlgorithmGamm::AlgorithmGamm(QTreeWidgetItem *parent, CryptType MethodType, bool isShowFull) :
    AlgorithmTreeItem(parent,MethodType,  QString("Метод гаммирования"))
{
    this->isShowFull = isShowFull;
    buildAlgorithmTree();
}

void AlgorithmGamm::buildAlgorithmTree() {
    createStepItem(this,"Первые 32 разряда из S перемещаем в N1,\n вторые 32 разряда из S перемещаем в N2");
    QTreeWidgetItem *stepItem1 = createStepItem(this,"Шифрование N1 и N2\n в режиме простой замены");
    if (this->isShowFull) new AlgorithmSR(stepItem1,MethodType);

    QStringList StepItemTextLists;
    StepItemTextLists.append("Перемещаем из N1 в N3 и из N2 в N4");
    StepItemTextLists.append("Суммирование N4 и N6 по модулю 2^32-1\n (результат в N4), суммирование N3 и N5\n по модулю 2^32-1 (результат в N5)");
    StepItemTextLists.append("Перемещаем из N3 в N1, из N4 в N2");

    createStepItems(this,StepItemTextLists);

    QTreeWidgetItem *stepItem2 = createStepItem(this,"Шифрование N1 и N2\n в режиме простой замены");
    if (this->isShowFull) new AlgorithmSR(stepItem2,MethodType);

    createStepItem(this,"Поразрядное суммирование по модулю 2\n первых 32-разрядов Tо(Tш) с N1,\n и вторых 32-разрядов To с N2");

}



