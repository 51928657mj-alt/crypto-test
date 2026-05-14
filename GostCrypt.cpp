#include "GostCrypt.h"
#include "ui_GostCrypt.h"

#include <QFileDialog>
#include <QStringList>
#include <QMessageBox>
#include <QObject>
#include <QBitArray>
#include <QDataStream>
#include <QTimer>
#include <bitset>
#include <QApplication>
#include "windows.h"
#include "Utils/FilePath.h"
#include "Utils/FileSize.h"
#include "Utils/HexUtils.h"
#include "Exc/GenericExc.h"
#include <QDebug>


const int SIZE_BLOCK = 4; //Размер считываемого блока
const int KEY_SIZE = 32; //Размер ключа шифрования
const int TABLE_SIZE = 64; //Размер блока подстановки
const int S_SIZE = 8; // Размер синхропосылки

// Цвета
QColor blue(135, 206, 250);
QColor white(255, 255, 255);

GostCrypt::GostCrypt(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::GostCrypt) {
    m_ui->setupUi(this);

    //setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
    connect(m_ui->pushButtonGostIn, SIGNAL(clicked()), this, SLOT (OnPushButtonGostInClicked()));
    //connect(m_ui->pushButtonGostOut, SIGNAL(clicked()), this, SLOT (OnPushButtonGostOutClicked()));
    connect(m_ui->pushButtonCryptView, SIGNAL (clicked()), this, SLOT(OnPushButtonCryptViewClicked()));
    connect(m_ui->pushButtonInKey, SIGNAL (clicked()), this, SLOT(OnPushButtonInKeyClicked()));
    connect(m_ui->pushButtonInTable, SIGNAL (clicked()), this, SLOT(OnPushButtonInTableClicked()));
    connect(m_ui->pushButtonInS, SIGNAL (clicked()), this, SLOT(OnPushButtonInSClicked()));
    connect(m_ui->PushButtonSRNext, SIGNAL (clicked()), this, SLOT(OnPushButtonNextSRClicked()));
    connect(m_ui->pushButtonSRcontinue, SIGNAL (clicked()), this, SLOT(OnPushButtonContinueSRClicked()));
    connect(m_ui->radioButtonEncrypt, SIGNAL (clicked()), this, SLOT(OnRadioButtonEncryptClicked()));
    connect(m_ui->radioButtonDecrypt, SIGNAL (clicked()), this, SLOT(OnRadioButtonDecryptClicked()));
    connect(m_ui->pushButtonGammNext, SIGNAL(clicked()), this, SLOT(OnPushButtonGammNextClicked()));
    connect(m_ui->pushButtonGammStop, SIGNAL(clicked()), this, SLOT(OnPushButtonGammStopClicked()));
    connect(m_ui->pushButtonGammFeedBackNext, SIGNAL(clicked()), this, SLOT(OnPushButtonGammFeedBackNextClicked()));
    connect(m_ui->pushButtonGammFeedBackStop, SIGNAL(clicked()), this, SLOT(OnPushButtonGammFeedBackStopClicked()));
    connect(m_ui->pushButton_ImitoNext, SIGNAL(clicked()), this, SLOT(OnPushButtonImitoNextClicked()));
    connect(m_ui->pushButton_ImitoStop, SIGNAL(clicked()), this, SLOT(OnPushButtonImitoStopClicked()));
    connect(m_ui->pushButtonClearAll, SIGNAL(clicked()), this, SLOT(OnPushButtonClearAllClicked()));
    connect(m_ui->pushButtonWriteDatatoFile, SIGNAL(clicked()), this, SLOT(OnPushButtonWriteDataToFileClicked()));
    QStringList typesMode;
    typesMode.clear();

    typesMode.append(tr("Режим простой замены"));
    typesMode.append(tr("Режим гаммирования"));
    typesMode.append(tr("Режим гаммирования с обратной связью"));
    typesMode.append(tr("Режим вычисления имитовставки"));

    m_ui->comboBoxCryptMode->blockSignals(true);
    for (int i = 0; i < typesMode.count(); i++) {
        m_ui->comboBoxCryptMode->addItem(typesMode.at(i), ModeSimpleReplacement+i);
    }
    m_ui->comboBoxCryptMode->setCurrentIndex(0);
    m_ui->comboBoxCryptMode->blockSignals(false);

    connect(m_ui->comboBoxCryptMode, SIGNAL(activated(int)), this, SLOT(OnComboBoxCryptModeChanged()));

    m_count = 0; //переходы
    m_cycle = 1; //цикл
    m_dataCount = 0; //max - m_dataList/2
    m_typeCrypt = u::Encrypt;
    m_mode = ModeSimpleReplacement;
    m_ui->label_Hide->hide();
    m_ui->label_HideButton->hide();
    m_ui->GostCryptStackedWidgetView->setCurrentWidget(m_ui->ModeSRStackedWidget);
    SRHideandShowActiveLabels();
    m_sleeperThread = NULL;
    QString sPath = QApplication::applicationDirPath().append("/keysPath/sGost.bin");
    QString tablePath = QApplication::applicationDirPath().append("/keysPath/tableGost.bin");
    QString keyPath = QApplication::applicationDirPath().append("/keysPath/keyGost.bin");
    SetS(sPath);
    SetKey(keyPath);
    SetTable(tablePath);

    this->setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);
    m_ui->pushButtonWriteDatatoFile->setEnabled(false);
   /* m_ui->listWidget_Gost->setStyleSheet("QListWidge::item:selected {"
                                         "background-color: rgb{135, 206, 250};"
                                         "}");*/
}

GostCrypt::~GostCrypt() {
    ClearData();
    delete m_ui;
}

void GostCrypt::OnRadioButtonEncryptClicked() {
    Clear();
    m_typeCrypt = u::Encrypt;
    SetEnabledButtonView();
}

void GostCrypt::OnRadioButtonDecryptClicked() {
    Clear();
    m_typeCrypt = u::Decrypt;
    SetEnabledButtonView();
}

void GostCrypt::OnPushButtonClearAllClicked() {
    m_ui->lineEditGostIn->clear();
    //m_ui->lineEditGostOut->clear();
    m_ui->lineEditInKey->clear();
    m_ui->lineEditInS->clear();
    m_ui->lineEditInTable->clear();
    m_ui->tableWidgetInputData->clear();
    m_ui->tableWidgetS->clear();
    m_ui->tableWidgetOutputData->clear();
    m_ui->tableWidgetInTable->clear();
    m_ui->tableWidgetKey->clear();
    Clear();
    ClearData();
    m_ui->comboBoxCryptMode->setCurrentIndex(0);
    m_typeCrypt = u::Encrypt;
    m_mode = ModeSimpleReplacement;
    m_ui->GostCryptStackedWidgetView->setCurrentWidget(m_ui->ModeSRStackedWidget);
    m_inFileName.clear();
    m_outFileName.clear();
}

void GostCrypt::Clear() {
    m_ui->GostCryptStackedWidgetView->setEnabled(false);
    ClearModeSRStackedWidget();
    ClearModeGammStackedWidget();
    ClearModeGammFeedBackStackedWidget();
    ClearModeImitoStackedWidget();
    m_ui->tableWidgeK->clear();
    m_ui->tableWidgetKZY->clear();
    m_count = 0;
    m_cycle = 1;
    m_dataCount = 0;
    m_ui->pushButtonCryptView->setEnabled(false);
    m_ui->tableWidgetOutputData->clear();
    if (m_sleeperThread != NULL && !m_sleeperThread->isFinished()) {
        m_sleeperThread->terminate();
        delete m_sleeperThread;
        m_sleeperThread = NULL;
    }
    m_ui->label_Hide->hide();
    m_ui->pushButtonWriteDatatoFile->setEnabled(false);
}

void GostCrypt::ClearData() {
    m_dataList.clear();
    m_cryptDataList.clear();
    m_keyList.clear();
    m_sList.clear();
    m_table.clear();
}

void GostCrypt::ClearModeGammFeedBackStackedWidget() {
    m_ui->tableWidgetGammFeedBackCM0->clear();
    m_ui->tableWidgetGammFeedBackCM1->clear();
    m_ui->tableWidgetGammFeedBackCM2->clear();
    m_ui->tableWidgetGammFeedBackCM5->clear();
    m_ui->tableWidgetGammFeedBackK->clear();
    m_ui->tableWidgetGammFeedBackKZY->clear();
    m_ui->tableWidgetGammFeedBackN1->clear();
    m_ui->tableWidgetGammFeedBackN2->clear();
    m_ui->tableWidgetGammFeedBackR1->clear();
    m_ui->tableWidgetGammFeedBackR2->clear();
}

void GostCrypt::ClearModeGammStackedWidget() {
    m_ui->tableWidgetGammN1->clear();
    m_ui->tableWidgetGammN2->clear();
    m_ui->tableWidgetGammCM1->clear();
    m_ui->tableWidgetGammCM2->clear();
    m_ui->tableWidgetGammR1->clear();
    m_ui->tableWidgetGammR2->clear();
    m_ui->tableWidgetGammKZY->clear();
    m_ui->tableWidgetGammK->clear();
    m_ui->tableWidgetGammCM3->clear();
    m_ui->tableWidgetGammCM4->clear();
    m_ui->tableWidgetGammN3->clear();
    m_ui->tableWidgetGammN4->clear();
    m_ui->tableWidgetGammN6->clear();
    m_ui->tableWidgetGammN5->clear();
    m_ui->tableWidgetGammCM0->clear();
    m_ui->tableWidgetGammCM5->clear();
}

void GostCrypt::ClearModeSRStackedWidget() {
    m_ui->tableWidgetCM1->clear();
    m_ui->tableWidgetCM2->clear();
    m_ui->tableWidgetN1->clear();
    m_ui->tableWidgetN2->clear();
    m_ui->tableWidgetK1->clear();
    m_ui->tableWidgetR->clear();

}
void GostCrypt::ClearModeImitoStackedWidget() {
    m_ui->tableWidgetImitoCM0->clear();
    m_ui->tableWidgetImitoCM5->clear();
    m_ui->tableWidgetImitoCM3->clear();
    m_ui->tableWidgetImitoCM4->clear();
    m_ui->tableWidgetImitoN1->clear();
    m_ui->tableWidgetImitoN2->clear();
    m_ui->tableWidgetImitoCM1->clear();
    m_ui->tableWidgetImitoCM2->clear();
    m_ui->tableWidgetImitoK->clear();
    m_ui->tableWidgetImitoKZY->clear();
    m_ui->tableWidgetImitoR1->clear();
    m_ui->tableWidgetImitoR2->clear();
    m_ui->tableWidgetImitoIv->clear();
}

void GostCrypt::SetEnabledButtonView() {
    switch (m_mode) {
        case (ModeSimpleReplacement):
        case (ModeAuthCode):{
            if (!m_dataList.isEmpty() && !m_keyList.isEmpty() && !m_table.isEmpty()) {
                    m_ui->pushButtonCryptView->setEnabled(true);
            }
            break;
        }
        case (ModeGamm):
        case (ModeGammFeedback): {
            if (!m_dataList.isEmpty() && !m_keyList.isEmpty() && !m_table.isEmpty() && !m_sList.isEmpty()) {
                m_ui->pushButtonCryptView->setEnabled(true);
            }
            break;
        }
        default: {
            throw GenericExc(QObject::tr("Неизвестный тип шифрования"));
        }
    }
}

void GostCrypt::OnPushButtonWriteDataToFileClicked() {
    QString err;
    try {
        if (m_cryptDataList.isEmpty()) {
            throw GenericExc(QObject::tr("Зашифрованные данные отсутствуют"));
        }
        QFileInfo fileInfo(m_inFileName);
        QDir dir = fileInfo.absoluteDir();
        QString fileOut1 = dir.absolutePath().append("/crypt_").append(fileInfo.fileName());
        QString filePath = QFileDialog::getSaveFileName(this,
               tr("Сохранить зашифрованные данные"), fileOut1, tr("Бинарный файл СЧП (*.bin)"));
        if (filePath.isEmpty()) {
            return;
        }
        QFile file(filePath);
        int n = QMessageBox::warning(this, "Предупреждение", "Выходной файл будет очищен" "\nПродолжить?",
                                         "Да", "Отмена", QString(), 0, 1); //Нажата кнопка Yes
        if (n) {
            return;
        }
        if (!file.open(QIODevice::WriteOnly)) {
            throw GenericExc(QObject::tr("Ошибка открытия выходного файла %0").arg(m_outFileName));
        } else {
            for (int i = 0; i < m_cryptDataList.size(); i++) {
                QDataStream out(&file);
                out.setByteOrder(QDataStream::LittleEndian);
                out << (quint32)m_cryptDataList.at(i);
            }
            file.close();
            QMessageBox::information(this, tr("Запись в файл"), tr("Запись зашифрованных данных в файл \n%0 произведена успешно").arg(m_outFileName));
        }
     } catch (const GenericExc& exc) {
        err = exc.GetWhat();
    } catch (...) {
        err = tr("Неизвестная ошибка");
    }
    if (!err.isEmpty()) {
        QMessageBox::critical(this, tr("Ошибка"), tr("Ошибка записи в файл данных: %0").arg(err));
    }
}

void GostCrypt::OnPushButtonGostInClicked() {
    //m_dataList.clear();
    //m_ui->tableWidgetInputData->clear();
    Clear();
    FillPath(*m_ui->lineEditGostIn, false);
    SetInFileName(*m_ui->lineEditGostIn);    
    QFile file(m_inFileName);
    QString err;
    try {
        // Проверяем указан ли путь к выходному файлу
        if (m_inFileName.isEmpty()) {
            m_ui->lineEditGostIn->clear();
            m_ui->lineEditGostIn->setFocus();
            throw GenericExc(QObject::tr("Входной файл не задан"));
        } else if (u::IsFileExists(m_inFileName) != u::yes) {
            // Проверяем существование выходного файла
            m_ui->lineEditGostIn->clear();
            m_ui->lineEditGostIn->setFocus();
            throw GenericExc(QObject::tr("Файл не существует").arg(m_inFileName));;
        } else  if (!file.open(QIODevice::ReadOnly)) {
            m_ui->lineEditGostIn->clear();
            m_ui->lineEditGostIn->setFocus();
            throw GenericExc(QObject::tr("Ошибка открытия входного файла %0").arg(m_inFileName));
        } else if ((m_mode == ModeSimpleReplacement) && (file.size() % (2*SIZE_BLOCK) != 0)) {
            m_ui->lineEditGostIn->clear();
            m_ui->lineEditGostIn->setFocus();
            throw GenericExc(QObject::tr("Размер файла %0 должен быть кратен 8 байтам").arg(m_inFileName));
        } else if (file.size() > 64) {
            m_ui->lineEditGostIn->clear();
            m_ui->lineEditGostIn->setFocus();
            throw GenericExc(QObject::tr("Размер файла %0 должен быть не более 64 байт").arg(m_inFileName));
        } else if (u::IsSamePath(m_inFileName, m_outFileName) == u::yes && m_outFileName.compare("") != 0) {
            m_ui->lineEditGostIn->clear();
            m_ui->lineEditGostIn->setFocus();
            throw GenericExc(QObject::tr("Входной файл совпадает с выходным файлом").arg(m_inFileName));
        }
        if (!m_dataList.isEmpty()) {
            m_dataList.clear();
        }        
        double fblocks = (double)file.size()/(SIZE_BLOCK);
        u::uint32 temp;
        for (int j = 0; j < fblocks; j++) {
            temp = 0;
            if (file.read(reinterpret_cast<char*>(&temp), SIZE_BLOCK) <= 0) {
                file.close();
                if (!file.atEnd()) throw GenericExc(QObject::tr("Ошибка чтения файла %0").arg(m_inFileName));
            }
            m_dataList.append(temp);
            PutDataToTable(m_dataList.at(j), *m_ui->tableWidgetInputData, j);
        }
        float f = file.size() /(2*SIZE_BLOCK);
        if ((file.size() % (2*SIZE_BLOCK) != 0) && (f-(int)f<0.5)) {
            m_dataList.append(0);
            f = 0;
        }
    } catch (const GenericExc& exc) {
        err = exc.GetWhat();
    } catch (...) {
        err = tr("Неизвестная ошибка");
    }
    if (!err.isEmpty()) {
        QMessageBox::critical(this, tr("Ошибка"), tr("Ошибка чтения файла входных данных: %0").arg(err));
    }
    SetEnabledButtonView();
}

void GostCrypt::FillPath(QLineEdit& edit, bool save) {
    QString path = save ? QFileDialog::getSaveFileName(this, tr("Выбор файла для сохранения"), edit.text(), tr("Все файлы (*.*);;Бинарный файл (*.bin)")) : QFileDialog::getOpenFileName(this, tr("Выбор файла для открытия"), edit.text(), tr("Бинарный файл (*.bin)"));
    if (!path.isEmpty()) {
        edit.setText(path);
    }
}

void GostCrypt::OnComboBoxCryptModeChanged() {
    Clear();
    m_mode = m_ui->comboBoxCryptMode->currentIndex();
    if (m_mode == ModeAuthCode) {
        ShowEncryptionToggle(*m_ui->radioButtonEncrypt, *m_ui->radioButtonDecrypt, false);
    } else {
        ShowEncryptionToggle(*m_ui->radioButtonEncrypt, *m_ui->radioButtonDecrypt, true);
    }
    if (m_mode == ModeSimpleReplacement || m_mode == ModeAuthCode) {
        m_ui->labelS->setEnabled(false);
        m_ui->tableWidgetS->setEnabled(false);
        m_ui->labelSIn->setEnabled(false);
        m_ui->lineEditInS->setEnabled(false);
        m_ui->pushButtonInS->setEnabled(false);
        if (m_mode == ModeSimpleReplacement) {
            m_ui->GostCryptStackedWidgetView->setCurrentWidget(m_ui->ModeSRStackedWidget);
        } else {
            m_ui->GostCryptStackedWidgetView->setCurrentWidget(m_ui->ModeImitoStackedWidget);
        }
    } else {
        m_ui->labelS->setEnabled(true);
        m_ui->tableWidgetS->setEnabled(true);
        m_ui->labelSIn->setEnabled(true);
        m_ui->lineEditInS->setEnabled(true);
        m_ui->pushButtonInS->setEnabled(true);
        if (m_mode == ModeGamm) {
            m_ui->GostCryptStackedWidgetView->setCurrentWidget(m_ui->ModeGammStackedWidget);
        } else {
            m_ui->GostCryptStackedWidgetView->setCurrentWidget(m_ui->ModeGammFeedbackStackedwidget);
        }
    }
    SetEnabledButtonView();
}

void GostCrypt::ShowEncryptionToggle(QRadioButton &encr, QRadioButton &decr, bool show) {
    if (show) {
        encr.setEnabled(true);
        decr.setEnabled(true);
    }
    else {
        encr.setChecked(true);

        encr.setEnabled(false);
        decr.setEnabled(false);
    }
}
void GostCrypt::SetS(const QString& sPath) {
    QFile file(sPath);
    QString err;
    try {
        if (sPath.isEmpty()) {
            m_ui->lineEditInS->clear();
            m_ui->lineEditInS->setFocus();
            return;
            //throw GenericExc(QObject::tr("Файл синхропосылки не задан"));
        } else if (!file.open(QIODevice::ReadOnly)) {
            m_ui->lineEditInS->clear();
            m_ui->lineEditInS->setFocus();
            return;
            //throw GenericExc(QObject::tr("Ошибка открытия файла ключа шифрования %0").arg(sPath));
        } else if (file.size() != (S_SIZE)) {
            m_ui->lineEditInS->clear();
            m_ui->lineEditInS->setFocus();
            throw GenericExc(QObject::tr("Размер файла %0 должен быть равен 8 байтам").arg(sPath));
        }
        int blocks = S_SIZE/SIZE_BLOCK;
        u::uint32 temp;
        if (!m_sList.isEmpty()) {
            m_sList.clear();
        }
        for (int j = 0; j < blocks; j++) {
            if (file.read(reinterpret_cast<char*>(&temp), SIZE_BLOCK) <= 0) {
                file.close();
                throw GenericExc(QObject::tr("Ошибка чтения файла синхропосылки %0").arg(sPath));
            }
            m_sList.append(temp);
            PutDataToTable(m_sList.at(j), *m_ui->tableWidgetS, j);
        }
        file.close();
        m_ui->lineEditInS->setText(sPath);
    } catch (const GenericExc& exc) {
        err = exc.GetWhat();
    } catch (...) {
        err = tr("Неизвестная ошибка");
    }
    if (!err.isEmpty()) {
        QMessageBox::critical(this, tr("Ошибка"), tr("Ошибка чтения файла файла синхропосылки: %0").arg(err));
    }
}

void GostCrypt::SetTable(const QString& tablePath) {
    QFile file(tablePath);
    QString err;
    try {
        if (tablePath.isEmpty()) {
            m_ui->lineEditInTable->clear();
            m_ui->lineEditInTable->setFocus();
            return;
            //throw GenericExc(QObject::tr("Таблица замен не задана"));
        } else if (!file.open(QIODevice::ReadOnly)) {
            m_ui->lineEditInTable->clear();
            m_ui->lineEditInTable->setFocus();
            return;
            //throw GenericExc(QObject::tr("Ошибка открытия файла таблицы замен %0").arg(tablePath));
        } else if (file.size() != TABLE_SIZE) {
            m_ui->lineEditInTable->clear();
            m_ui->lineEditInTable->setFocus();
            throw GenericExc(QObject::tr("Размер файла таблицы замен %0 должен быть равен 64 байта").arg(tablePath));
        }
        int blocks = TABLE_SIZE/(2*SIZE_BLOCK);
        u::uint64 temp, columns = 0;
        if (!m_table.isEmpty()) {
            m_table.clear();
        }
        for (int j = 0; j < blocks; j++) {
                if (file.read(reinterpret_cast<char*>(&temp), SIZE_BLOCK*2) <= 0) {
                    file.close();
                    throw GenericExc(QObject::tr("Ошибка чтения файла таблицы замен %0").arg(tablePath));
                }
                QString hexadecimal;
                hexadecimal.setNum(temp, 16);
                QList<u::uint8> temp;
                for (int i = 0; i < 16; i++) {
                    QTableWidgetItem *newItem = new QTableWidgetItem(hexadecimal.at(15-i).toUpper());
                    m_ui->tableWidgetInTable->setItem(i, 7-columns, newItem);
                    temp.append(u::ConvertToHex(hexadecimal.at(15-i)));
                }
                m_table.append(temp);
                columns++;
        }
    file.close();
    m_ui->lineEditInTable->setText(tablePath);
    } catch (const GenericExc& exc) {
        err = exc.GetWhat();
    } catch (...) {
        err = tr("Неизвестная ошибка");
    }
    if (!err.isEmpty()) {
        QMessageBox::critical(this, tr("Ошибка"), tr("Ошибка чтения файла таблицы замен: %0").arg(err));
    }
}

void GostCrypt::SetKey(const QString& keyPath) {
    QFile file(keyPath);
    QString err;
    try {
        if (keyPath.isEmpty()) {
            m_ui->lineEditInKey->clear();
            m_ui->lineEditInKey->setFocus();
            return;
            //throw GenericExc(QObject::tr("Ключ шифрования не задан"));
        } else if (!file.open(QIODevice::ReadOnly)) {
            m_ui->lineEditInKey->clear();
            m_ui->lineEditInKey->setFocus();
            return;
            //throw GenericExc(QObject::tr("Ошибка открытия файла ключа шифрования %0").arg(keyPath));
        } else if (file.size() != (KEY_SIZE)) {
            m_ui->lineEditInKey->clear();
            m_ui->lineEditInKey->setFocus();
            throw GenericExc(QObject::tr("Размер файла %0 должен быть равен 32 байтам").arg(keyPath));
        }
        int blocks = KEY_SIZE/SIZE_BLOCK;
        u::uint32 temp;
        if (!m_keyList.isEmpty()) {
            m_keyList.clear();
        }
        for (int j = 0; j < blocks; j++) {
            if (file.read(reinterpret_cast<char*>(&temp), SIZE_BLOCK) <= 0) {
                file.close();
                throw GenericExc(QObject::tr("Ошибка чтения файла ключа шифрования %0").arg(keyPath));
            }
            m_keyList.append(temp);
            PutDataToTable(m_keyList.at(j), *m_ui->tableWidgetKey, j);
        }
        file.close();
        m_ui->lineEditInKey->setText(keyPath);
    } catch (const GenericExc& exc) {
        err = exc.GetWhat();
    } catch (...) {
        err = tr("Неизвестная ошибка");
    }
    if (!err.isEmpty()) {
        QMessageBox::critical(this, tr("Ошибка"), tr("Ошибка чтения файла ключа шифрования: %0").arg(err));
    }
}

void GostCrypt::OnPushButtonInSClicked() {
    Clear();
    FillPath(*m_ui->lineEditInS, false);
    QString sPath = m_ui->lineEditInS->text();
    QFile file(sPath);
    QString err;
    try {
        if (sPath.isEmpty()) {
            m_ui->lineEditInS->clear();
            m_ui->lineEditInS->setFocus();
            throw GenericExc(QObject::tr("Файл синхропосылки не задан"));
        } else if (!file.open(QIODevice::ReadOnly)) {
            m_ui->lineEditInS->clear();
            m_ui->lineEditInS->setFocus();
            throw GenericExc(QObject::tr("Ошибка открытия файла ключа шифрования %0").arg(sPath));
        } else if (file.size() != (S_SIZE)) {
            m_ui->lineEditInS->clear();
            m_ui->lineEditInS->setFocus();
            throw GenericExc(QObject::tr("Размер файла %0 должен быть равен 8 байтам").arg(sPath));
        }
        int blocks = S_SIZE/SIZE_BLOCK;
        u::uint32 temp;
        if (!m_sList.isEmpty()) {
            m_sList.clear();
        }
        for (int j = 0; j < blocks; j++) {
            if (file.read(reinterpret_cast<char*>(&temp), SIZE_BLOCK) <= 0) {
                file.close();
                throw GenericExc(QObject::tr("Ошибка чтения файла синхропосылки %0").arg(sPath));
            }
            m_sList.append(temp);
            PutDataToTable(m_sList.at(j), *m_ui->tableWidgetS, j);
        }
        file.close();
    } catch (const GenericExc& exc) {
        err = exc.GetWhat();
    } catch (...) {
        err = tr("Неизвестная ошибка");
    }
    if (!err.isEmpty()) {
        QMessageBox::critical(this, tr("Ошибка"), tr("Ошибка чтения файла файла синхропосылки: %0").arg(err));
    }
    SetEnabledButtonView();
}

void GostCrypt::OnPushButtonCryptViewClicked() {
    QString fileNameIcon = ":/Images/asterisk.png";
    QString fileNameBaseIcon = ":/Images/point.png";
    m_ui->tableWidgetOutputData->clear();
    //m_ui->treeWidget_Gost->clear
    m_dataCount = 0;
    m_count = 0;

    switch (m_mode) {
        case (ModeSimpleReplacement): {        
            m_ui->GostCryptStackedWidgetView->setEnabled(true);
            m_ui->ModeSRStackedWidget->setEnabled(true);
            SetDataToTablesView(*m_ui->tableWidgetN1, *m_ui->tableWidgetN2, *m_ui->tableWidgetKZY, *m_ui->tableWidgeK);
            m_ui->PushButtonSRNext->setEnabled(true);
            m_ui->pushButtonSRcontinue->setEnabled(true);
            CryptGostSimpleReplacement();
            m_ui->pushButtonCryptView->setEnabled(false);
            m_ui->treeWidget_Gost->clear();
            m_treeItem = new AlgorithmSR(m_ui->treeWidget_Gost,(AlgorithmInfo::CryptType)this->m_typeCrypt);
            m_ui->treeWidget_Gost->expandAll();
            break;
           }
        case (ModeGamm): {
            m_ui->GostCryptStackedWidgetView->setEnabled(true);
            m_ui->ModeGammStackedWidget->setEnabled(true);
            GammHideandShowActiveLabels();
            m_ui->tableWidgetGammKZY->setEnabled(true);
            SetDataToTablesView(*m_ui->tableWidgetGammN1, *m_ui->tableWidgetGammN2, *m_ui->tableWidgetGammKZY,
                                *m_ui->tableWidgetGammK, *m_ui->tableWidgetGammCM0);
            m_ui->pushButtonGammNext->setEnabled(true);
            m_ui->pushButtonGammStop->setEnabled(false);
            CryptGostGamm();
            m_ui->pushButtonCryptView->setEnabled(false);
            PutDataBinToTable(m_gostGammList->GetN5(), *m_ui->tableWidgetGammN5, 0);
            PutDataBinToTable(m_gostGammList->GetN6(), *m_ui->tableWidgetGammN6, 0);
            m_ui->treeWidget_Gost->clear();
            m_treeItem = new AlgorithmGamm(m_ui->treeWidget_Gost,(AlgorithmInfo::CryptType)this->m_typeCrypt,true);
            m_ui->treeWidget_Gost->expandAll();
            //m_ui->treeWidget_Gost->setWordWrap(true);
            break;
        }
        case (ModeGammFeedback): {
            m_ui->GostCryptStackedWidgetView->setEnabled(true);
            m_ui->ModeGammFeedbackStackedwidget->setEnabled(true);
            GammFeedBackHideandShowActiveLabels();
            SetDataToTablesView(*m_ui->tableWidgetGammFeedBackN1, *m_ui->tableWidgetGammFeedBackN2, *m_ui->tableWidgetGammFeedBackKZY,
                                *m_ui->tableWidgetGammFeedBackK, *m_ui->tableWidgetGammFeedBackCM0);
            m_ui->pushButtonGammFeedBackNext->setEnabled(true);
            m_ui->pushButtonGammFeedBackStop->setEnabled(false);
            CryptGostGammFeedBackAlg();
            m_ui->pushButtonCryptView->setEnabled(false);
            m_ui->treeWidget_Gost->clear();
            m_treeItem = new AlgoritmGammFeedBack(m_ui->treeWidget_Gost,(AlgorithmInfo::CryptType)this->m_typeCrypt,true);
            m_ui->treeWidget_Gost->expandAll();
            break;
        }
        case (ModeAuthCode): {
            m_ui->GostCryptStackedWidgetView->setEnabled(true);
            m_ui->ModeImitoStackedWidget->setEnabled(true);
            ImitoHideandShowActiveLabels();
            SetDataToTablesView(*m_ui->tableWidgetImitoCM0, *m_ui->tableWidgetImitoKZY, *m_ui->tableWidgetImitoK);
            m_ui->pushButton_ImitoNext->setEnabled(true);
            m_ui->pushButton_ImitoStop->setEnabled(false);
            CryptGostImitoAlg();
            m_first = true; //для имитовставки
            m_ui->pushButtonCryptView->setEnabled(false);

            m_ui->treeWidget_Gost->clear();
            m_treeItem = new AlgorithmImito(m_ui->treeWidget_Gost,(AlgorithmInfo::CryptType)this->m_typeCrypt,true);
            m_ui->treeWidget_Gost->expandAll();

            break;
        }
        default: {
            throw GenericExc(QObject::tr("Неизвестный тип шифрования"));
            break;
        }
    }
    m_ui->pushButtonWriteDatatoFile->setEnabled(false);
}

void GostCrypt::SetDataToTablesView(QTableWidget &tableWidgetN1, QTableWidget &tableWidgetN2,
                                    QTableWidget &tableWidgetKZY, QTableWidget &tableWidgetK) {
    PutDataBinToTable(m_dataList.at(m_dataCount*2), tableWidgetN1, 0);
    PutDataBinToTable(m_dataList.at(m_dataCount*2+1), tableWidgetN2, 0);
    PutDataBinToTable(m_keyList, tableWidgetKZY);
    PutKBinToTable(tableWidgetK);
    SRCreateItems();
}

void GostCrypt::SetDataToTablesView(QTableWidget &tableWidgetCM0, QTableWidget &tableWidgetKZY, QTableWidget &tableWidgetK) {
    PutDataBinToTable(m_dataList.at(m_dataCount*2), tableWidgetCM0, 0);
    PutDataBinToTable(m_dataList.at(m_dataCount*2+1), tableWidgetCM0, 1);
    PutDataBinToTable(m_keyList, tableWidgetKZY);
    PutKBinToTable(tableWidgetK);
    SRCreateItems();
}

void GostCrypt::SetDataToTablesView(QTableWidget &tableWidgetN1, QTableWidget &tableWidgetN2,
                                    QTableWidget &tableWidgetKZY, QTableWidget &tableWidgetK,
                                    QTableWidget &tableWidgetData) {
    PutDataBinToTable(m_sList.at(0), tableWidgetN1, 0);
    PutDataBinToTable(m_sList.at(1), tableWidgetN2, 0);
    PutDataBinToTable(m_keyList, tableWidgetKZY);
    PutKBinToTable(tableWidgetK);
    PutDataBinToTable(m_dataList.at(m_dataCount*2), tableWidgetData, 0);
    PutDataBinToTable(m_dataList.at(m_dataCount*2+1), tableWidgetData, 1);
    SRCreateItems();
}

void GostCrypt::PutDataToTable(u::uint32 data, QTableWidget &tableWidget, int row) {
    QString hexadecimal;
    hexadecimal.setNum(data, 16);
    //while (hexadecimal.contains("00")) hexadecimal=hexadecimal.replace("00","");
    int i = 0;
    int k = 8 - hexadecimal.length();
    if (hexadecimal.length() != 8) {
        for (i; i < (8 - hexadecimal.length()); i++){
            QTableWidgetItem *newItem = new QTableWidgetItem("0");
            tableWidget.setItem(row, i, newItem);
        }
    }
    for (k = 0; k < hexadecimal.length(); k++) {
            QTableWidgetItem *newItem = new QTableWidgetItem(hexadecimal.at(k).toUpper());
            tableWidget.setItem(row, i++, newItem);
    }
}

void GostCrypt::SetInFileName(QLineEdit& edit) {
    QString in = edit.text();
    m_inFileName = in;
}

void GostCrypt::SetOutFileName(QLineEdit &edit) {
    QString in = edit.text();
    m_outFileName = in;
}

void GostCrypt::OnPushButtonInKeyClicked() {
    Clear();
    FillPath(*m_ui->lineEditInKey, false);
    QString keyPath = m_ui->lineEditInKey->text();
    QFile file(keyPath);
    QString err;
    try {
        if (keyPath.isEmpty()) {
            m_ui->lineEditInKey->clear();
            m_ui->lineEditInKey->setFocus();
            throw GenericExc(QObject::tr("Ключ шифрования не задан"));
        } else if (!file.open(QIODevice::ReadOnly)) {
            m_ui->lineEditInKey->clear();
            m_ui->lineEditInKey->setFocus();
            throw GenericExc(QObject::tr("Ошибка открытия файла ключа шифрования %0").arg(keyPath));
        } else if (file.size() != (KEY_SIZE)) {
            m_ui->lineEditInKey->clear();
            m_ui->lineEditInKey->setFocus();
            throw GenericExc(QObject::tr("Размер файла %0 должен быть равен 32 байтам").arg(keyPath));
        }
        int blocks = KEY_SIZE/SIZE_BLOCK;
        u::uint32 temp;
        if (!m_keyList.isEmpty()) {
            m_keyList.clear();
        }
        for (int j = 0; j < blocks; j++) {
            if (file.read(reinterpret_cast<char*>(&temp), SIZE_BLOCK) <= 0) {
                file.close();
                throw GenericExc(QObject::tr("Ошибка чтения файла ключа шифрования %0").arg(keyPath));
            }
            m_keyList.append(temp);
            PutDataToTable(m_keyList.at(j), *m_ui->tableWidgetKey, j);
        }
        file.close();
    } catch (const GenericExc& exc) {
        err = exc.GetWhat();
    } catch (...) {
        err = tr("Неизвестная ошибка");
    }
    if (!err.isEmpty()) {
        QMessageBox::critical(this, tr("Ошибка"), tr("Ошибка чтения файла ключа шифрования: %0").arg(err));
    }
    SetEnabledButtonView();
}

void GostCrypt::OnPushButtonInTableClicked() {
    Clear();
    FillPath(*m_ui->lineEditInTable, false);
    QString tablePath = m_ui->lineEditInTable->text();
    QFile file(tablePath);
    QString err;
    try {
        if (tablePath.isEmpty()) {
            m_ui->lineEditInTable->clear();
            m_ui->lineEditInTable->setFocus();
            throw GenericExc(QObject::tr("Таблица замен не задана"));
        } else if (!file.open(QIODevice::ReadOnly)) {
            m_ui->lineEditInTable->clear();
            m_ui->lineEditInTable->setFocus();
            throw GenericExc(QObject::tr("Ошибка открытия файла таблицы замен %0").arg(tablePath));
        } else if (file.size() != TABLE_SIZE) {
            m_ui->lineEditInTable->clear();
            m_ui->lineEditInTable->setFocus();
            throw GenericExc(QObject::tr("Размер файла таблицы замен %0 должен быть равен 64 байта").arg(tablePath));
        }
        int blocks = TABLE_SIZE/(2*SIZE_BLOCK);
        u::uint64 temp, columns = 0;
        if (!m_table.isEmpty()) {
            m_table.clear();
        }
        for (int j = 0; j < blocks; j++) {
                if (file.read(reinterpret_cast<char*>(&temp), SIZE_BLOCK*2) <= 0) {
                    file.close();
                    throw GenericExc(QObject::tr("Ошибка чтения файла таблицы замен %0").arg(tablePath));
                }
                QString hexadecimal;
                hexadecimal.setNum(temp, 16);
                QList<u::uint8> temp;
                for (int i = 0; i < 16; i++) {
                    QTableWidgetItem *newItem = new QTableWidgetItem(hexadecimal.at(15-i).toUpper());
                    m_ui->tableWidgetInTable->setItem(i, 7-columns, newItem);
                    temp.append(u::ConvertToHex(hexadecimal.at(15-i)));
                }
                m_table.append(temp);
                columns++;
        }
    file.close();
    } catch (const GenericExc& exc) {
        err = exc.GetWhat();
    } catch (...) {
        err = tr("Неизвестная ошибка");
    }
    if (!err.isEmpty()) {
        QMessageBox::critical(this, tr("Ошибка"), tr("Ошибка чтения файла таблицы замен: %0").arg(err));
    }
    SetEnabledButtonView();
}


void GostCrypt::PutDataBinToTable(QList<u::uint32> dataList, QTableWidget &tableWidget) {
    for (int i = 0; i < dataList.length(); i++) {
        PutDataBinToTable(dataList.at(i), tableWidget, i);
    }
}

void GostCrypt::PutDataBinToTable(u::uint32 data, QTableWidget &tableWidget, int row) {
     std::bitset<32> bits(data);
     QString temp;
     for (int i = 0; i < 32; i++) {
         if (bits[31-i]) {
             temp.append("1");
         } else {
             temp.append("0");
         }
     }

      for (int i = 7; i >= 0; i--) {
          QString tempSt;
          for (int k = 0; k < 4; k++) {
            tempSt.append(temp.at(i*4+k));
          }
          QTableWidgetItem *newItem = new QTableWidgetItem(tempSt);
          tableWidget.setItem(row, i, newItem);
      }
}

void GostCrypt::PutKBinToTable(QTableWidget &tableWidget) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 16; j++) {
            u::uint8 t = m_table.at(i).at(j);
            std::bitset<4> bits(t);
            QString temp;
            for (int k = 0; k < 4; k++) {
                if (bits[3-k]) {
                    temp.append("1");
                } else {
                    temp.append("0");
                }
            }
            QTableWidgetItem *newItem = new QTableWidgetItem(temp);
            tableWidget.setItem(j, 7-i, newItem);
        }
    }
}

void GostCrypt::IncreaseCount() {
    if (m_count < 6) {
        m_count++;
    } else {
        m_count = 1;
    }
}

void GostCrypt::OnPushButtonContinueSRClicked() {
    SRClearTablesStyle();
    ClearModeSRStackedWidget();
    SRHideandShowActiveLabels();
    m_count = 0;
    m_cycle = 1;
    PutDataToTable(m_cryptDataList.at(m_dataCount*2), *m_ui->tableWidgetOutputData, m_dataCount*2);
    PutDataToTable(m_cryptDataList.at(m_dataCount*2+1), *m_ui->tableWidgetOutputData, m_dataCount*2+1);
    if (m_dataCount < m_dataList.size()/2-1) {
        m_dataCount++;
        PutDataBinToTable(m_dataList.at(m_dataCount*2), *m_ui->tableWidgetN1, 0);
        PutDataBinToTable(m_dataList.at(m_dataCount*2+1), *m_ui->tableWidgetN2, 0);
        m_ui->PushButtonSRNext->setEnabled(true);
    } else {
        m_ui->pushButtonSRcontinue->setEnabled(false);
        m_ui->PushButtonSRNext->setEnabled(false);
        m_ui->pushButtonCryptView->setEnabled(true);
        m_ui->pushButtonWriteDatatoFile->setEnabled(true);
        m_ui->label_Hide->hide();
        m_ui->label_HideButton->hide();
        if (m_typeCrypt == u::Encrypt) {
            QMessageBox::information(this, tr("Информация"), tr("Зашифрование данных из файла %0 в режиме простой замены выполнено успешно").arg(m_inFileName));
        } else {
            QMessageBox::information(this, tr("Информация"), tr("Расшифрование данных из файла  %0 в режиме простой замены выполнено успешно").arg(m_inFileName));
        }
    }
    SRCreateItems();

}

void GostCrypt::SRClearTablesStyle() {
    PaintRow(*m_ui->tableWidgetN1, 0, white);
    PaintRow(*m_ui->tableWidgetCM1, 0, white);
    PaintRow(*m_ui->tableWidgetK1, 0, white);
    PaintRow(*m_ui->tableWidgetR, 0, white);
    PaintRow(*m_ui->tableWidgetCM2, 0, white);
    PaintRow(*m_ui->tableWidgetN2, 0, white);
    for (int i = 0; i < m_ui->tableWidgetKZY->rowCount(); i++) {
        PaintRow(*m_ui->tableWidgetKZY, i, white);
    }
    for (int i = 0; i < m_ui->tableWidgeK->rowCount(); i++) {
        PaintRow(*m_ui->tableWidgeK, i, white);
    }
}

void GostCrypt::PaintRow(QTableWidget &tableWidget, int row, QColor& color) {
    for (int i = 0; i < tableWidget.columnCount(); i++) {
       tableWidget.item(row, i)->setBackgroundColor(color);
    }
}

void GostCrypt::PaintItem(QTableWidget &tableWidget, int row, int column, QColor color) {
    tableWidget.item(row, column)->setBackgroundColor(color);
}

void GostCrypt::SRHideandShowActiveLabels() {
    m_ui->labelPic1->show();
    m_ui->labelPic1->setEnabled(true);
    m_ui->labelPic3->show();
    m_ui->labelPic3->setEnabled(true);
    m_ui->labelPic4->show();
    m_ui->labelPic4->setEnabled(true);
    m_ui->labelPic5->show();
    m_ui->labelPic5->setEnabled(true);
    m_ui->labelPic6->show();
    m_ui->labelPic6->setEnabled(true);
    m_ui->label_Pic11->show();
    m_ui->label_Pic11->setEnabled(true);
    m_ui->label_Picture2_2->show();
    m_ui->label_Picture2_2->setEnabled(true);
    m_ui->label_Picture3_3->show();
    m_ui->label_Picture3_3->setEnabled(true);
    m_ui->label_Picture4_4->show();
    m_ui->label_Picture4_4->setEnabled(true);
    m_ui->label_Picture5_5->show();
    m_ui->label_Picture5_5->setEnabled(true);
    m_ui->labelPic1_1->hide();
    m_ui->labelPic3_1->hide();
    m_ui->labelPic4_1->hide();
    m_ui->labelPic5_1->hide();
    m_ui->labelPic6_1->hide();
    m_ui->label_Pic11color->hide();
    m_ui->label_Picture2_2color->hide();
    m_ui->label_Picture3_3color->hide();
    m_ui->label_Picture4_4color->hide();
    m_ui->label_Picture5_5color->hide();
}

void GostCrypt::SREmitTableN1() {
    if (m_count == 1) {
         m_ui->labelPic1->hide();
         m_ui->labelPic1_1->show();
    }
    if (m_count == 6 && m_cycle != 32) {
        m_ui->label_Picture3_3->hide();
        m_ui->label_Picture3_3color->show();
        PutDataBinToTable(m_gostSRList->GetCryptStruct()[m_dataCount].at(m_cycle-1).N1, *m_ui->tableWidgetN1, 0);
    }
    PaintRow(*m_ui->tableWidgetN1, 0, blue);
}

void GostCrypt::SREmitTableCM1() {
    if (m_count == 1) {
        PutDataBinToTable(m_gostSRList->GetCryptStruct()[m_dataCount].at(m_cycle-1).CM1, *m_ui->tableWidgetCM1, 0);
    }
    PaintRow(*m_ui->tableWidgetCM1, 0, blue);
}

void GostCrypt::SREmitTableR() {
    if (m_count == 3) {
        m_ui->labelPic5->hide();
        m_ui->labelPic5_1->show();
        PutDataBinToTable(m_gostSRList->GetCryptStruct()[m_dataCount].at(m_cycle-1).R2, *m_ui->tableWidgetR, 0);
    } else if (m_count == 4) {
        m_ui->labelPic6->hide();
        m_ui->labelPic6_1->show();
    }
    PaintRow(*m_ui->tableWidgetR, 0, blue);
}

void GostCrypt::SREmitTableCM2() {
    if (m_count == 4) {
        PutDataBinToTable(m_gostSRList->GetCryptStruct()[m_dataCount].at(m_cycle-1).CM2, *m_ui->tableWidgetCM2, 0);
    } else if (m_count == 5 && m_cycle == 32) {
        m_ui->label_Picture4_4->hide();
        m_ui->label_Picture4_4color->show();
        m_ui->label_Picture4_4color->setEnabled(true);
    }
    PaintRow(*m_ui->tableWidgetCM2, 0, blue);
}

void GostCrypt::SREmitTableN2() {
    if (m_count == 5 && m_cycle != 32) {
      m_ui->label_Picture5_5->hide();
      m_ui->label_Picture5_5color->show();
      m_ui->label_Picture5_5color->setEnabled(true);
      PutDataBinToTable(m_gostSRList->GetCryptStruct()[m_dataCount].at(m_cycle-1).N2, *m_ui->tableWidgetN2, 0);
    } else if (m_count == 4) {
        m_ui->label_Picture2_2->hide();
        m_ui->label_Picture2_2color->show();
    } else if (m_count == 5 && m_cycle == 32) {
        m_ui->label_Picture4_4->hide();
        m_ui->label_Picture4_4color->show();
        m_ui->label_Picture4_4color->setEnabled(true);
        PutDataBinToTable(m_gostSRList->GetCryptStruct()[m_dataCount].at(m_cycle-1).N2, *m_ui->tableWidgetN2, 0);
    }
    PaintRow(*m_ui->tableWidgetN2, 0, blue);
}

void GostCrypt::SREmitTableKey() {
    m_ui->label_Pic11->hide();
    m_ui->label_Pic11color->show();
    m_ui->label_Pic11color->setEnabled(true);
    PaintRow(*m_ui->tableWidgetKZY, m_gostSRList->GetCryptStruct()[m_dataCount].at(m_cycle-1).ActiveKey, blue);
}

void GostCrypt::SREmitTableItemCM1() {
    QColor color(135, 206, 250);
    PaintItem(*m_ui->tableWidgetCM1, 0, m_iter, color);

}

void GostCrypt::SREmitTableK() {
    m_ui->labelPic3->hide();
    m_ui->labelPic3_1->show();
    int column = (int)m_gostSRList->GetCryptStruct()[m_dataCount].at(m_cycle-1).ActiveTableList.at(7-m_iter).at(0);
    int row = (int)m_gostSRList->GetCryptStruct()[m_dataCount].at(m_cycle-1).ActiveTableList.at(7-m_iter).at(1);
    PaintItem(*m_ui->tableWidgeK, row, 7-column, blue);
}

void GostCrypt::SREmitTableItemK1() {
    m_ui->labelPic4->hide();
    m_ui->labelPic4_1->show();
    PaintItem(*m_ui->tableWidgetK1, 0, m_iter, blue);
    std::bitset<32> bits(m_gostSRList->GetCryptStruct()[m_dataCount].at(m_cycle-1).R1);
    QString temp, tempSt;
    for (int i = 0; i < 32; i++) {
        if (bits[31-i]) {
            temp.append("1");
        } else {
            temp.append("0");
        }
    }
    for (int i = m_iter*4; i < m_iter*4 + 4; i++) {
         tempSt.append(temp[i]);
        m_ui->tableWidgetK1->item(0, m_iter)->setText(tempSt);
    }
    m_iter--;
}

void GostCrypt::SREmitTableK1() {
    PaintRow(*m_ui->tableWidgetK1, 0, blue);
}

void GostCrypt::EmitNextStepInTreeWidget()
{
    this->m_treeItem->selectNextItem(true);
}

/*void GostCrypt::SREmitNextStepTreeWidget()
{
    alg->;
}*/

void GostCrypt::SRSetButtonEnabledorDisenabled(){
    if (m_ui->PushButtonSRNext->isEnabled()) {
        m_ui->label_Hide->show();
        m_ui->label_HideButton->show();
        m_ui->PushButtonSRNext->setEnabled(false);
        m_ui->pushButtonSRcontinue->setEnabled(false);
    } else {
        m_ui->label_Hide->hide();
        m_ui->label_HideButton->hide();
        m_ui->PushButtonSRNext->setEnabled(true);
        m_ui->pushButtonSRcontinue->setEnabled(true);
    }
}



void GostCrypt::OnPushButtonNextSRClicked() {
    IncreaseCount();

       if (m_cycle <= 32) {
           this->m_treeItem->selectNextItem();
           if (m_count == 1) {                
                SRClearTablesStyle();
                SRHideandShowActiveLabels();
                QTimer::singleShot(0, this, SLOT(SREmitTableN1()));
                QTimer::singleShot(0, this, SLOT(SRSetButtonEnabledorDisenabled()));
                QTimer::singleShot(500, this, SLOT(SREmitTableKey()));
                QTimer::singleShot(1000, this, SLOT(SREmitTableCM1()));
                QTimer::singleShot(1000, this, SLOT(SRSetButtonEnabledorDisenabled()));
           } else if (m_count == 2) {   //  TODO через цикл for               
               SRClearTablesStyle();
               SRHideandShowActiveLabels();

                m_iter = 7;
                QTimer::singleShot(0, this, SLOT(SRSetButtonEnabledorDisenabled()));
                QTimer::singleShot(0, this, SLOT(SREmitTableItemCM1()));
                QTimer::singleShot(500, this, SLOT(SREmitTableK()));
                QTimer::singleShot(1000, this, SLOT(SREmitTableItemK1()));
                QTimer::singleShot(1500, this, SLOT(SREmitTableItemCM1()));
                QTimer::singleShot(2000, this, SLOT(SREmitTableK()));
                QTimer::singleShot(2500, this, SLOT(SREmitTableItemK1()));
                QTimer::singleShot(3000, this, SLOT(SREmitTableItemCM1()));
                QTimer::singleShot(3500, this, SLOT(SREmitTableK()));
                QTimer::singleShot(4000, this, SLOT(SREmitTableItemK1()));
                QTimer::singleShot(4500, this, SLOT(SREmitTableItemCM1()));
                QTimer::singleShot(5000, this, SLOT(SREmitTableK()));
                QTimer::singleShot(5500, this, SLOT(SREmitTableItemK1()));
                QTimer::singleShot(6000, this, SLOT(SREmitTableItemCM1()));
                QTimer::singleShot(6500, this, SLOT(SREmitTableK()));
                QTimer::singleShot(7000, this, SLOT(SREmitTableItemK1()));
                QTimer::singleShot(7500, this, SLOT(SREmitTableItemCM1()));
                QTimer::singleShot(8000, this, SLOT(SREmitTableK()));
                QTimer::singleShot(8500, this, SLOT(SREmitTableItemK1()));
                QTimer::singleShot(9000, this, SLOT(SREmitTableItemCM1()));
                QTimer::singleShot(9500, this, SLOT(SREmitTableK()));
                QTimer::singleShot(10000, this, SLOT(SREmitTableItemK1()));
                QTimer::singleShot(10500, this, SLOT(SREmitTableItemCM1()));
                QTimer::singleShot(11000, this, SLOT(SREmitTableK()));
                QTimer::singleShot(11500, this, SLOT(SREmitTableItemK1()));
                QTimer::singleShot(11500, this, SLOT(SRSetButtonEnabledorDisenabled()));
           } else if (m_count == 3) {
                SRClearTablesStyle();
                SRHideandShowActiveLabels();
                this->m_treeItem->selectAlgoritmItem(m_count-1);
                QTimer::singleShot(0, this, SLOT(SRSetButtonEnabledorDisenabled()));
                QTimer::singleShot(0, this, SLOT(SREmitTableK1()));
                QTimer::singleShot(500, this, SLOT(SREmitTableR()));
                QTimer::singleShot(500, this, SLOT(SRSetButtonEnabledorDisenabled()));
           } else if (m_count == 4) {
                SRClearTablesStyle();
                SRHideandShowActiveLabels();
                QTimer::singleShot(0, this, SLOT(SRSetButtonEnabledorDisenabled()));
                QTimer::singleShot(0, this, SLOT(SREmitTableR()));
                QTimer::singleShot(500, this, SLOT(SREmitTableN2()));
                QTimer::singleShot(1000, this, SLOT(SREmitTableCM2()));
                QTimer::singleShot(1000, this, SLOT(SRSetButtonEnabledorDisenabled()));
           } else if (m_count == 5 && m_cycle != 32) {
               SRClearTablesStyle();
               SRHideandShowActiveLabels();

               QTimer::singleShot(0, this, SLOT(SRSetButtonEnabledorDisenabled()));
               QTimer::singleShot(0, this, SLOT(SREmitTableN1()));
               QTimer::singleShot(500, this, SLOT(SREmitTableN2()));
               QTimer::singleShot(500, this, SLOT(SRSetButtonEnabledorDisenabled()));
           } else if (m_count == 6 && m_cycle != 32) {
               SRClearTablesStyle();
               SRHideandShowActiveLabels();
               QTimer::singleShot(0, this, SLOT(SRSetButtonEnabledorDisenabled()));
               QTimer::singleShot(0, this, SLOT(SREmitTableCM2()));
               QTimer::singleShot(500, this, SLOT(SREmitTableN1()));
               QTimer::singleShot(500, this, SLOT(SRSetButtonEnabledorDisenabled()));
               QTimer::singleShot(505, this, SLOT(IncreaseCycle()));
            } else if (m_count == 5 && m_cycle == 32) {
               SRClearTablesStyle();
               QTimer::singleShot(0, this, SLOT(SRSetButtonEnabledorDisenabled()));
               QTimer::singleShot(0, this, SLOT(SREmitTableCM2()));
               QTimer::singleShot(500, this, SLOT(SREmitTableN2()));
               QTimer::singleShot(500, this, SLOT(SRSetButtonEnabledorDisenabled()));
               QTimer::singleShot(505, this, SLOT(IncreaseCycle()));
           }
        } else {
           m_ui->PushButtonSRNext->setEnabled(false);
           SRHideandShowActiveLabels();           
           SRClearTablesStyle();
           PutDataToTable(m_cryptDataList.at(m_dataCount*2), *m_ui->tableWidgetOutputData, m_dataCount*2);
           PutDataToTable(m_cryptDataList.at(m_dataCount*2+1), *m_ui->tableWidgetOutputData, m_dataCount*2+1);
           if (m_typeCrypt == u::Encrypt) {
               QMessageBox::information(this, tr("Информация"), tr("Зашифрование блока данных из файла %0 в режиме простой замены выполнено успешно").arg(m_inFileName));
           } else {
               QMessageBox::information(this, tr("Информация"),tr("Расшифрование блока данных из файла %0 в режиме простой замены выполнено успешно").arg(m_inFileName));
           }
           m_ui->pushButtonWriteDatatoFile->setEnabled(true);
       }
}

void GostCrypt::IncreaseCycle() {
  if (m_cycle <= 32) {
      m_cycle++;
  }
}

//функция для создания ячеек, чтобы их можно было закрасить/добавить данные и т.д.
void GostCrypt::SRCreateItems() {
    switch (m_mode) {
        case ModeSimpleReplacement:
            for (int i = 0; i < m_ui->tableWidgetCM1->columnCount(); i++) {
                QTableWidgetItem* item1 = new QTableWidgetItem();
                m_ui->tableWidgetCM1->setItem(0, i, item1);
                QTableWidgetItem* item2 = new QTableWidgetItem();
                m_ui->tableWidgetCM2->setItem(0, i, item2);
                QTableWidgetItem* item3 = new QTableWidgetItem();
                m_ui->tableWidgetK1->setItem(0, i, item3);
                QTableWidgetItem* item4 = new QTableWidgetItem();
                m_ui->tableWidgetR->setItem(0, i, item4);
            }
            break;
        case ModeGamm:
            for (int i = 0; i < m_ui->tableWidgetGammCM1->columnCount(); i++) {
                QTableWidgetItem* item1 = new QTableWidgetItem();
                m_ui->tableWidgetGammCM1->setItem(0, i, item1);
                QTableWidgetItem* item2 = new QTableWidgetItem();
                m_ui->tableWidgetGammCM2->setItem(0, i, item2);
                QTableWidgetItem* item3 = new QTableWidgetItem();
                m_ui->tableWidgetGammR1->setItem(0, i, item3);
                QTableWidgetItem* item4 = new QTableWidgetItem();
                m_ui->tableWidgetGammR2->setItem(0, i, item4);
                QTableWidgetItem* item5 = new QTableWidgetItem();
                m_ui->tableWidgetGammN3->setItem(0, i, item5);
                QTableWidgetItem* item6 = new QTableWidgetItem();
                m_ui->tableWidgetGammN4->setItem(0, i, item6);
                QTableWidgetItem* item7 = new QTableWidgetItem();
                m_ui->tableWidgetGammCM3->setItem(0, i, item7);
                QTableWidgetItem* item8 = new QTableWidgetItem();
                m_ui->tableWidgetGammCM4->setItem(0, i, item8);
                for (int j = 0; j < m_ui->tableWidgetGammCM5->rowCount(); j++) {
                    QTableWidgetItem* item9 = new QTableWidgetItem();
                    m_ui->tableWidgetGammCM5->setItem(j, i, item9);
                }
            }
            break;
        case ModeGammFeedback:
            for (int i = 0; i < m_ui->tableWidgetGammFeedBackCM1->columnCount(); i++) {
                QTableWidgetItem* item3 = new QTableWidgetItem();
                m_ui->tableWidgetGammFeedBackCM1->setItem(0, i, item3);
                QTableWidgetItem* item4 = new QTableWidgetItem();
                m_ui->tableWidgetGammFeedBackR1->setItem(0, i, item4);
                QTableWidgetItem* item5 = new QTableWidgetItem();
                m_ui->tableWidgetGammFeedBackR2->setItem(0, i, item5);
                QTableWidgetItem* item6 = new QTableWidgetItem();
                m_ui->tableWidgetGammFeedBackCM2->setItem(0, i, item6);
                for (int j = 0; j < m_ui->tableWidgetGammFeedBackCM5->rowCount(); j++) {
                    QTableWidgetItem* item9 = new QTableWidgetItem();
                    m_ui->tableWidgetGammFeedBackCM5->setItem(j, i, item9);
                }
            }
            break;
        case ModeAuthCode:
        for (int i = 0; i < m_ui->tableWidgetImitoCM1->columnCount(); i++) {
            QTableWidgetItem* item1 = new QTableWidgetItem();
            m_ui->tableWidgetImitoCM1->setItem(0, i, item1);
            QTableWidgetItem* item2 = new QTableWidgetItem();
            m_ui->tableWidgetImitoCM2->setItem(0, i, item2);
            QTableWidgetItem* item3 = new QTableWidgetItem();
            m_ui->tableWidgetImitoR1->setItem(0, i, item3);
            QTableWidgetItem* item4 = new QTableWidgetItem();
            m_ui->tableWidgetImitoR2->setItem(0, i, item4);
            QTableWidgetItem* item7 = new QTableWidgetItem();
            m_ui->tableWidgetImitoCM3->setItem(0, i, item7);
            QTableWidgetItem* item8 = new QTableWidgetItem();
            m_ui->tableWidgetImitoCM4->setItem(0, i, item8);
            QTableWidgetItem* item9 = new QTableWidgetItem();
            m_ui->tableWidgetImitoIv->setItem(0, i, item9);
            QTableWidgetItem* item10 = new QTableWidgetItem();
            m_ui->tableWidgetImitoN1->setItem(0, i, item10);
            QTableWidgetItem* item11 = new QTableWidgetItem();
            m_ui->tableWidgetImitoN2->setItem(0, i, item11);
            for (int j = 0; j < m_ui->tableWidgetGammCM5->rowCount(); j++) {
                QTableWidgetItem* item9 = new QTableWidgetItem();
                m_ui->tableWidgetImitoCM5->setItem(j, i, item9);
            }
        }
            break;
        default:
            throw GenericExc(QObject::tr("Неизвестный тип шифрования"));
            break;
    }
}
void GostCrypt::CryptGostSimpleReplacement() {
    m_cryptDataList.clear();
    m_gostSRList = new CryptGostSR(&m_dataList, &m_keyList, &m_table);
    if (m_typeCrypt == u::Encrypt) {
        m_gostSRList->Crypt();
    } else if (m_typeCrypt == u::Decrypt) {
        m_gostSRList->Decrypt();
    }
    if (!m_cryptDataList.isEmpty()) {
        m_cryptDataList.clear();
    }
    for (int i = 0; i < m_dataList.size()/2; i++) {
        m_cryptDataList.append(m_gostSRList->GetCryptStruct()[i].at(31).N1);
        m_cryptDataList.append(m_gostSRList->GetCryptStruct()[i].at(31).N2);
    }
}

void GostCrypt::CryptGostGamm() {
    m_cryptDataList.clear();
    m_gostGammList = new CryptGostGamma(&m_dataList, &m_keyList, &m_table, &m_sList);
    // тип шифрование/расшифрование не имеет значения, вызывается одна функция
    m_gostGammList->Crypt();
    if (!m_cryptDataList.isEmpty()) {
        m_cryptDataList.clear();
    }
    for (int i = 0; i < m_dataList.size(); i++) {
        m_cryptDataList.append(m_gostGammList->GetCM5().at(i));
        m_cryptDataList.append(m_gostGammList->GetCM5().at(i+1));
        i++;
    }
}
// m_count = 1: с потоком шифруем имитовставку
// m_count = 2: N1->N3; N2->N4
// m_count = 3: CM3 = N3+N5; CM4 = N4+N5
// m_count = 4: CM3->N3->N1; CM4->N4->N2
// m_count = 5: с потоком шифруем сумму
// m_count = 6: CM5->CM0^N1
void GostCrypt::GammHideandShowActiveLabels() {
    m_ui->label_Pic1->setEnabled(true);
    m_ui->label_Pic1->show();
    m_ui->label_Pic1_color->hide();
    m_ui->label_Pic9->setEnabled(true);
    m_ui->label_Pic9->show();
    m_ui->label_Pic9color->hide();
    m_ui->label_Pic3->setEnabled(true);
    m_ui->label_Pic3->show();
    m_ui->label_Pic3color->hide();
    m_ui->label_Pic4->setEnabled(true);
    m_ui->label_Pic4->show();
    m_ui->label_Pic4color->hide();
    m_ui->label_Pic4->setEnabled(true);
    m_ui->label_Pic4->show();
    m_ui->label_Pic4color->hide();
    m_ui->label_Pic5->setEnabled(true);
    m_ui->label_Pic5->show();
    m_ui->label_Pic5color->hide();
    m_ui->label_Pic6->setEnabled(true);
    m_ui->label_Pic6->show();
    m_ui->label_Pic6color->hide();
    m_ui->label_Pic10->setEnabled(true);
    m_ui->label_Pic10->show();
    m_ui->label_Pic10_color->hide();
    m_ui->label_Pic11_2->setEnabled(true);
    m_ui->label_Pic11_2->show();
    m_ui->label_Pic11_color->hide();
    m_ui->label_Pic12->setEnabled(true);
    m_ui->label_Pic12->show();
    m_ui->label_Pic12_color->hide();
    m_ui->label_Pic19->setEnabled(true);
    m_ui->label_Pic19->show();
    m_ui->label_Pic19_color->hide();
    m_ui->label_Pic13->setEnabled(true);
    m_ui->label_Pic13->show();
    m_ui->label_Pic13_color->hide();
    m_ui->label_Pic14->setEnabled(true);
    m_ui->label_Pic14->show();
    m_ui->label_Pic14_color->hide();
    m_ui->label_Pic15->setEnabled(true);
    m_ui->label_Pic15->show();
    m_ui->label_Pic15_color->hide();
    m_ui->label_Pic16->setEnabled(true);
    m_ui->label_Pic16->show();
    m_ui->label_Pic16_color->hide();
    m_ui->label_Pic17->setEnabled(true);
    m_ui->label_Pic17->show();
    m_ui->label_Pic17_color->hide();
    m_ui->label_Pic18->setEnabled(true);
    m_ui->label_Pic18->show();
    m_ui->label_Pic18_color->hide();
    m_ui->label_Pic20->setEnabled(true);
    m_ui->label_Pic20->show();
    m_ui->label_Pic20_color->hide();
    m_ui->label_Pic21->setEnabled(true);
    m_ui->label_Pic21->show();
    m_ui->label_Pic21_color->hide();
    m_ui->label_Pic22->setEnabled(true);
    m_ui->label_Pic22->show();
    m_ui->label_Pic22_color->hide();
    m_ui->label_Pic23->setEnabled(true);
    m_ui->label_Pic23->show();
    m_ui->label_Pic23_color->hide();
}

void GostCrypt::GammClearTablesStyle() {
    PaintRow(*m_ui->tableWidgetGammCM3, 0, white);
    PaintRow(*m_ui->tableWidgetGammCM4, 0, white);
    PaintRow(*m_ui->tableWidgetGammN3, 0, white);
    PaintRow(*m_ui->tableWidgetGammN4, 0, white);
    PaintRow(*m_ui->tableWidgetGammN1, 0, white);
    PaintRow(*m_ui->tableWidgetGammN2, 0, white);
    PaintRow(*m_ui->tableWidgetGammN6, 0, white);
    PaintRow(*m_ui->tableWidgetGammN5, 0, white);
    PaintRow(*m_ui->tableWidgetGammCM0, 0, white);
    PaintRow(*m_ui->tableWidgetGammCM0, 1, white);
    PaintRow(*m_ui->tableWidgetGammCM5, 0, white);
    PaintRow(*m_ui->tableWidgetGammCM5, 1, white);

}

void GostCrypt::GammTablesStyleClear() {
    PaintRow(*m_ui->tableWidgetGammN1, 0, white);
    PaintRow(*m_ui->tableWidgetGammN2, 0, white);
    PaintRow(*m_ui->tableWidgetGammCM1, 0, white);
    PaintRow(*m_ui->tableWidgetGammCM2, 0, white);
    PaintRow(*m_ui->tableWidgetGammR1, 0, white);
    PaintRow(*m_ui->tableWidgetGammR2, 0, white);
    for (int i = 0; i < m_ui->tableWidgetGammKZY->rowCount(); i++) {
        PaintRow(*m_ui->tableWidgetGammKZY, i, white);
    }
    for (int i = 0; i < m_ui->tableWidgetGammK->rowCount(); i++) {
        PaintRow(*m_ui->tableWidgetGammK, i, white);
    }
}

void GostCrypt::GammEmitTableN1() {
    if (m_count == 6) {
        PaintRow(*m_ui->tableWidgetGammN1, 0, blue);
    } if (m_count == 2) {
        m_ui->label_Pic18->hide();
        m_ui->label_Pic18_color->show();
        m_ui->label_Pic18_color->setEnabled(true);
        PaintRow(*m_ui->tableWidgetGammN1, 0, blue);
    } else if (m_count == 4) {
        m_ui->label_Pic18->hide();
        m_ui->label_Pic18_color->show();
        m_ui->label_Pic18_color->setEnabled(true);
        PutDataBinToTable(m_gostGammList->GetDataSumN1().at(m_dataCount), *m_ui->tableWidgetGammN1, 0);
        PaintRow(*m_ui->tableWidgetGammN1, 0, blue);
    } else {
        return;
    }
}

void GostCrypt::GammEmitTableN1(char step, char cycle) {
    u::uint32 data;
    if (m_count == 5) {
        data = m_gostGammList->GetCryptStructList()[m_dataCount].at(cycle).N1;
    } else if (m_count == 1) {
        data = m_gostGammList->GetCryptIv().at(cycle).N1;
    } else {
        return;
    }
    if (step == 1) {
        PaintRow(*m_ui->tableWidgetGammN1, 0, blue);
        m_ui->label_Pic1->hide();
        m_ui->label_Pic1_color->show();
        m_ui->label_Pic1_color->setEnabled(true);
    } else if (step == 5) {
        GammHideandShowActiveLabels();
        PaintRow(*m_ui->tableWidgetGammN1, 0, blue);
        m_ui->label_Pic19->hide();
        m_ui->label_Pic19_color->show();
        m_ui->label_Pic19_color->setEnabled(true);
    } else if (step == 6) {
        m_ui->label_Pic21->hide();
        m_ui->label_Pic21_color->show();
        m_ui->label_Pic21_color->setEnabled(true);
        PutDataBinToTable(data, *m_ui->tableWidgetGammN1, 0);
        PaintRow(*m_ui->tableWidgetGammN1, 0, blue);
    } else {
        return;
    }
}

void GostCrypt::GammEmitTableCM1(char cycle) {
    u::uint32 data;
    if (m_count == 1) {
        data = m_gostGammList->GetCryptIv().at(cycle).CM1;
    } else if (m_count == 5) {
        data = m_gostGammList->GetCryptStructList()[m_dataCount].at(cycle).CM1;
    } else {
        return;
    }
    GammHideandShowActiveLabels();
    PutDataBinToTable(data, *m_ui->tableWidgetGammCM1, 0);
    PaintRow(*m_ui->tableWidgetGammCM1, 0, blue);
}

void GostCrypt::GammEmitTableN2() {
    if (m_count == 6) {
        PaintRow(*m_ui->tableWidgetGammN2, 0, blue);
    } if (m_count == 2) {
        m_ui->label_Pic17->hide();
        m_ui->label_Pic17_color->show();
        m_ui->label_Pic17_color->setEnabled(true);
        PaintRow(*m_ui->tableWidgetGammN2, 0, blue);
    } else if (m_count == 4) {
        m_ui->label_Pic17->hide();
        m_ui->label_Pic17_color->show();
        m_ui->label_Pic17_color->setEnabled(true);
        PutDataBinToTable(m_gostGammList->GetDataSumN2().at(m_dataCount), *m_ui->tableWidgetGammN2, 0);
        PaintRow(*m_ui->tableWidgetGammN2, 0, blue);
    } else {
        return;
    }
}

void GostCrypt::GammEmitTableN2(char step, char cycle) {
    u::uint32 data;
    if (m_count == 5) {
        data = m_gostGammList->GetCryptStructList()[m_dataCount].at(cycle).N2;
    } else if (m_count == 1) {
        data = m_gostGammList->GetCryptIv().at(cycle).N2;
    } else {
        return;
    }
    if (step == 4) {
        PaintRow(*m_ui->tableWidgetGammN2, 0, blue);
        m_ui->label_Pic10->hide();
        m_ui->label_Pic10_color->show();
        m_ui->label_Pic10_color->setEnabled(true);
    } else if (step == 5) {
        PutDataBinToTable(data, *m_ui->tableWidgetGammN2, 0);
        PaintRow(*m_ui->tableWidgetGammN2, 0, blue);
    } else {
        return;
    }
}

void GostCrypt::GammEmitTableKZY(char cycle) {
    int row;
    if (m_count == 1) {
        row = m_gostGammList->GetCryptIv().at(cycle).ActiveKey;
    } else if (m_count == 5) {
        row = m_gostGammList->GetCryptStructList()[m_dataCount].at(cycle).ActiveKey;
    } else {
        return;
    }
    m_ui->label_Pic9->hide();
    m_ui->label_Pic9color->show();
    m_ui->label_Pic9color->setEnabled(true);
    PaintRow(*m_ui->tableWidgetGammKZY, row, blue);
}

void GostCrypt::GammEmitTableK(char iter, char cycle) {
    int row, column;
    if (m_count == 1) {
        row = m_gostGammList->GetCryptIv().at(cycle).ActiveTableList.at(7-iter).at(1);
        column = m_gostGammList->GetCryptIv().at(cycle).ActiveTableList.at(7-iter).at(0);
    } else if (m_count == 5) {
        row = m_gostGammList->GetCryptStructList()[m_dataCount].at(cycle).ActiveTableList.at(7-iter).at(1);
        column = m_gostGammList->GetCryptStructList()[m_dataCount].at(cycle).ActiveTableList.at(7-iter).at(0);
    } else {
        return;
    }
    PaintItem(*m_ui->tableWidgetGammK, row, 7-column, blue);

}

void GostCrypt::GammEmitTableR1() {
    GammHideandShowActiveLabels();
    PaintRow(*m_ui->tableWidgetGammR1, 0, blue);
    m_ui->label_Pic5->hide();
    m_ui->label_Pic5color->show();
    m_ui->label_Pic5color->setEnabled(true);
}

void GostCrypt::GammEmitTableR2(char step, char cycle) {
    u::uint32 data;
    if (m_count == 1) {
        data = m_gostGammList->GetCryptIv().at(cycle).R2;
    } else if (m_count == 5) {
        data = m_gostGammList->GetCryptStructList()[m_dataCount].at(cycle).R2;
    } else {
        return;
    }
    if (step == 3) {
        PutDataBinToTable(data, *m_ui->tableWidgetGammR2, 0);
        PaintRow(*m_ui->tableWidgetGammR2, 0, blue);
    } else if (step == 4) {
        GammHideandShowActiveLabels();
        PaintRow(*m_ui->tableWidgetGammR2, 0, blue);
        m_ui->label_Pic6->hide();
        m_ui->label_Pic6color->show();
        m_ui->label_Pic6color->setEnabled(true);
    } else {
        return;
    }
}

void GostCrypt::GammEmitTableCM2(char step, char cycle) {
    u::uint32 data;
    if (m_count == 1) {
        data = m_gostGammList->GetCryptIv().at(cycle).CM2;
    } else if (m_count == 5) {
        data = m_gostGammList->GetCryptStructList()[m_dataCount].at(cycle).CM2;
    } else {
        return;
    }
    if (step == 4) {
        PutDataBinToTable(data, *m_ui->tableWidgetGammCM2, 0);
        PaintRow(*m_ui->tableWidgetGammCM2, 0, blue);
    } else if (step == 6 ) {
        GammHideandShowActiveLabels();
        PaintRow(*m_ui->tableWidgetGammCM2, 0, blue);
        m_ui->label_Pic12->hide();
        m_ui->label_Pic12_color->show();
        m_ui->label_Pic12_color->setEnabled(true);
    } else if (step == 5) {
        GammHideandShowActiveLabels();
        PaintRow(*m_ui->tableWidgetGammCM2, 0, blue);
        m_ui->label_Pic11_2->hide();
        m_ui->label_Pic11_color->show();
        m_ui->label_Pic11_color->setEnabled(true);
    } else {
        return;
    }
}

void GostCrypt::GammEmitTableN6() {
    PaintRow(*m_ui->tableWidgetGammN6, 0, blue);
    m_ui->label_Pic14->hide();
    m_ui->label_Pic14_color->show();
    m_ui->label_Pic14_color->setEnabled(true);
}

void GostCrypt::GammEmitTableN5() {
    PaintRow(*m_ui->tableWidgetGammN5, 0, blue);
    m_ui->label_Pic13->hide();
    m_ui->label_Pic13_color->show();
    m_ui->label_Pic13_color->setEnabled(true);
}

void GostCrypt::GammEmitTableCM4() {
    if (m_count == 3) {
        PutDataBinToTable(m_gostGammList->GetDataSumN2().at(m_dataCount), *m_ui->tableWidgetGammCM4, 0);
        PaintRow(*m_ui->tableWidgetGammCM4, 0, blue);
    } else if (m_count == 4) {
        PaintRow(*m_ui->tableWidgetGammCM4, 0, blue);
        m_ui->label_Pic15->hide();
        m_ui->label_Pic15_color->show();
        m_ui->label_Pic15_color->setEnabled(true);
    } else {
        return;
    }
}

void GostCrypt::GammEmitTableCM3() {
    if (m_count == 3) {
        PutDataBinToTable(m_gostGammList->GetDataSumN1().at(m_dataCount), *m_ui->tableWidgetGammCM3, 0);
        PaintRow(*m_ui->tableWidgetGammCM3, 0, blue);
    } else if (m_count == 4) {
        PaintRow(*m_ui->tableWidgetGammCM3, 0, blue);
        m_ui->label_Pic16->hide();
        m_ui->label_Pic16_color->show();
        m_ui->label_Pic16_color->setEnabled(true);
    } else {
        return;
    }
}

void GostCrypt::GammEmitTableN4() {
    if (m_count == 2) {
        PutDataBinToTable(m_gostGammList->GetN4().at(m_dataCount), *m_ui->tableWidgetGammN4, 0);
        PaintRow(*m_ui->tableWidgetGammN4, 0, blue);
    } else if (m_count == 3) {
        m_ui->label_Pic15->hide();
        m_ui->label_Pic15_color->show();
        m_ui->label_Pic15_color->setEnabled(true);
        PaintRow(*m_ui->tableWidgetGammN4, 0, blue);
    } else if (m_count == 4) {
        PutDataBinToTable(m_gostGammList->GetDataSumN2().at(m_dataCount), *m_ui->tableWidgetGammN4, 0);
        PaintRow(*m_ui->tableWidgetGammN4, 0, blue);
        m_ui->label_Pic15->hide();
        m_ui->label_Pic15_color->show();
        m_ui->label_Pic15_color->setEnabled(true);
    } else {
        return;
    }
}

void GostCrypt::GammEmitTableN3() {
    if (m_count == 2) {
        PutDataBinToTable(m_gostGammList->GetN3().at(m_dataCount), *m_ui->tableWidgetGammN3, 0);
        PaintRow(*m_ui->tableWidgetGammN3, 0, blue);
    } else if (m_count == 3) {
        m_ui->label_Pic16->hide();
        m_ui->label_Pic16_color->show();
        m_ui->label_Pic16_color->setEnabled(true);
        PaintRow(*m_ui->tableWidgetGammN3, 0, blue);
    } else if (m_count == 4) {
        PutDataBinToTable(m_gostGammList->GetDataSumN1().at(m_dataCount), *m_ui->tableWidgetGammN3, 0);
        PaintRow(*m_ui->tableWidgetGammN3, 0, blue);
    } else {
        return;
    }
}

void GostCrypt::GammEmitItemR1(char column, char cycle) {
    m_ui->label_Pic4->hide();
    m_ui->label_Pic4color->show();
    m_ui->label_Pic4color->setEnabled(true);
    if (m_count == 1) {
        PaintItem(*m_ui->tableWidgetGammR1, 0, column, blue);
        std::bitset<32> bits(m_gostGammList->GetCryptIv().at(cycle).R1);
        QString temp, tempSt;
        for (int i = 0; i < 32; i++) {
            if (bits[31-i]) {
                temp.append("1");
            } else {
                temp.append("0");
            }
        }
        for (int i = column*4; i < column*4 + 4; i++) {
             tempSt.append(temp[i]);
        }
         m_ui->tableWidgetGammR1->item(0, column)->setText(tempSt);
    } else if (m_count == 5) {
        PaintItem(*m_ui->tableWidgetGammR1, 0, column, blue);
        std::bitset<32> bits(m_gostGammList->GetCryptStructList()[m_dataCount].at(cycle).R1);
        QString temp, tempSt;
        for (int i = 0; i < 32; i++) {
            if (bits[31-i]) {
                temp.append("1");
            } else {
                temp.append("0");
            }
        }
        for (int i = column*4; i < column*4 + 4; i++) {
             tempSt.append(temp[i]);
        }
        m_ui->tableWidgetGammR1->item(0, column)->setText(tempSt);
    } else {
        return;
    }
}

void GostCrypt::GammEmitItemCM1(char column) {
    GammHideandShowActiveLabels();
    m_ui->label_Pic3->hide();
    m_ui->label_Pic3color->show();
    m_ui->label_Pic3color->setEnabled(true);
    PaintItem(*m_ui->tableWidgetGammCM1, 0, column, blue);
}

void GostCrypt::ThreadUtils() {
    QObject::connect(m_sleeperThread, SIGNAL(SentCM1(char)), this,
                     SLOT(GammEmitTableCM1(char)));
    QObject::connect(m_sleeperThread, SIGNAL(SentKZY(char)), this,
                     SLOT(GammEmitTableKZY(char)));
    QObject::connect(m_sleeperThread, SIGNAL(SentK(char, char)), this,
                     SLOT(GammEmitTableK(char, char)));
    QObject::connect(m_sleeperThread, SIGNAL(SentR1()), this,
                     SLOT(GammEmitTableR1()));
    QObject::connect(m_sleeperThread, SIGNAL(SentR2(char, char)), this,
                     SLOT(GammEmitTableR2(char, char)));
    QObject::connect(m_sleeperThread, SIGNAL(SentN1(char, char)), this,
                     SLOT(GammEmitTableN1(char, char)));
    QObject::connect(m_sleeperThread, SIGNAL(SentN2(char, char)), this,
                     SLOT(GammEmitTableN2(char, char)));
    QObject::connect(m_sleeperThread, SIGNAL(SentCM2(char, char)), this,
                     SLOT(GammEmitTableCM2(char, char)));
    QObject::connect(m_sleeperThread, SIGNAL(SentR1Item(char, char)), this,
                     SLOT(GammEmitItemR1(char, char)));
    QObject::connect(m_sleeperThread, SIGNAL(SentClearTablesStyle()), this,
                     SLOT(GammTablesStyleClear()));
    QObject::connect(m_sleeperThread, SIGNAL(SentCM1Item(char)), this,
                     SLOT(GammEmitItemCM1(char)));
    QObject::connect(m_sleeperThread, SIGNAL(SentButtonsEnabled()),this, SLOT(GammSetPushButtonEnabledorDisanabled()));
    QObject::connect(m_sleeperThread, SIGNAL(StepFinish()),this, SLOT(EmitNextStepInTreeWidget()));

}

void GostCrypt::GammSetPushButtonEnabledorDisanabled() {
    m_ui->pushButtonGammStop->setEnabled(false);
    m_ui->pushButtonGammNext->setEnabled(true);
}

void GostCrypt::GammPushButtonNextEnabledorDisanabled() {
    if (m_ui->pushButtonGammNext->isEnabled()) {
        m_ui->label_Hide->show();
        m_ui->label_HideButton->show();
        m_ui->pushButtonGammNext->setEnabled(false);
    } else {
        m_ui->label_Hide->hide();
        m_ui->label_HideButton->hide();
        m_ui->pushButtonGammNext->setEnabled(true);
    }
}

void GostCrypt::OnPushButtonGammStopClicked() {

    GammHideandShowActiveLabels();
    m_sleeperThread->terminate();
    GammSetPushButtonEnabledorDisanabled();
    delete m_sleeperThread;
    m_sleeperThread = NULL;
    m_ui->tableWidgetGammCM1->clear();
    m_ui->tableWidgetGammCM2->clear();
    m_ui->tableWidgetGammR1->clear();
    m_ui->tableWidgetGammR2->clear();
    m_ui->label_Hide->hide();
    m_ui->label_HideButton->hide();
    for (int i = 0; i < m_ui->tableWidgetGammCM1->columnCount(); i++) {
        QTableWidgetItem* item1 = new QTableWidgetItem();
        m_ui->tableWidgetGammCM1->setItem(0, i, item1);
        QTableWidgetItem* item2 = new QTableWidgetItem();
        m_ui->tableWidgetGammCM2->setItem(0, i, item2);
        QTableWidgetItem* item3 = new QTableWidgetItem();
        m_ui->tableWidgetGammR1->setItem(0, i, item3);
        QTableWidgetItem* item4 = new QTableWidgetItem();
        m_ui->tableWidgetGammR2->setItem(0, i, item4);
    }
    GammTablesStyleClear();
    if (m_count == 1) {        
        this->m_treeItem->selectNextTopItem(true);
        PutDataBinToTable(m_gostGammList->GetCryptIv().at(31).N1, *m_ui->tableWidgetGammN1, 0);
        PutDataBinToTable(m_gostGammList->GetCryptIv().at(31).N2, *m_ui->tableWidgetGammN2, 0);
    } else if (m_count == 5) {
        this->m_treeItem->selectNextTopItem(true);
        PutDataBinToTable(m_gostGammList->GetCryptStructList()[m_dataCount].at(31).N1, *m_ui->tableWidgetGammN1, 0);
        PutDataBinToTable(m_gostGammList->GetCryptStructList()[m_dataCount].at(31).N2, *m_ui->tableWidgetGammN2, 0);
    } else {
        return;
    }
    PaintRow(*m_ui->tableWidgetGammN1, 0, blue);
    PaintRow(*m_ui->tableWidgetGammN2, 0, blue);
}

void GostCrypt::GammEmitTableCM0() {
    if (m_iter == 0) {
        m_ui->label_Pic20->hide();
        m_ui->label_Pic20_color->show();
        m_ui->label_Pic20_color->setEnabled(true);
    } else if (m_iter == 1) {
        m_ui->label_Pic21->hide();
        m_ui->label_Pic21_color->show();
        m_ui->label_Pic21_color->setEnabled(true);
    } else return;
    PaintRow(*m_ui->tableWidgetGammCM0, m_iter, blue);
}

void GostCrypt::GammEmitTableCM5() {
    if (m_iter == 0) {
        m_ui->label_Pic23->hide();
        m_ui->label_Pic23_color->show();
        m_ui->label_Pic23_color->setEnabled(true);
    } else if (m_iter == 1) {
        m_ui->label_Pic22->hide();
        m_ui->label_Pic22_color->show();
        m_ui->label_Pic22_color->setEnabled(true);
    } else return;
    PutDataBinToTable(m_gostGammList->GetCM5().at(2*m_dataCount + m_iter), *m_ui->tableWidgetGammCM5, m_iter);
    PaintRow(*m_ui->tableWidgetGammCM5, m_iter, blue);
    m_iter++;
}

void GostCrypt::OnPushButtonGammNextClicked() {
        IncreaseCount();
        this->m_treeItem->selectNextItem(true);
        switch (m_count) {            
            case (1): {
                this->m_treeItem->selectAlgoritmItem(1,true);
                GammHideandShowActiveLabels();
                m_ui->pushButtonGammNext->setEnabled(false);
                m_ui->pushButtonGammStop->setEnabled(true);
                m_sleeperThread = new GostSleeperThread(false, this);
                ThreadUtils();
                m_sleeperThread->start();
                break;
            }
            case (2): {
                GammClearTablesStyle();
                GammHideandShowActiveLabels();
                m_ui->tableWidgetGammCM5->clear();
                m_ui->tableWidgetGammCM0->clear();
                PutDataBinToTable(m_dataList.at(2*m_dataCount), *m_ui->tableWidgetGammCM0, 0);
                PutDataBinToTable(m_dataList.at(2*m_dataCount+1), *m_ui->tableWidgetGammCM0, 1);
                PaintRow(*m_ui->tableWidgetGammCM0, 0, blue);
                PaintRow(*m_ui->tableWidgetGammCM0, 1, blue);
                for (int i = 0; i < m_ui->tableWidgetGammCM5->columnCount(); i++) {
                    for (int j = 0; j < m_ui->tableWidgetGammCM5->rowCount(); j++) {
                        QTableWidgetItem* item2 = new QTableWidgetItem();
                        m_ui->tableWidgetGammCM5->setItem(j, i, item2);
                 }

                }
                if (m_dataCount == 0) {
                    GammPushButtonNextEnabledorDisanabled();
                    QTimer::singleShot(0, this, SLOT(GammEmitTableN1()));
                    QTimer::singleShot(0, this, SLOT(GammEmitTableN2()));
                    QTimer::singleShot(500, this, SLOT(GammEmitTableN3()));
                    QTimer::singleShot(500, this, SLOT(GammEmitTableN4()));
                    QTimer::singleShot(500, this, SLOT(GammPushButtonNextEnabledorDisanabled()));
                }
                break;
            }
            case (3): {
                this->m_treeItem->selectAlgoritmItem(194,true);
                GammClearTablesStyle();
                GammHideandShowActiveLabels();
                GammPushButtonNextEnabledorDisanabled();
                QTimer::singleShot(0, this, SLOT(GammEmitTableN3()));
                QTimer::singleShot(0, this, SLOT(GammEmitTableN4()));
                QTimer::singleShot(500, this, SLOT(GammEmitTableN5()));
                QTimer::singleShot(500, this, SLOT(GammEmitTableN6()));
                QTimer::singleShot(1000, this, SLOT(GammEmitTableCM3()));
                QTimer::singleShot(1000, this, SLOT(GammEmitTableCM4()));
                QTimer::singleShot(1005, this, SLOT(GammPushButtonNextEnabledorDisanabled()));
                break;
            }
            case (4): {
                GammClearTablesStyle();
                GammHideandShowActiveLabels();
                GammPushButtonNextEnabledorDisanabled();
                QTimer::singleShot(0, this, SLOT(GammEmitTableCM3()));
                QTimer::singleShot(0, this, SLOT(GammEmitTableCM4()));
                QTimer::singleShot(500, this, SLOT(GammEmitTableN3()));
                QTimer::singleShot(500, this, SLOT(GammEmitTableN4()));
                QTimer::singleShot(1000, this, SLOT(GammEmitTableN1()));
                QTimer::singleShot(1000, this, SLOT(GammEmitTableN2()));
                QTimer::singleShot(1005, this, SLOT(GammPushButtonNextEnabledorDisanabled()));
                break;
            }
            case (5): {

                GammClearTablesStyle();
                GammHideandShowActiveLabels();
                m_ui->pushButtonGammNext->setEnabled(false);
                m_ui->pushButtonGammStop->setEnabled(true);
                m_sleeperThread = new GostSleeperThread(false, this);
                ThreadUtils();
                m_sleeperThread->start();
                break;
            }
            case (6): {

                m_iter = 0;
                GammClearTablesStyle();
                GammHideandShowActiveLabels();
                GammPushButtonNextEnabledorDisanabled();
                QTimer::singleShot(0, this, SLOT(GammEmitTableCM0()));
                QTimer::singleShot(1000, this, SLOT(GammEmitTableN1()));
                QTimer::singleShot(2000, this, SLOT(GammEmitTableCM5()));
                QTimer::singleShot(2000, this, SLOT(EmitOutPutWidgetRow1()));
                QTimer::singleShot(3000, this, SLOT(GammEmitTableCM0()));
                QTimer::singleShot(4000, this, SLOT(GammEmitTableN2()));
                QTimer::singleShot(5000, this, SLOT(GammEmitTableCM5()));
                QTimer::singleShot(5000, this, SLOT(EmitOutPutWidgetRow2()));
                QTimer::singleShot(5050, this, SLOT(EmitContinueOrNo()));


                break;
            }
            default: {
                throw GenericExc(QObject::tr("Неверное действие"));
            }
    }
}
void GostCrypt::EmitOutPutWidgetRow1() {
    PutDataToTable(m_cryptDataList.at(2*m_dataCount), *m_ui->tableWidgetOutputData, 2*m_dataCount);
}

void GostCrypt::EmitOutPutWidgetRow2() {
    PutDataToTable(m_cryptDataList.at(2*m_dataCount+1), *m_ui->tableWidgetOutputData, 2*m_dataCount+1);
}

void GostCrypt::EmitContinueOrNo() {
    m_dataCount++;
    if (m_dataCount >= m_dataList.size()/2) {
        if (m_typeCrypt == u::Encrypt) {
            QMessageBox::information(this, tr("Информация"), tr("Зашифрование данных из файла %0 в режиме гаммирования выполнено успешно").arg(m_inFileName));
        } else {
            QMessageBox::information(this, tr("Информация"),tr("Расшифрование данных из файла %0 в режиме гаммирования выполнено успешно").arg(m_inFileName));
        }
        m_ui->pushButtonGammNext->setEnabled(false);
        m_ui->pushButtonCryptView->setEnabled(true);
        m_ui->label_Hide->hide();
        m_ui->label_HideButton->hide();
        m_ui->pushButtonWriteDatatoFile->setEnabled(true);
        return;
    } else {
        GammPushButtonNextEnabledorDisanabled();
    }
    m_count = 1; // т.к. имитовставку мы больше не считаем
}

// алгоритм гаммирования с обратной связью
void GostCrypt::CryptGostGammFeedBackAlg() {
    m_cryptDataList.clear();
    m_gostGammFeedBackList = new CryptGostGammFeedBack(&m_dataList, &m_keyList, &m_table, &m_sList);
    if (m_typeCrypt == u::Encrypt) {
        m_gostGammFeedBackList->Crypt();
    } else if (m_typeCrypt == u::Decrypt) {
        m_gostGammFeedBackList->Decrypt();
    }
    if (!m_cryptDataList.isEmpty()) {
        m_cryptDataList.clear();
    }
    for (int i = 0; i < m_dataList.size(); i++) {
        m_cryptDataList.append(m_gostGammFeedBackList->GetCM5().at(i));
        m_cryptDataList.append(m_gostGammFeedBackList->GetCM5().at(i+1));
        i++;
    }
}
void GostCrypt::GammFeedBackClearTablesInThread() {
    PaintRow(*m_ui->tableWidgetGammFeedBackCM1, 0, white);
    PaintRow(*m_ui->tableWidgetGammFeedBackN2, 0, white);
    PaintRow(*m_ui->tableWidgetGammFeedBackN1, 0, white);
    PaintRow(*m_ui->tableWidgetGammFeedBackCM1, 0, white);
    PaintRow(*m_ui->tableWidgetGammFeedBackCM2, 0, white);
    PaintRow(*m_ui->tableWidgetGammFeedBackR1, 0, white);
    PaintRow(*m_ui->tableWidgetGammFeedBackR2, 0, white);
    for (int i = 0; i < m_ui->tableWidgetGammFeedBackKZY->rowCount(); i++) {
        PaintRow(*m_ui->tableWidgetGammFeedBackKZY, i, white);
    }
    for (int i = 0; i < m_ui->tableWidgetGammFeedBackK->rowCount(); i++) {
        PaintRow(*m_ui->tableWidgetGammFeedBackK, i, white);
    }
}

void GostCrypt::GammFeedBackClearTablesStyle() {
    GammFeedBackClearTablesInThread();
    for (int i = 0; i < m_ui->tableWidgetGammFeedBackCM0->rowCount(); i++) {
        PaintRow(*m_ui->tableWidgetGammFeedBackCM0, i, white);
    }
    for (int i = 0; i < m_ui->tableWidgetGammFeedBackCM5->rowCount(); i++) {
        PaintRow(*m_ui->tableWidgetGammFeedBackCM5, i, white);
    }
}

void GostCrypt::GammFeedBackHideandShowActiveLabels() {
    m_ui->label_GammFeedPic1->show();
    m_ui->label_GammFeedPic1->setEnabled(true);
    m_ui->label_GammFeedPic1_color->hide();
    m_ui->label_GammFeedPic2->show();
    m_ui->label_GammFeedPic2->setEnabled(true);
    m_ui->label_GammFeedPic2_color->hide();
    m_ui->label_GammFeedPic3->show();
    m_ui->label_GammFeedPic3->setEnabled(true);
    m_ui->label_GammFeedPic3_color->hide();
    m_ui->label_GammFeedPic4->show();
    m_ui->label_GammFeedPic4->setEnabled(true);
    m_ui->label_GammFeedPic4_color->hide();
    m_ui->label_GammFeedPic5->show();
    m_ui->label_GammFeedPic5->setEnabled(true);
    m_ui->label_GammFeedPic5_color->hide();
    m_ui->label_GammFeedPic6->show();
    m_ui->label_GammFeedPic6->setEnabled(true);
    m_ui->label_GammFeedPic6_color->hide();
    m_ui->label_GammFeedPic7->show();
    m_ui->label_GammFeedPic7->setEnabled(true);
    m_ui->label_GammFeedPic7_color->hide();
    m_ui->label_GammFeedPic8->show();
    m_ui->label_GammFeedPic8->setEnabled(true);
    m_ui->label_GammFeedPic8_color->hide();
    m_ui->label_GammFeedPic9->show();
    m_ui->label_GammFeedPic9->setEnabled(true);
    m_ui->label_GammFeedPic9_color->hide();
    m_ui->label_GammFeedPic10->show();
    m_ui->label_GammFeedPic10->setEnabled(true);
    m_ui->label_GammFeedPic10_color->hide();
    m_ui->label_GammFeedPic11->show();
    m_ui->label_GammFeedPic11->setEnabled(true);
    m_ui->label_GammFeedPic11_color->hide();
    m_ui->label_GammFeedPic12->show();
    m_ui->label_GammFeedPic12->setEnabled(true);
    m_ui->label_GammFeedPic12_color->hide();
    m_ui->label_GammFeedPic13->show();
    m_ui->label_GammFeedPic13->setEnabled(true);
    m_ui->label_GammFeedPic13_color->hide();
    m_ui->label_GammFeedPic14->show();
    m_ui->label_GammFeedPic14->setEnabled(true);
    m_ui->label_GammFeedPic14_color->hide();
}

void GostCrypt::GammFeedBackEmitItemR1(char column, char cycle) {
    m_ui->label_GammFeedPic4->hide();
    m_ui->label_GammFeedPic4_color->show();
    m_ui->label_GammFeedPic4_color->setEnabled(true);
    if (m_count == 1) {
        PaintItem(*m_ui->tableWidgetGammFeedBackR1, 0, column, blue);
        std::bitset<32> bits(m_gostGammFeedBackList->GetCryptStructList()[m_dataCount].at(cycle).R1);
        QString temp, tempSt;
        for (int i = 0; i < 32; i++) {
            if (bits[31-i]) {
                temp.append("1");
            } else {
                temp.append("0");
            }
        }
        for (int i = column*4; i < column*4 + 4; i++) {
             tempSt.append(temp[i]);
        }
         m_ui->tableWidgetGammFeedBackR1->item(0, column)->setText(tempSt);
    } else {
        return;
    }
}

void GostCrypt::GammFeedBackEmitItemCM1(char column) {
    GammFeedBackHideandShowActiveLabels();
    m_ui->label_GammFeedPic3->hide();
    m_ui->label_GammFeedPic3_color->show();
    m_ui->label_GammFeedPic3_color->setEnabled(true);
    PaintItem(*m_ui->tableWidgetGammFeedBackCM1, 0, column, blue);
}


void GostCrypt::GammFeedBackEmitTableN1() {
     if (m_count == 2) {
         m_ui->label_GammFeedPic11->hide();
         m_ui->label_GammFeedPic11_color->show();
         m_ui->label_GammFeedPic11_color->setEnabled(true);
        PaintRow(*m_ui->tableWidgetGammFeedBackN1, 0, blue);
    } else if (m_count == 3) {
         if (m_typeCrypt == u::Encrypt) {
             m_ui->label_GammFeedPic14->hide();
             m_ui->label_GammFeedPic14_color->show();
             m_ui->label_GammFeedPic14_color->setEnabled(true);
         } else if (m_typeCrypt == u::Decrypt) {
             m_ui->label_GammFeedPic11->hide();
             m_ui->label_GammFeedPic11_color->show();
             m_ui->label_GammFeedPic11_color->setEnabled(true);
         } else {
             return;
         }
        PutDataBinToTable(m_gostGammFeedBackList->GetN1().at(m_dataCount), *m_ui->tableWidgetGammFeedBackN1, 0);
        PaintRow(*m_ui->tableWidgetGammFeedBackN1, 0, blue);
    } else {
        return;
    }
}

void GostCrypt::GammFeedBackEmitTableN1(char step, char cycle) {
    u::uint32 data;
    data = m_gostGammFeedBackList->GetCryptStructList()[m_dataCount].at(cycle).N1;
    if (step == 1) {
        GammFeedBackHideandShowActiveLabels();
        PaintRow(*m_ui->tableWidgetGammFeedBackN1, 0, blue);
        m_ui->label_GammFeedPic1->hide();
        m_ui->label_GammFeedPic1_color->show();
        m_ui->label_GammFeedPic1_color->setEnabled(true);
    } else if (step == 5) {
        GammFeedBackHideandShowActiveLabels();
        PaintRow(*m_ui->tableWidgetGammFeedBackN1, 0, blue);
        m_ui->label_GammFeedPic10->hide();
        m_ui->label_GammFeedPic10_color->show();
        m_ui->label_GammFeedPic10->setEnabled(true);
    } else if (step == 6) {
        m_ui->label_GammFeedPic8->hide();
        m_ui->label_GammFeedPic8_color->show();
        m_ui->label_Pic21_color->setEnabled(true);
        PutDataBinToTable(data, *m_ui->tableWidgetGammFeedBackN1, 0);
        PaintRow(*m_ui->tableWidgetGammFeedBackN1, 0, blue);
    } else {
        return;
    }
}

void GostCrypt::GammFeedBackEmitTableCM1(char cycle) {
    u::uint32 data;
    if (m_count == 1) {
        data = m_gostGammFeedBackList->GetCryptStructList()[m_dataCount].at(cycle).CM1;
    } else {
        return;
    }
    GammFeedBackHideandShowActiveLabels();
    PutDataBinToTable(data, *m_ui->tableWidgetGammFeedBackCM1, 0);
    PaintRow(*m_ui->tableWidgetGammFeedBackCM1, 0, blue);
}

void GostCrypt::GammFeedBackEmitTableN2() {
     if (m_count == 2) {
        m_ui->label_GammFeedPic12->hide();
        m_ui->label_GammFeedPic12_color->show();
        m_ui->label_GammFeedPic12_color->setEnabled(true);
        PaintRow(*m_ui->tableWidgetGammFeedBackN2, 0, blue);

    } else if (m_count == 3) {
        if (m_typeCrypt == u::Encrypt) {
             m_ui->label_GammFeedPic13->hide();
             m_ui->label_GammFeedPic13_color->show();
             m_ui->label_GammFeedPic13_color->setEnabled(true);
        } else if (m_typeCrypt == u::Decrypt) {
             m_ui->label_GammFeedPic12->hide();
             m_ui->label_GammFeedPic12_color->show();
             m_ui->label_GammFeedPic12_color->setEnabled(true);
        } else {
             return;
        }
        PutDataBinToTable(m_gostGammFeedBackList->GetN2().at(m_dataCount), *m_ui->tableWidgetGammFeedBackN2, 0);
        PaintRow(*m_ui->tableWidgetGammFeedBackN2, 0, blue);
    } else {
        return;
    }
}

void GostCrypt::GammFeedBackEmitTableN2(char step, char cycle) {
    u::uint32 data;
    if (m_count == 1) {
        data = m_gostGammFeedBackList->GetCryptStructList()[m_dataCount].at(cycle).N2;
    } else {
        return;
    }
    if (step == 4) {
        PaintRow(*m_ui->tableWidgetGammFeedBackN2, 0, blue);
        m_ui->label_GammFeedPic7->hide();
        m_ui->label_GammFeedPic7_color->show();
        m_ui->label_GammFeedPic7_color->setEnabled(true);
    } else if (step == 5) {
        PutDataBinToTable(data, *m_ui->tableWidgetGammFeedBackN2, 0);
        PaintRow(*m_ui->tableWidgetGammFeedBackN2, 0, blue);
    } else {
        return;
    }
}

void GostCrypt::GammFeedBackEmitTableKZY(char cycle) {
    int row;
    if (m_count == 1) {
        row = m_gostGammFeedBackList->GetCryptStructList()[m_dataCount].at(cycle).ActiveKey;
    } else {
        return;
    }
    PaintRow(*m_ui->tableWidgetGammFeedBackKZY, row, blue);
    m_ui->label_GammFeedPic2->hide();
    m_ui->label_GammFeedPic2_color->show();
    m_ui->label_GammFeedPic2_color->setEnabled(true);
}

void GostCrypt::GammFeedBackEmitTableK(char iter, char cycle) {
    int row, column;
    //if (m_count == 1) {
        row = m_gostGammFeedBackList->GetCryptStructList()[m_dataCount].at(cycle).ActiveTableList.at(7-iter).at(1);
        column = m_gostGammFeedBackList->GetCryptStructList()[m_dataCount].at(cycle).ActiveTableList.at(7-iter).at(0);
   // }
    PaintItem(*m_ui->tableWidgetGammFeedBackK, row, 7-column, blue);

}

void GostCrypt::GammFeedBackEmitTableR1() {
    GammFeedBackHideandShowActiveLabels();
    PaintRow(*m_ui->tableWidgetGammFeedBackR1, 0, blue);
    m_ui->label_GammFeedPic5->hide();
    m_ui->label_GammFeedPic5_color->show();
    m_ui->label_GammFeedPic5_color->setEnabled(true);
}

void GostCrypt::GammFeedBackEmitTableR2(char step, char cycle) {
    u::uint32 data;
    if (m_count == 1) {
        data = m_gostGammFeedBackList->GetCryptStructList()[m_dataCount].at(cycle).R2;
    } else {
        return;
    }
    if (step == 3) {
        PutDataBinToTable(data, *m_ui->tableWidgetGammFeedBackR2, 0);
        PaintRow(*m_ui->tableWidgetGammFeedBackR2, 0, blue);
    } else if (step == 4) {
        GammFeedBackHideandShowActiveLabels();
        PaintRow(*m_ui->tableWidgetGammFeedBackR2, 0, blue);
        m_ui->label_GammFeedPic6->hide();
        m_ui->label_GammFeedPic6_color->show();
        m_ui->label_GammFeedPic6_color->setEnabled(true);
    } else {
        return;
    }
}

void GostCrypt::GammFeedBackEmitTableCM2(char step, char cycle) {
    u::uint32 data;
    if (m_count == 1) {
        data = m_gostGammFeedBackList->GetCryptStructList()[m_dataCount].at(cycle).CM2;
    } else {
        return;
    }
    if (step == 4) {
        PutDataBinToTable(data, *m_ui->tableWidgetGammFeedBackCM2, 0);
        PaintRow(*m_ui->tableWidgetGammFeedBackCM2, 0, blue);
    } else if (step == 6 ) {
        GammFeedBackHideandShowActiveLabels();
        PaintRow(*m_ui->tableWidgetGammFeedBackCM2, 0, blue);
        m_ui->label_GammFeedPic8->hide();
        m_ui->label_GammFeedPic8_color->show();
        m_ui->label_GammFeedPic8_color->setEnabled(true);
    } else if (step == 5) {
        GammFeedBackHideandShowActiveLabels();
        PaintRow(*m_ui->tableWidgetGammFeedBackCM2, 0, blue);
        m_ui->label_GammFeedPic9->hide();
        m_ui->label_GammFeedPic9_color->show();
        m_ui->label_GammFeedPic9_color->setEnabled(true);
    } else {
        return;
    }
}
void GostCrypt::GammFeedBackPushButtonEnabledorDisanabled() {
    m_ui->pushButtonGammFeedBackStop->setEnabled(false);
    m_ui->pushButtonGammFeedBackNext->setEnabled(true);
}

void GostCrypt::GammFeedBackEmitTableCM0() {
    switch (m_count) {
    case 2:
        PaintRow(*m_ui->tableWidgetGammFeedBackCM0, m_iter, blue);
        break;
      case 3:
        if (m_iter == 0) {
            PaintRow(*m_ui->tableWidgetGammFeedBackCM0, 0, blue);
            m_iter++;
        } else if (m_iter == 1) {
            PaintRow(*m_ui->tableWidgetGammFeedBackCM0, 1, blue);
            m_iter = 0;
        }
        break;
    default:
        break;
    }
}

void GostCrypt::GammFeedBackEmitTableCM5() {
    switch (m_count) {
    case 2:
        if (m_iter == 0) {
            m_ui->label_GammFeedPic14->hide();
            m_ui->label_GammFeedPic14_color->show();
            m_ui->label_GammFeedPic14_color->setEnabled(true);
        } else if (m_iter == 1) {
            m_ui->label_GammFeedPic13->hide();
            m_ui->label_GammFeedPic13_color->show();
            m_ui->label_GammFeedPic13_color->setEnabled(true);
        } else return;
        PutDataBinToTable(m_gostGammFeedBackList->GetCM5().at(2*m_dataCount + m_iter), *m_ui->tableWidgetGammFeedBackCM5, m_iter);
        PaintRow(*m_ui->tableWidgetGammFeedBackCM5, m_iter, blue);
        if (m_iter == 0) {
            m_iter++;
        } else {
            m_iter = 0;
        }
        break;
    case 3:
        if (m_iter == 0) {
            PaintRow(*m_ui->tableWidgetGammFeedBackCM5, 0, blue);
            m_iter++;
        } else if (m_iter == 1) {
            PaintRow(*m_ui->tableWidgetGammFeedBackCM5, 1, blue);
            m_iter = 0;
        }
      break;
    default:
        break;
    }

}

void GostCrypt::GammFeedBackEmitContinueOrNo() {
    m_dataCount++;
    m_count = 0;
}

void GostCrypt::GammFeedBackSetButtonEnabledorDisenabled(){
    if (m_ui->pushButtonGammFeedBackNext->isEnabled()) {
        m_ui->label_Hide->show();
        m_ui->label_HideButton->show();
        m_ui->pushButtonGammFeedBackNext->setEnabled(false);
    } else {
        m_ui->label_Hide->hide();
        m_ui->label_HideButton->hide();
        m_ui->pushButtonGammFeedBackNext->setEnabled(true);
    }
}

void GostCrypt::GammFeedBackThreadUtils() {
    QObject::connect(m_sleeperThread, SIGNAL(SentCM1(char)), this,
                     SLOT(GammFeedBackEmitTableCM1(char)));
    QObject::connect(m_sleeperThread, SIGNAL(SentKZY(char)), this,
                     SLOT(GammFeedBackEmitTableKZY(char)));
    QObject::connect(m_sleeperThread, SIGNAL(SentK(char, char)), this,
                     SLOT(GammFeedBackEmitTableK(char, char)));
    QObject::connect(m_sleeperThread, SIGNAL(SentR1()), this,
                     SLOT(GammFeedBackEmitTableR1()));
    QObject::connect(m_sleeperThread, SIGNAL(SentR2(char, char)), this,
                     SLOT(GammFeedBackEmitTableR2(char, char)));
    QObject::connect(m_sleeperThread, SIGNAL(SentN1(char, char)), this,
                     SLOT(GammFeedBackEmitTableN1(char, char)));
    QObject::connect(m_sleeperThread, SIGNAL(SentN2(char, char)), this,
                     SLOT(GammFeedBackEmitTableN2(char, char)));
    QObject::connect(m_sleeperThread, SIGNAL(SentCM2(char, char)), this,
                     SLOT(GammFeedBackEmitTableCM2(char, char)));
    QObject::connect(m_sleeperThread, SIGNAL(SentR1Item(char, char)), this,
                     SLOT(GammFeedBackEmitItemR1(char, char)));
    QObject::connect(m_sleeperThread, SIGNAL(SentClearTablesStyle()), this,
                     SLOT(GammFeedBackClearTablesInThread()));
    QObject::connect(m_sleeperThread, SIGNAL(SentCM1Item(char)), this,
                     SLOT(GammFeedBackEmitItemCM1(char)));
    QObject::connect(m_sleeperThread, SIGNAL(SentButtonsEnabled()),this, SLOT(GammFeedBackPushButtonEnabledorDisanabled()));
    QObject::connect(m_sleeperThread, SIGNAL(StepFinish()),this, SLOT(EmitNextStepInTreeWidget()));
}


void GostCrypt::OnPushButtonGammFeedBackNextClicked() {
    IncreaseCount();
    switch (m_count) {
        case 1: {
            this->m_treeItem->selectNextItem(true);
            //this->m_treeItem->selectNextItem(true);
            if (m_dataCount >= 1) { // при первом цикле начальные данные уже есть в таблице
                PutDataBinToTable(m_dataList.at(2*m_dataCount), *m_ui->tableWidgetGammFeedBackCM0, 0);
                PutDataBinToTable(m_dataList.at(2*m_dataCount+1), *m_ui->tableWidgetGammFeedBackCM0, 1);
                for (int i = 0; i < m_ui->tableWidgetGammFeedBackCM5->columnCount(); i++) {
                    for (int j = 0; j < m_ui->tableWidgetGammFeedBackCM5->rowCount(); j++) {
                        QTableWidgetItem* item2 = new QTableWidgetItem();
                        m_ui->tableWidgetGammFeedBackCM5->setItem(j, i, item2);
                    }
                }
            }
            GammFeedBackHideandShowActiveLabels();
            m_ui->pushButtonGammFeedBackNext->setEnabled(false);
            m_ui->pushButtonGammFeedBackStop->setEnabled(true);
            m_sleeperThread = new GostSleeperThread(false, this);
            GammFeedBackThreadUtils();
            m_sleeperThread->start();
            if (m_sleeperThread->isFinished()) {
                delete m_sleeperThread;
                m_sleeperThread = NULL;
                m_ui->pushButtonGammFeedBackNext->setEnabled(true);
                m_ui->pushButtonGammFeedBackStop->setEnabled(false);
            }
            break;
        }
        case 2: {
            this->m_treeItem->selectNextItem(true);
            m_iter = 0;
            GammFeedBackClearTablesStyle();
            GammFeedBackHideandShowActiveLabels();
            GammFeedBackSetButtonEnabledorDisenabled();
            QTimer::singleShot(0, this, SLOT(GammFeedBackEmitTableCM0()));
            QTimer::singleShot(1000, this, SLOT(GammFeedBackEmitTableN1()));
            QTimer::singleShot(2000, this, SLOT(GammFeedBackEmitTableCM5()));
            QTimer::singleShot(2000, this, SLOT(EmitOutPutWidgetRow1()));
            QTimer::singleShot(3000, this, SLOT(GammFeedBackEmitTableCM0()));
            QTimer::singleShot(4000, this, SLOT(GammFeedBackEmitTableN2()));
            QTimer::singleShot(5000, this, SLOT(GammFeedBackEmitTableCM5()));
            QTimer::singleShot(5000, this, SLOT(EmitOutPutWidgetRow2()));
            QTimer::singleShot(5005, this, SLOT(GammFeedBackSetButtonEnabledorDisenabled()));
            break;
        }
        case 3: {
            this->m_treeItem->selectNextItem(true);
            GammFeedBackClearTablesStyle();
            GammFeedBackHideandShowActiveLabels();
            if (m_dataCount+1 >= m_dataList.size()/2 ) {
                m_ui->label_Hide->hide();
                m_ui->label_HideButton->hide();
                m_ui->pushButtonGammFeedBackNext->setEnabled(false);
                m_ui->pushButtonCryptView->setEnabled(true);
                m_ui->pushButtonWriteDatatoFile->setEnabled(true);
                if (m_typeCrypt == u::Encrypt) {
                    QMessageBox::information(this, tr("Информация"), tr("Зашифрование данных из файла %0 в режиме гаммирования с обратной связью выполнено успешно").arg(m_inFileName));
                } else {
                    QMessageBox::information(this, tr("Информация"),tr("Расшифрование данных из файла %0 в режиме гаммирования с обратной связью выполнено успешно").arg(m_inFileName));
                }
                m_ui->pushButtonGammFeedBackNext->setEnabled(false);
                return;
            } else {
                GammFeedBackSetButtonEnabledorDisenabled();
                switch (m_typeCrypt) {
                case u::Encrypt:
                    QTimer::singleShot(0, this, SLOT(GammFeedBackEmitTableCM5()));
                    QTimer::singleShot(1000, this, SLOT(GammFeedBackEmitTableN1()));
                    QTimer::singleShot(2000, this, SLOT(GammFeedBackEmitTableCM5())); //
                    QTimer::singleShot(3000, this, SLOT(GammFeedBackEmitTableN2()));
                    break;
                case u::Decrypt:
                    QTimer::singleShot(0, this, SLOT(GammFeedBackEmitTableCM0()));
                    QTimer::singleShot(1000, this, SLOT(GammFeedBackEmitTableN1()));
                    QTimer::singleShot(2000, this, SLOT(GammFeedBackEmitTableCM0())); //
                    QTimer::singleShot(3000, this, SLOT(GammFeedBackEmitTableN2()));
                    break;
                default:
                    break;
                }
              QTimer::singleShot(3050, this, SLOT(GammFeedBackEmitContinueOrNo()));
              QTimer::singleShot(3055, this, SLOT(GammFeedBackSetButtonEnabledorDisenabled()));
            }
            break;
        }
    default:
        throw GenericExc(QObject::tr("Неверное действие"));
     }
}

void GostCrypt::OnPushButtonGammFeedBackStopClicked() {
    this->m_treeItem->selectNextTopItem(true);
    GammFeedBackHideandShowActiveLabels();
    m_sleeperThread->terminate();
    GammFeedBackPushButtonEnabledorDisanabled();
    delete m_sleeperThread;
    m_sleeperThread = NULL;
    m_ui->tableWidgetGammFeedBackCM1->clear();
    m_ui->tableWidgetGammFeedBackCM2->clear();
    m_ui->tableWidgetGammFeedBackR1->clear();
    m_ui->tableWidgetGammFeedBackR2->clear();
    m_ui->label_Hide->hide();
    m_ui->label_HideButton->hide();
    for (int i = 0; i < m_ui->tableWidgetGammFeedBackCM1->columnCount(); i++) {
        QTableWidgetItem* item1 = new QTableWidgetItem();
        m_ui->tableWidgetGammFeedBackCM1->setItem(0, i, item1);
        QTableWidgetItem* item2 = new QTableWidgetItem();
        m_ui->tableWidgetGammFeedBackCM2->setItem(0, i, item2);
        QTableWidgetItem* item3 = new QTableWidgetItem();
        m_ui->tableWidgetGammFeedBackR1->setItem(0, i, item3);
        QTableWidgetItem* item4 = new QTableWidgetItem();
        m_ui->tableWidgetGammFeedBackR2->setItem(0, i, item4);
    }
    GammFeedBackClearTablesStyle();
    PutDataBinToTable(m_gostGammFeedBackList->GetCryptStructList()[m_dataCount].at(31).N1, *m_ui->tableWidgetGammFeedBackN1, 0);
    PutDataBinToTable(m_gostGammFeedBackList->GetCryptStructList()[m_dataCount].at(31).N2, *m_ui->tableWidgetGammFeedBackN2, 0);

    PaintRow(*m_ui->tableWidgetGammFeedBackN1, 0, blue);
    PaintRow(*m_ui->tableWidgetGammFeedBackN2, 0, blue);
}

//алгоритм вычисления имитовставки
void GostCrypt::ImitoHideandShowActiveLabels() {
    m_ui->label_ImitoPic1->show();
    m_ui->label_ImitoPic1->setEnabled(true);
    m_ui->label_ImitoPic1_color->hide();
    m_ui->label_ImitoPic2->show();
    m_ui->label_ImitoPic2->setEnabled(true);
    m_ui->label_ImitoPic2_color->hide();
    m_ui->label_ImitoPic3->show();
    m_ui->label_ImitoPic3->setEnabled(true);
    m_ui->label_ImitoPic3_color->hide();
    m_ui->label_ImitoPic4->show();
    m_ui->label_ImitoPic4->setEnabled(true);
    m_ui->label_ImitoPic4_color->hide();
    m_ui->label_ImitoPic5->show();
    m_ui->label_ImitoPic5->setEnabled(true);
    m_ui->label_ImitoPic5_color->hide();
    m_ui->label_ImitoPic6->show();
    m_ui->label_ImitoPic6->setEnabled(true);
    m_ui->label_ImitoPic6_color->hide();
    m_ui->label_ImitoPic7->show();
    m_ui->label_ImitoPic7->setEnabled(true);
    m_ui->label_ImitoPic7_color->hide();
    m_ui->label_ImitoPic8->show();
    m_ui->label_ImitoPic8->setEnabled(true);
    m_ui->label_ImitoPic8_color->hide();
    m_ui->label_ImitoPic9->show();
    m_ui->label_ImitoPic9->setEnabled(true);
    m_ui->label_ImitoPic9_color->hide();
    m_ui->label_ImitoPic12->show();
    m_ui->label_ImitoPic12->setEnabled(true);
    m_ui->label_ImitoPic12_color->hide();
    m_ui->label_ImitoPic13->show();
    m_ui->label_ImitoPic13->setEnabled(true);
    m_ui->label_ImitoPic13_color->hide();
    m_ui->label_ImitoPic14->show();
    m_ui->label_ImitoPic14->setEnabled(true);
    m_ui->label_ImitoPic14_color->hide();
    m_ui->label_ImitoPic15->show();
    m_ui->label_ImitoPic15->setEnabled(true);
    m_ui->label_ImitoPic15_color->hide();
    m_ui->label_ImitoPic16->show();
    m_ui->label_ImitoPic16->setEnabled(true);
    m_ui->label_ImitoPic16_color->hide();
    m_ui->label_ImitoPic17->show();
    m_ui->label_ImitoPic17->setEnabled(true);
    m_ui->label_ImitoPic17_color->hide();
    m_ui->label_ImitoPic18->show();
    m_ui->label_ImitoPic18->setEnabled(true);
    m_ui->label_ImitoPic18_color->hide();
    m_ui->label_ImitoPic19->show();
    m_ui->label_ImitoPic19->setEnabled(true);
    m_ui->label_ImitoPic19_color->hide();
}

void GostCrypt::CryptGostImitoAlg() {
    m_cryptDataList.clear();
    m_gostImitoList = new GryptGostImito();
    m_gostImitoList->ComputeImito(&m_dataList, &m_keyList, &m_table);
    if (!m_cryptDataList.isEmpty()) {
        m_cryptDataList.clear();
    }
    m_cryptDataList.append(m_gostImitoList->GetImito());
}

void GostCrypt::ImitoEmitItemR1(char column, char cycle) {
    m_ui->label_ImitoPic3_color->show();
    m_ui->label_ImitoPic3_color->setEnabled(true);
    m_ui->label_ImitoPic3->hide();
    PaintItem(*m_ui->tableWidgetImitoR1, 0, column, blue);
    std::bitset<32> bits(m_gostImitoList->GetCryptStruct()[m_dataCount].at(cycle).R1);
    QString temp, tempSt;
    for (int i = 0; i < 32; i++) {
        if (bits[31-i]) {
            temp.append("1");
        } else {
            temp.append("0");
        }
    }
    for (int i = column*4; i < column*4 + 4; i++) {
        tempSt.append(temp[i]);
    }
    m_ui->tableWidgetImitoR1->item(0, column)->setText(tempSt);

}

void GostCrypt::ImitoEmitItemCM1(char column) {
    ImitoHideandShowActiveLabels();
    m_ui->label_ImitoPic2_color->show();
    m_ui->label_ImitoPic2_color->setEnabled(true);
    m_ui->label_ImitoPic2->hide();
    PaintItem(*m_ui->tableWidgetImitoCM1, 0, column, blue);
}


void GostCrypt::ImitoEmitTableN1() {
    if (m_first) {
        m_ui->label_ImitoPic15_color->show();
        m_ui->label_ImitoPic15_color->setEnabled(true);
        m_ui->label_ImitoPic15->hide();
        PutDataBinToTable(m_gostImitoList->GetN1().at(m_dataCount), *m_ui->tableWidgetImitoN1, 0);
        PaintRow(*m_ui->tableWidgetImitoN1, 0, blue);
    } else {
        switch (m_count) {
        case 1:
            PaintRow(*m_ui->tableWidgetImitoN1, 0, blue);
            break;
        case 2: {
            m_ui->label_ImitoPic15_color->show();
            m_ui->label_ImitoPic15_color->setEnabled(true);
            m_ui->label_ImitoPic15->hide();
            PutDataBinToTable(m_gostImitoList->GetN1().at(m_dataCount), *m_ui->tableWidgetImitoN1, 0);
            PaintRow(*m_ui->tableWidgetImitoN1, 0, blue);
            break;
        }
        case 3:
            PaintRow(*m_ui->tableWidgetImitoN1, 0, blue);
            break;
        case 4: {
            m_ui->label_ImitoPic12_color->show();
            m_ui->label_ImitoPic12_color->setEnabled(true);
            m_ui->label_ImitoPic12->hide();
            PutDataBinToTable(m_gostImitoList->GetCM3().at(m_dataCount), *m_ui->tableWidgetImitoN1, 0);
            PaintRow(*m_ui->tableWidgetImitoN1, 0, blue);
            break;
        }
        default:
            break;
        }
    }
}

void GostCrypt::ImitoEmitTableN1(char step, char cycle) {
    if (step == 1) {
        ImitoHideandShowActiveLabels();
        PaintRow(*m_ui->tableWidgetImitoN1, 0, blue);
        m_ui->label_ImitoPic1_color->show();
        m_ui->label_ImitoPic1_color->setEnabled(true);
        m_ui->label_ImitoPic1->hide();
    } else if (step == 5) {
        ImitoHideandShowActiveLabels();
        PaintRow(*m_ui->tableWidgetImitoN1, 0, blue);
        m_ui->label_ImitoPic18_color->show();
        m_ui->label_ImitoPic18_color->setEnabled(true);
        m_ui->label_ImitoPic18->hide();
    } else if (step == 6) {
        m_ui->label_ImitoPic8_color->show();
        m_ui->label_ImitoPic8_color->setEnabled(true);
        m_ui->label_ImitoPic8->hide();
        PutDataBinToTable(m_gostImitoList->GetCryptStruct()[m_dataCount].at(cycle).N1,
                          *m_ui->tableWidgetImitoN1, 0);
        PaintRow(*m_ui->tableWidgetImitoN1, 0, blue);
    } else {
        return;
    }
}

void GostCrypt::ImitoEmitTableCM1(char cycle) {
    ImitoHideandShowActiveLabels();
    PutDataBinToTable(m_gostImitoList->GetCryptStruct()[m_dataCount].at(cycle).CM1,
                      *m_ui->tableWidgetImitoCM1, 0);
    PaintRow(*m_ui->tableWidgetImitoCM1, 0, blue);
}

void GostCrypt::ImitoEmitTableN2() {
    if (m_first) {
        m_ui->label_ImitoPic14_color->show();
        m_ui->label_ImitoPic14_color->setEnabled(true);
        m_ui->label_ImitoPic14->hide();
        PutDataBinToTable(m_gostImitoList->GetN2().at(m_dataCount), *m_ui->tableWidgetImitoN2, 0);
        PaintRow(*m_ui->tableWidgetImitoN2, 0, blue);
    } else {
        switch (m_count) {
        case 1:
            PaintRow(*m_ui->tableWidgetImitoN2, 0, blue);
            break;
        case 2: {
            m_ui->label_ImitoPic14_color->show();
            m_ui->label_ImitoPic14_color->setEnabled(true);
            m_ui->label_ImitoPic14->hide();
            PutDataBinToTable(m_gostImitoList->GetN2().at(m_dataCount), *m_ui->tableWidgetImitoN2, 0);
            PaintRow(*m_ui->tableWidgetImitoN2, 0, blue);
            break;
        }
        case 3:
            PaintRow(*m_ui->tableWidgetImitoN2, 0, blue);
            break;
         case 4: {
            m_ui->label_ImitoPic13_color->show();
            m_ui->label_ImitoPic13_color->setEnabled(true);
            m_ui->label_ImitoPic13->hide();
            PutDataBinToTable(m_gostImitoList->GetCM4().at(m_dataCount), *m_ui->tableWidgetImitoN2, 0);
            PaintRow(*m_ui->tableWidgetImitoN2, 0, blue);
            break;
        }
        default:
            break;
        }
    }
}

void GostCrypt::ImitoEmitTableN2(char step, char cycle) {
    if (step == 4) {
        PaintRow(*m_ui->tableWidgetImitoN2, 0, blue);
        m_ui->label_ImitoPic7_color->show();
        m_ui->label_ImitoPic7_color->setEnabled(true);
        m_ui->label_ImitoPic7->hide();
    } else if (step == 5) {
        PutDataBinToTable(m_gostImitoList->GetCryptStruct()[m_dataCount].at(cycle).N2,
                          *m_ui->tableWidgetImitoN2, 0);
        PaintRow(*m_ui->tableWidgetImitoN2, 0, blue);
    } else {
        return;
    }
}

void GostCrypt::ImitoEmitTableKZY(char cycle) {
    PaintRow(*m_ui->tableWidgetImitoKZY, m_gostImitoList->GetCryptStruct()[m_dataCount].at(cycle).ActiveKey,
             blue);
    m_ui->label_ImitoPic6_color->show();
    m_ui->label_ImitoPic6_color->setEnabled(true);
    m_ui->label_ImitoPic6->hide();
}

void GostCrypt::ImitoEmitTableK(char iter, char cycle) {
    int row, column;
    row = m_gostImitoList->GetCryptStruct()[m_dataCount].at(cycle).ActiveTableList.at(7-iter).at(1);
    column = m_gostImitoList->GetCryptStruct()[m_dataCount].at(cycle).ActiveTableList.at(7-iter).at(0);
    PaintItem(*m_ui->tableWidgetImitoK, row, 7-column, blue);
}

void GostCrypt::ImitoEmitTableR1() {
    ImitoHideandShowActiveLabels();
    PaintRow(*m_ui->tableWidgetImitoR1, 0, blue);
    m_ui->label_ImitoPic4_color->show();
    m_ui->label_ImitoPic4_color->setEnabled(true);
    m_ui->label_ImitoPic4->hide();
}

void GostCrypt::ImitoEmitTableR2(char step, char cycle) {
    if (step == 3) {
        PutDataBinToTable(m_gostImitoList->GetCryptStruct()[m_dataCount].at(cycle).R2,
                          *m_ui->tableWidgetImitoR2, 0);
        PaintRow(*m_ui->tableWidgetImitoR2, 0, blue);
    } else if (step == 4) {
        ImitoHideandShowActiveLabels();
        PaintRow(*m_ui->tableWidgetImitoR2, 0, blue);
        m_ui->label_ImitoPic5_color->show();
        m_ui->label_ImitoPic5_color->setEnabled(true);
        m_ui->label_ImitoPic5->hide();
    } else {
        return;
    }
}

void GostCrypt::ImitoEmitTableCM2(char step, char cycle) {
    if (step == 4) {
        PutDataBinToTable(m_gostImitoList->GetCryptStruct()[m_dataCount].at(cycle).CM2,
                          *m_ui->tableWidgetImitoCM2, 0);
        PaintRow(*m_ui->tableWidgetImitoCM2, 0, blue);
    } else if (step == 6 ) {
        ImitoHideandShowActiveLabels();
        PaintRow(*m_ui->tableWidgetImitoCM2, 0, blue);
    } else if (step == 5) {
        ImitoHideandShowActiveLabels();
        PaintRow(*m_ui->tableWidgetImitoCM2, 0, blue);
        m_ui->label_ImitoPic9_color->show();
        m_ui->label_ImitoPic9_color->setEnabled(true);
        m_ui->label_ImitoPic9->hide();
    } else {
        return;
    }

}

void GostCrypt::ImitoEmitTableCM0() {
        PaintRow(*m_ui->tableWidgetImitoCM0, m_iter, blue);
        m_iter++;

}

void GostCrypt::ImitoEmitTableCM5() {
    switch (m_count) {
    case 1:
        if (m_iter == 0) {
            m_ui->label_ImitoPic17_color->show();
            m_ui->label_ImitoPic17_color->setEnabled(true);
            m_ui->label_ImitoPic17->hide();
        } else {
            m_ui->label_ImitoPic16_color->show();
            m_ui->label_ImitoPic16_color->setEnabled(true);
            m_ui->label_ImitoPic16->hide();
        }
        PutDataBinToTable(m_gostImitoList->GetCM5().at(2*m_dataCount + m_iter),
                          *m_ui->tableWidgetImitoCM5, m_iter);
        PaintRow(*m_ui->tableWidgetImitoCM5, m_iter, blue);
        m_iter++;
        break;
    case 3:
        PaintRow(*m_ui->tableWidgetImitoCM5, m_iter, blue);
        if (m_iter == 0) {
            /*m_ui->label_GammFeedPic9->hide();
            m_ui->label_GammFeedPic9_color->show();
            m_ui->label_GammFeedPic9_color->setEnabled(true);*/
        } else {
            /*m_ui->label_GammFeedPic9->hide();
            m_ui->label_GammFeedPic9_color->show();
            m_ui->label_GammFeedPic9_color->setEnabled(true);*/
        }
        m_iter++;
        break;
    case 6:
        PaintRow(*m_ui->tableWidgetImitoCM5, 0, blue);
        break;
    default:
        break;
    }
}

void GostCrypt::ImitoEmitTableIv() {
    m_ui->label_ImitoPic19_color->show();
    m_ui->label_ImitoPic19_color->setEnabled(true);
    m_ui->label_ImitoPic19->hide();
    m_ui->label_Hide->hide();
    m_ui->label_HideButton->hide();
    PutDataBinToTable(m_gostImitoList->GetImito(),
                      *m_ui->tableWidgetImitoIv, 0);
    PaintRow(*m_ui->tableWidgetImitoIv, 0, blue);
    PutDataToTable(m_gostImitoList->GetImito(),
                   *m_ui->tableWidgetOutputData, 0);
    m_ui->pushButtonCryptView->setEnabled(true);
    m_ui->pushButton_ImitoNext->setEnabled(false);
    m_ui->pushButton_ImitoStop->setEnabled(false);
    m_ui->pushButtonWriteDatatoFile->setEnabled(true);
    QMessageBox::information(this, tr("Вычисление имитовставки"), tr("Вычисление имитовставки файла %0 выполнено успешно").arg(m_inFileName));
}

void GostCrypt::ImitoEmitTableCM3() {
    switch (m_count) {
    case 3:
        PutDataBinToTable(m_gostImitoList->GetCM3().at(m_dataCount),
                          *m_ui->tableWidgetImitoCM3, 0);
        PaintRow(*m_ui->tableWidgetImitoCM3,0, blue);
        break;
    case 4:
        PaintRow(*m_ui->tableWidgetImitoCM3,0, blue);        
    default:
        break;
    }
}

void GostCrypt::ImitoEmitTableCM4() {
    switch (m_count) {
    case 3:
        PutDataBinToTable(m_gostImitoList->GetCM4().at(m_dataCount),
                          *m_ui->tableWidgetImitoCM4, 0);
        PaintRow(*m_ui->tableWidgetImitoCM4,0, blue);
        break;
    case 4:
        PaintRow(*m_ui->tableWidgetImitoCM4,0, blue);
    default:
        break;
    }
}

void GostCrypt::ImitoSetPushButtonDisanabled() {
    m_ui->pushButton_ImitoNext->setEnabled(true);
    m_ui->pushButton_ImitoStop->setEnabled(false);
}
void GostCrypt::ImitoPushButtonEnabledorDisanabled() {
    if (m_ui->pushButton_ImitoNext->isEnabled()) {
        m_ui->label_Hide->show();
        m_ui->label_HideButton->show();
        m_ui->pushButton_ImitoNext->setEnabled(false);
    } else {
        m_ui->label_Hide->hide();
        m_ui->label_HideButton->hide();
        m_ui->pushButton_ImitoNext->setEnabled(true);
    }
}
void GostCrypt::ImitoClearTablesInThread() {
    PaintRow(*m_ui->tableWidgetImitoCM1, 0, white);
    PaintRow(*m_ui->tableWidgetImitoN2, 0, white);
    PaintRow(*m_ui->tableWidgetImitoN1, 0, white);
    PaintRow(*m_ui->tableWidgetImitoCM2, 0, white);
    PaintRow(*m_ui->tableWidgetImitoR1, 0, white);
    PaintRow(*m_ui->tableWidgetImitoR2, 0, white);
    for (int i = 0; i < m_ui->tableWidgetImitoKZY->rowCount(); i++) {
        PaintRow(*m_ui->tableWidgetImitoKZY, i, white);
    }
    for (int i = 0; i < m_ui->tableWidgetImitoK->rowCount(); i++) {
        PaintRow(*m_ui->tableWidgetImitoK, i, white);
    }
}

void GostCrypt::ImitoClearTablesStyle() {
    ImitoClearTablesInThread();
    for (int i = 0; i < m_ui->tableWidgetImitoCM0->rowCount(); i++) {
        PaintRow(*m_ui->tableWidgetImitoCM0, i, white);
    }
    for (int i = 0; i < m_ui->tableWidgetImitoCM5->rowCount(); i++) {
        PaintRow(*m_ui->tableWidgetImitoCM5, i, white);
    }
    PaintRow(*m_ui->tableWidgetImitoCM3, 0, white);
    PaintRow(*m_ui->tableWidgetImitoCM4, 0, white);
    PaintRow(*m_ui->tableWidgetImitoIv, 0, white);
}

void GostCrypt::ImitoThreadUtils() {
    QObject::connect(m_sleeperThread, SIGNAL(SentCM1(char)), this,
                     SLOT(ImitoEmitTableCM1(char)));
    QObject::connect(m_sleeperThread, SIGNAL(SentKZY(char)), this,
                     SLOT(ImitoEmitTableKZY(char)));
    QObject::connect(m_sleeperThread, SIGNAL(SentK(char, char)), this,
                     SLOT(ImitoEmitTableK(char, char)));
    QObject::connect(m_sleeperThread, SIGNAL(SentR1()), this,
                     SLOT(ImitoEmitTableR1()));
    QObject::connect(m_sleeperThread, SIGNAL(SentR2(char, char)), this,
                     SLOT(ImitoEmitTableR2(char, char)));
    QObject::connect(m_sleeperThread, SIGNAL(SentN1(char, char)), this,
                     SLOT(ImitoEmitTableN1(char, char)));
    QObject::connect(m_sleeperThread, SIGNAL(SentN2(char, char)), this,
                     SLOT(ImitoEmitTableN2(char, char)));
    QObject::connect(m_sleeperThread, SIGNAL(SentCM2(char, char)), this,
                     SLOT(ImitoEmitTableCM2(char, char)));
    QObject::connect(m_sleeperThread, SIGNAL(SentR1Item(char, char)), this,
                     SLOT(ImitoEmitItemR1(char, char)));
    QObject::connect(m_sleeperThread, SIGNAL(SentClearTablesStyle()), this,
                     SLOT(ImitoClearTablesInThread()));
    QObject::connect(m_sleeperThread, SIGNAL(SentCM1Item(char)), this,
                     SLOT(ImitoEmitItemCM1(char)));
    QObject::connect(m_sleeperThread, SIGNAL(SentButtonsEnabled()),this, SLOT(ImitoSetPushButtonDisanabled()));
    QObject::connect(m_sleeperThread, SIGNAL(StepFinish()),this, SLOT(EmitNextStepInTreeWidget()));
}


void GostCrypt::OnPushButtonImitoNextClicked() {
    IncreaseCount();
    this->m_treeItem->selectNextItem(true);
    if (m_first) {
        switch (m_count) {
        case 1:
            m_iter = 0;
            ImitoClearTablesStyle();
            ImitoHideandShowActiveLabels();
            QTimer::singleShot(0, this, SLOT(ImitoPushButtonEnabledorDisanabled()));
            QTimer::singleShot(0, this, SLOT(ImitoEmitTableCM0()));
            QTimer::singleShot(1000, this, SLOT(ImitoEmitTableN1()));
            QTimer::singleShot(1500, this, SLOT(ImitoEmitTableCM0()));
            QTimer::singleShot(2500, this, SLOT(ImitoEmitTableN2()));
            QTimer::singleShot(2505, this, SLOT(ImitoPushButtonEnabledorDisanabled()));
            break;
        case 2:

            ImitoHideandShowActiveLabels();
            ImitoClearTablesStyle();
            if (m_first) {
                m_first = false;
            }
            m_count = 0;
            m_ui->pushButton_ImitoNext->setEnabled(false);
            m_ui->pushButton_ImitoStop->setEnabled(true);
            if (m_sleeperThread != NULL) {
                delete m_sleeperThread;
            }
            m_sleeperThread = new GostSleeperThread(true, this);
            ImitoThreadUtils();
            m_sleeperThread->start();
            break;
        default:
            break;
        }
    }
    else {
        switch (m_count) {
        case 1:
            m_iter = 0;
            ImitoClearTablesStyle();
            ImitoHideandShowActiveLabels();
            QTimer::singleShot(0, this, SLOT(ImitoPushButtonEnabledorDisanabled()));
            QTimer::singleShot(0, this, SLOT(ImitoEmitTableN1()));
            QTimer::singleShot(1000, this, SLOT(ImitoEmitTableCM5()));
            QTimer::singleShot(2000, this, SLOT(ImitoEmitTableN2()));
            QTimer::singleShot(3000, this, SLOT(ImitoEmitTableCM5()));
            QTimer::singleShot(4500, this, SLOT(ImitoEmitContinueOrNo()));
            if (m_dataCount+1 < m_dataList.size()/2) {
                QTimer::singleShot(3011, this, SLOT(ImitoPushButtonEnabledorDisanabled()));
            }
            break;
        case 2:
            m_iter = 0;
            ImitoClearTablesStyle();
            ImitoHideandShowActiveLabels();
            for (int i = 0; i < m_ui->tableWidgetImitoCM0->rowCount(); i++) {
                PutDataBinToTable(m_dataList.at(m_dataCount + 1 + i),
                                  *m_ui->tableWidgetImitoCM0, i);
            }
            QTimer::singleShot(0, this, SLOT(ImitoPushButtonEnabledorDisanabled()));
            QTimer::singleShot(0, this, SLOT(ImitoEmitTableCM0()));
            QTimer::singleShot(1000, this, SLOT(ImitoEmitTableN1()));
            QTimer::singleShot(1500, this, SLOT(ImitoEmitTableCM0()));
            QTimer::singleShot(2500, this, SLOT(ImitoEmitTableN2()));
            QTimer::singleShot(2505, this, SLOT(ImitoPushButtonEnabledorDisanabled()));
            break;
        case 3:
            ImitoClearTablesStyle();
            ImitoHideandShowActiveLabels();
            ImitoPushButtonEnabledorDisanabled();
            m_iter = 0;
            QTimer::singleShot(0, this, SLOT(ImitoEmitTableN1()));
            QTimer::singleShot(1000, this, SLOT(ImitoEmitTableCM5()));
            QTimer::singleShot(2000, this, SLOT(ImitoEmitTableCM3()));
            QTimer::singleShot(3000, this, SLOT(ImitoEmitTableN2()));
            QTimer::singleShot(4000, this, SLOT(ImitoEmitTableCM5()));
            QTimer::singleShot(5000, this, SLOT(ImitoEmitTableCM4()));
            QTimer::singleShot(5005, this, SLOT(ImitoPushButtonEnabledorDisanabled()));
            break;
        case 4:
            ImitoClearTablesStyle();
            ImitoHideandShowActiveLabels();
            ImitoPushButtonEnabledorDisanabled();
            QTimer::singleShot(0, this, SLOT(ImitoEmitTableCM3()));
            QTimer::singleShot(0, this, SLOT(ImitoEmitTableCM4()));
            QTimer::singleShot(1000, this, SLOT(ImitoEmitTableN1()));
            QTimer::singleShot(1000, this, SLOT(ImitoEmitTableN2()));
            QTimer::singleShot(1005, this, SLOT(ImitoPushButtonEnabledorDisanabled()));
            break;
        case 5:
            this->m_treeItem->selectAlgoritmItem(1,true);
            ImitoHideandShowActiveLabels();
            ImitoClearTablesStyle();
            m_ui->pushButton_ImitoNext->setEnabled(false);
            m_ui->pushButton_ImitoStop->setEnabled(true);
            if (m_sleeperThread != NULL) {
                delete m_sleeperThread;
            }
            m_sleeperThread = new GostSleeperThread(true, this);
            ImitoThreadUtils();
            m_sleeperThread->start();
            m_count = 0;
            break;
        default:
            break;
        }
    }
}

void GostCrypt::ImitoEmitContinueOrNo() {
    m_dataCount++;
    if (m_dataCount >= m_dataList.size()/2) {
        ImitoHideandShowActiveLabels();
        ImitoClearTablesStyle();
        PaintRow(*m_ui->tableWidgetImitoCM5, 0, blue);
        QTimer::singleShot(1000, this, SLOT(ImitoEmitTableIv()));
    }
}

void GostCrypt::OnPushButtonImitoStopClicked() {
    this->m_treeItem->selectNextTopItem(true);
    ImitoHideandShowActiveLabels();
    m_sleeperThread->terminate();
    ImitoSetPushButtonDisanabled();
    delete m_sleeperThread;
    m_sleeperThread = NULL;
    m_ui->tableWidgetImitoCM1->clear();
    m_ui->tableWidgetImitoCM2->clear();
    m_ui->tableWidgetImitoR1->clear();
    m_ui->tableWidgetImitoR2->clear();
    m_ui->label_Hide->hide();
    m_ui->label_HideButton->hide();
    for (int i = 0; i < m_ui->tableWidgetImitoCM1->columnCount(); i++) {
        QTableWidgetItem* item1 = new QTableWidgetItem();
        m_ui->tableWidgetImitoCM1->setItem(0, i, item1);
        QTableWidgetItem* item2 = new QTableWidgetItem();
        m_ui->tableWidgetImitoCM2->setItem(0, i, item2);
        QTableWidgetItem* item3 = new QTableWidgetItem();
        m_ui->tableWidgetImitoR1->setItem(0, i, item3);
        QTableWidgetItem* item4 = new QTableWidgetItem();
        m_ui->tableWidgetImitoR2->setItem(0, i, item4);
    }
    ImitoClearTablesStyle();
    PutDataBinToTable(m_gostImitoList->GetCryptStruct()[m_dataCount].at(15).N1, *m_ui->tableWidgetImitoN1, 0);
    PutDataBinToTable(m_gostImitoList->GetCryptStruct()[m_dataCount].at(15).N2, *m_ui->tableWidgetImitoN2, 0);

    PaintRow(*m_ui->tableWidgetImitoN1, 0, blue);
    PaintRow(*m_ui->tableWidgetImitoN2, 0, blue);
}
