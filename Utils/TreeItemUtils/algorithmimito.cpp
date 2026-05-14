#include "algorithmimito.h"

AlgorithmImito::AlgorithmImito(QTreeWidget *parent, CryptType MethodType, bool isShowFull) :
    AlgorithmTreeItem(parent,MethodType,QString("Метод гаммирования"))
{
    this->isShowFull = isShowFull;
    buildAlgorithmTree();
}

AlgorithmImito::AlgorithmImito(QTreeWidgetItem *parent, CryptType MethodType, bool isShowFull) :
    AlgorithmTreeItem(parent,MethodType,  QString("Метод гаммирования"))
{
    this->isShowFull = isShowFull;
    buildAlgorithmTree();
}

void AlgorithmImito::buildAlgorithmTree() {
    createStepItem(this,"Блок данных записываются в N1 и N2");
    QStringList StepItemTextLists;
    StepItemTextLists.append("Сложение ключа Xi с N1 по модулю 2^32");
    StepItemTextLists.append("Преобразование CM1 при помощи таблицы замены K");
    StepItemTextLists.append("Сдвиг на 11 шагов в сторону старших разрядов");
    StepItemTextLists.append("Сложение R2 и N2 по модулю 2");
    StepItemTextLists.append("Перемещаем N1-> N2 ");
    StepItemTextLists.append("Перемещаем CM2->N1");

    QTreeWidgetItem *cycleItem1 = createCycleItem(this,"k",0,2);
    QTreeWidgetItem *cycleItem2 = createCycleItem(cycleItem1,"i",0,7);
    createStepItems(cycleItem2,StepItemTextLists);
    createStepItem(this,"Блоки N1 и N2 записываются в CM5");
    createStepItem(this,"Новые блоки входных данных записываются\nв N1 и N2");
    createStepItem(this,"1-ые 32 бита из CM5 суммируются\nпо модулю 2 с N1. Рехультат в CM3.\n2-ые 32 бита из CM5 суммируются\nпо модулю 2 с N2. Рехультат в CM4.");
    createStepItem(this,"CM4 в N2, CM3 в N1");


}
