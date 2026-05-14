#ifndef ALGORITHMTREEITEM_H
#define ALGORITHMTREEITEM_H

#include <QTreeWidgetItem>
#include <QLabel>
#include <QList>

namespace AlgorithmInfo {
enum CryptType {Crypt, Decrypt};
}

using namespace AlgorithmInfo;

class AlgorithmTreeItem : public QTreeWidgetItem
{    
public:

    AlgorithmTreeItem(QTreeWidget *parent, CryptType MethodType, QString MethodName);
    AlgorithmTreeItem(QTreeWidgetItem *parent, CryptType MethodType, QString MethodName);
    void selectAlgoritmItem(int index, bool isCloseLast=false);
    void selectNextItem(bool isCloseLast=false);
    void selectNextTopItem(bool isCloseLast=false);
    int getSelectedItem() { return SelectedItem; }

protected:
    virtual void buildAlgorithmTree() = 0;
    QTreeWidgetItem* createCycleItem(QTreeWidgetItem *parent,QString var, int start, int finish);
    void createStepItems(QTreeWidgetItem *parent,QStringList StepItemTextLists);
    QTreeWidgetItem* createStepItem(QTreeWidgetItem *parent, QString StepItemTextLists);
    int SelectedItem;
    CryptType MethodType;
    QList <QTreeWidgetItem*> stepItems;

private:
    void visitTree(QList<QTreeWidgetItem*> &stepItems, QTreeWidgetItem* item);

};

#endif // ALGORITHMTREEITEM_H
