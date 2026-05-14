#include "algorithmtreeitem.h"
#include <QTreeWidgetItemIterator>
#include <QDebug>

AlgorithmTreeItem::AlgorithmTreeItem(QTreeWidget *parent, CryptType MethodType, QString MethodName) :
    QTreeWidgetItem(parent)
{
    if (MethodType==Crypt) setText(0,MethodName+" (шифрование)");
    else setText(0,MethodName+" (расшифрование)");
    this->MethodType = MethodType;
    SelectedItem=-1;

}

AlgorithmTreeItem::AlgorithmTreeItem(QTreeWidgetItem *parent, CryptType MethodType, QString MethodName) :
    QTreeWidgetItem(parent)
{
    if (MethodType==Crypt) setText(0,MethodName+" (шифрование)");
    else setText(0,MethodName+" (расшифрование)");
    this->MethodType = MethodType;
    SelectedItem=-1;

}


QTreeWidgetItem* AlgorithmTreeItem::createCycleItem(QTreeWidgetItem *parent, QString var, int start, int finish)
{
     QTreeWidgetItem* item = new QTreeWidgetItem(parent);

     item->setText(0,QString("Цикл по %1 от %2 до %3").arg(var).arg(start).arg(finish));
     item->setIcon(0,QIcon(":/Images/asterisk.png"));
     return item;

}

void AlgorithmTreeItem::createStepItems(QTreeWidgetItem *parent,QStringList StepItemTextLists) {
    for (int i = 0; i < StepItemTextLists.count(); i++) {
        QTreeWidgetItem* item = new QTreeWidgetItem(parent);        
        item->setText(0,StepItemTextLists[i]);
        item->setIcon(0,QIcon(":/Images/point.png"));
    }

}

QTreeWidgetItem* AlgorithmTreeItem::createStepItem(QTreeWidgetItem *parent,QString StepItemTextLists) {

  QTreeWidgetItem* item = new QTreeWidgetItem(parent);
  item->setText(0,StepItemTextLists);
  item->setIcon(0,QIcon(":/Images/point.png"));
    //QFontMetrics для переноса слов
  //if (item->treeWidget()->width()> item->
  return item;

}


void AlgorithmTreeItem::selectNextItem(bool isCloseLast) {
    this->selectAlgoritmItem(SelectedItem+1,isCloseLast);
}

void AlgorithmTreeItem::selectNextTopItem(bool isCloseLast) {
    int i = SelectedItem;
    while (i<stepItems.size()) {
        if (!stepItems[i]->parent()->parent()) break;
        i++;
    }
    selectAlgoritmItem(i-1,isCloseLast);

}

void AlgorithmTreeItem::visitTree(QList<QTreeWidgetItem*> &stepItems, QTreeWidgetItem* item) {
    if (!item->childCount()) {
        stepItems << item;

    }

    if (item->text(0).contains("Цикл")) {

            QStringList substr = item->text(0).split(" ");
            int cycleSize = abs(substr[6].toInt()-substr[4].toInt())+1;




            for (int j = 0; j < cycleSize; j++){
                for (int i = 0; i < item->childCount(); ++i) {
                 visitTree(stepItems,item->child(i));
                }
            }
    }
    else {
        for (int i = 0; i < item->childCount(); ++i) {
            visitTree(stepItems,item->child(i));
        }
    }
}

void AlgorithmTreeItem::selectAlgoritmItem(int index,bool isCloseLast) {

    if (!stepItems.size()) visitTree(stepItems,this);
    if (!stepItems.size()) return;

    SelectedItem = index;

    for (int i = 0; i<stepItems.size(); i++) {

        stepItems[i]->setBackground(0,QColor(255,255,255));
    }

    if (isCloseLast) this->treeWidget()->collapseAll();

    if (SelectedItem > stepItems.size()-1) SelectedItem = 0;
    stepItems[SelectedItem]->setBackground(0,QColor(135,206,250));

    QTreeWidgetItem *expandedItem = stepItems[SelectedItem];
    while(expandedItem->parent())  {
        expandedItem->parent()->setExpanded(true);
        expandedItem = expandedItem->parent();
    }



}
