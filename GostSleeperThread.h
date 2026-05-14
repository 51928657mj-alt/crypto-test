#ifndef GOSTSLEEPERTHREADGAMM_H
#define GOSTSLEEPERTHREADGAMM_H

#include <QThread>
#include <QList>
#include <Crypt/CryptStruct.h>
#include "Types/BitTypes.h"

class GostSleeperThread: public QThread {
    Q_OBJECT
public:
   explicit GostSleeperThread(bool imito, QObject *parent);
   ~GostSleeperThread();
signals:
    void SentCM1(char cycle); // заполнение и выделение
    //void SentCM1(); //просто для выделения
    void SentKZY(char cycle); //выделение строки таблицы
    void SentK(char iter, char cycle); //выделение ячейки таблицы замены
    void SentR1(); // заполнение и выделение
   // void SentR1(); //выделение
    void SentR2(char step, char cycle); // заполнение и выделение
    //void SentR2(); //выделение
    void SentN1(char step, char cycle); // заполнение и выделение
    //void SentN1(); //выделение
    void SentN2(char step, char cycle); // заполнение и выделение
    //void SentN2(); //выделение
    void SentCM2(char step, char cycle); // заполнение и выделение
    //void SentCM2(); //выделение
    void SentR1Item(char column, char cycle); // заполнение по ячейкам
    void SentCM1Item(char column); //выделение по ячейкам
    void SentClearTablesStyle();
    void SentButtonsEnabled();

    void StepFinish(); // Закончилось выполнение шага step
private:
    //void CM1Sent(int ms, int data);
    void CM1ItemSent(char column);
    void R1ItemSent(char column, char cycle);
    void CM1Sent(char cycle);
    void KSent(char iter, char cycle);
    void KZYSent(char cycle);
    void N2Sent(char step, char cycle);
    //void N2Sent(int ms);
    void N1Sent(char step, char cycle);
    //void N1Sent(int ms);
    void R1Sent();
    //void R1Sent(int ms);
    void R2Sent(char step, char cycle);
    //void R2Sent(int ms);
    void CM2Sent(char step, char cycle);
    //void CM2Sent(int ms);
    void ClearTablesStyle();
protected:
    void run();

private:
    int m_ms;
    int m_cycle;
    bool m_imito;
};

#endif // GOSTSLEEPERTHREADGAMM_H
