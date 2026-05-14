#include "algorithmsr.h"

AlgorithmSR::AlgorithmSR(QTreeWidget *parent, CryptType MethodType) :
    AlgorithmTreeItem(parent,MethodType,QString("Метод простой замены"))
{
    buildAlgorithmTree();

}

AlgorithmSR::AlgorithmSR(QTreeWidgetItem *parent, CryptType MethodType) :
    AlgorithmTreeItem(parent,MethodType,QString("Метод простой замены"))
{
    buildAlgorithmTree();

}

void AlgorithmSR::buildAlgorithmTree() {
    QStringList StepItemTextLists;
    StepItemTextLists.append("Сложение ключа Xi с N1 по модулю\n2^32");
    StepItemTextLists.append("Преобразование CM1 при помощи таблицы\nзамены K");
    StepItemTextLists.append("Сдвиг на 11 шагов в сторону старших\nразрядов");
    StepItemTextLists.append("Сложение R2 и N2 по модулю 2");
    StepItemTextLists.append("Перемещаем N1-> N2 ");
    StepItemTextLists.append("Перемещаем CM2->N1");

    if (this->MethodType == Crypt) {
        QTreeWidgetItem *cycleItem1 = createCycleItem(this,"k",0,2);
        QTreeWidgetItem *cycleItem2 = createCycleItem(cycleItem1,"i",0,7);
        createStepItems(cycleItem2,StepItemTextLists);
        QTreeWidgetItem *cycleItem3 = createCycleItem(this,"i",7,0);
        createStepItems(cycleItem3,StepItemTextLists);
    }
    else {

        QTreeWidgetItem *cycleItem1 = createCycleItem(this,"i",0,7);
        createStepItems(cycleItem1,StepItemTextLists);
        QTreeWidgetItem *cycleItem2 = createCycleItem(this,"k",0,2);
        QTreeWidgetItem *cycleItem3 = createCycleItem(cycleItem2,"i",7,0);
        createStepItems(cycleItem3,StepItemTextLists);

    }
}



