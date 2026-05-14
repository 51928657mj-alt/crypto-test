#ifndef GOSTCRYPT_H
#define GOSTCRYPT_H

#include <QWidget>
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QTableWidget>
#include <QList>
#include <QResizeEvent>

#include "Types/BitTypes.h"
#include "Crypt/CryptGostSR.h"
#include "Crypt/CryptGostGamma.h"
#include "Crypt/CryptGostGammFeedBack.h"
#include "Crypt/GryptGostImito.h"
#include "ThreadsUtils/GostSleeperThread.h"

#include "Utils/TreeItemUtils/algorithmsr.h"
#include "Utils/TreeItemUtils/algorithmgamm.h"
#include "Utils/TreeItemUtils/algoritmgammfeedback.h"
#include "Utils/TreeItemUtils/algorithmimito.h"

namespace Ui {
class GostCrypt;
}

class GostCrypt : public QDialog {
    Q_OBJECT

public:
    explicit GostCrypt(QWidget *parent = 0);
    ~GostCrypt();

signals:
    void closed();

private slots:
    void OnPushButtonGostInClicked();
    void OnComboBoxCryptModeChanged();
    void OnRadioButtonEncryptClicked();
    void OnRadioButtonDecryptClicked();
    void OnPushButtonCryptViewClicked();
    void OnPushButtonClearAllClicked();
    void OnPushButtonInKeyClicked();
    void OnPushButtonInTableClicked();
    void OnPushButtonInSClicked();
    void OnPushButtonNextSRClicked();
    void OnPushButtonContinueSRClicked();
    void OnPushButtonGammNextClicked();
    void OnPushButtonGammStopClicked();
    void OnPushButtonGammFeedBackNextClicked();
    void OnPushButtonGammFeedBackStopClicked();
    void OnPushButtonImitoNextClicked();
    void OnPushButtonImitoStopClicked();
    void OnPushButtonWriteDataToFileClicked();

// слоты для создания анимации (работа с QTimer)
    void SREmitTableN1();
    void SREmitTableN2();
    void SREmitTableKey();
    void SREmitTableK();  //для работы с тетрадами
    void SREmitTableCM1(); //чтобы забить значения
    void SREmitTableItemCM1(); //для работы с тетрадами
    void SREmitTableItemK1(); //для работы с тетрадами
    void SREmitTableR();
    void SREmitTableCM2();
    void SRSetButtonEnabledorDisenabled();
    void SREmitTableK1();

    void EmitNextStepInTreeWidget();

    void IncreaseCycle();


    void GammEmitTableN1(char step, char cycle);
    void GammEmitTableN2(char step, char cycle);
    void GammEmitTableKZY(char cycle);
    void GammEmitTableK(char iter, char cycle);
    void GammEmitTableR1();
    void GammEmitItemR1(char column, char cycle);
    void GammEmitTableR2(char step, char cycle);
    void GammEmitTableCM1(char cycle);
    void GammEmitItemCM1(char column);
    void GammEmitTableCM2(char step, char cycle);
    void GammTablesStyleClear();
    void GammSetPushButtonEnabledorDisanabled();
    void GammPushButtonNextEnabledorDisanabled();

    void GammEmitTableN2();
    void GammEmitTableN1();
    void GammEmitTableN6();
    void GammEmitTableN5();
    void GammEmitTableCM4();
    void GammEmitTableCM3();
    void GammEmitTableN4();
    void GammEmitTableN3();
    void GammEmitTableCM0();
    void GammEmitTableCM5();
    void EmitOutPutWidgetRow1();
    void EmitOutPutWidgetRow2();
    void EmitContinueOrNo();

    void GammFeedBackClearTablesInThread();
    void GammFeedBackPushButtonEnabledorDisanabled();
    void GammFeedBackEmitItemR1(char column, char cycle);
    void GammFeedBackEmitItemCM1(char column);
    void GammFeedBackEmitTableN1(char step, char cycle);
    void GammFeedBackEmitTableCM1(char cycle);
    void GammFeedBackEmitTableN2(char step, char cycle);
    void GammFeedBackEmitTableKZY(char cycle);
    void GammFeedBackEmitTableK(char iter, char cycle);
    void GammFeedBackEmitTableR1();
    void GammFeedBackEmitTableR2(char step, char cycle);
    void GammFeedBackEmitTableCM2(char step, char cycle);
    void GammFeedBackEmitTableCM0();
    void GammFeedBackEmitTableCM5();
    void GammFeedBackEmitTableN1();
    void GammFeedBackEmitTableN2();
    void GammFeedBackEmitContinueOrNo();
    void GammFeedBackSetButtonEnabledorDisenabled();

    void ImitoEmitTableN1();
    void ImitoEmitTableN2();
    void ImitoEmitTableCM0();
    void ImitoEmitTableCM5();
    void ImitoEmitTableIv();
    void ImitoEmitTableCM3();
    void ImitoEmitTableCM4();
    void ImitoEmitItemR1(char column, char cycle);
    void ImitoEmitTableR1();
    void ImitoEmitItemCM1(char column);
    void ImitoEmitTableN1(char step, char cycle);
    void ImitoEmitTableCM1(char cycle);
    void ImitoEmitTableN2(char step, char cycle);
    void ImitoEmitTableKZY(char cycle);
    void ImitoEmitTableK(char iter, char cycle);
    void ImitoEmitTableR2(char step, char cycle);
    void ImitoEmitTableCM2(char step, char cycle);
    void ImitoEmitContinueOrNo();
    void ImitoPushButtonEnabledorDisanabled();
    void ImitoClearTablesInThread();
    void ImitoSetPushButtonDisanabled();

public:
    QString GetInFileName() const;
    QString GetOutFileName() const;
    QList<u::uint32> GetDataList() const;
    QList<u::uint32> GetKeyList() const;
    QList<u::uint32> GetTableList() const;

private:
    void SetS(const QString& sPath);
    void SetTable(const QString& tablePath);
    void SetKey(const QString& keyPath);
    void FillPath(QLineEdit& edit, bool save);
    void ShowEncryptionToggle(QRadioButton& encr, QRadioButton& decr, bool show);
    void PutDataToTable(u::uint32 data, QTableWidget& tableWidget, int row);
    void PutDataBinToTable(QList<u::uint32> dataList, QTableWidget& tableWidget);
    void PutDataBinToTable(u::uint32 data, QTableWidget& tableWidget, int row);
    void PutKBinToTable(QTableWidget& tableWidget);
    QString GetInFile(QLineEdit& edit);
    QString GetOutFile(QLineEdit& edit);
    void SetInFileName(QLineEdit &edit);
    void SetOutFileName(QLineEdit& edit);

    void Clear();
    void ClearData();
    void ClearModeSRStackedWidget();
    void ClearModeGammStackedWidget();
    void ClearModeGammFeedBackStackedWidget();
    void ClearModeImitoStackedWidget();

    void SetEnabledButtonView();
    void SetDataToTablesView(QTableWidget& tableWidgetN1, QTableWidget& tableWidgetN2, QTableWidget& tableWidgetKZY,
                             QTableWidget& tableWidgetK);
    void SetDataToTablesView(QTableWidget& tableWidgetN1, QTableWidget& tableWidgetN2, QTableWidget& tableWidgetKZY,
                             QTableWidget& tableWidgetK, QTableWidget& tableWidgetData);
    void SetDataToTablesView(QTableWidget &tableWidgetCM0, QTableWidget &tableWidgetKZY, QTableWidget &tableWidgetK);
    void IncreaseCount();

    void SelectListItem(int selectIndex);

    void CryptGostSimpleReplacement();
    void CryptGostGamm();
    void CryptGostGammFeedBackAlg();
    void CryptGostImitoAlg();
    //для анимации
    void SRClearTablesStyle();
    void PaintRow(QTableWidget &tableWidget, int row, QColor &color);
    void PaintItem(QTableWidget& tableWidget, int row, int column, QColor color);
    void SRCreateItems(); //создаются ячейки, для последующего удобства работы с ними
    void SRHideandShowActiveLabels(); //прячет активные стрелки

    void GammClearTablesStyle();
    void ThreadUtils();
    void GammHideandShowActiveLabels(); //прячет активные стрелки

    void GammFeedBackClearTablesStyle();
    void GammFeedBackHideandShowActiveLabels();
    void GammFeedBackThreadUtils();

    void ImitoHideandShowActiveLabels();
    void ImitoThreadUtils();
    void ImitoClearTablesStyle();

private:
    enum Mode {
        ModeSimpleReplacement,
        ModeGamm,
        ModeGammFeedback,
        ModeAuthCode
    };

private:
    Ui::GostCrypt* m_ui;
    int m_mode;
    int m_typeCrypt;
    bool m_first;
    QString m_inFileName;
    QString m_outFileName;
    QTableWidget* m_tableWidget;
    QList<u::uint32> m_dataList;
    QList<u::uint32> m_cryptDataList;
    QList<u::uint32> m_keyList;
    QList<u::uint32> m_sList;
    QList<QList<u::uint8> >m_table;
    u::uint8 m_count; //переходы
    u::uint8 m_cycle; //цикл
    u::uint8 m_dataCount; //max - m_dataList/2
    u::uint8 m_iter; //для анимации тетрад
    CryptGostSR* m_gostSRList;
    CryptGostGammFeedBack* m_gostGammFeedBackList;
    CryptGostGamma* m_gostGammList;
    GryptGostImito* m_gostImitoList;
    GostSleeperThread* m_sleeperThread;

    AlgorithmTreeItem* m_treeItem;
};

#endif // GOSTCRYPT_H
