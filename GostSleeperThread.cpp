#include "GostSleeperThread.h"

GostSleeperThread::GostSleeperThread(bool imito, QObject *parent)
    : QThread(parent)
    , m_ms(500)
    , m_imito (imito) {
    if (imito) {
        m_cycle = 16;
    } else {
        m_cycle = 32;
    }
}

GostSleeperThread::~GostSleeperThread() {

}

void GostSleeperThread::run() {
    for (int i = 0; i < m_cycle; i++) {
        ClearTablesStyle();
        N1Sent(1, i); // шаг один
        KZYSent(i);
        CM1Sent(i);
        emit this->StepFinish();
        //шаг два
        ClearTablesStyle();
        for (int j = 0; j < 8; j++) {
            CM1ItemSent(7-j);
            KSent(7-j, i);
            R1ItemSent(7-j, i);
        }
        ClearTablesStyle();
        emit this->StepFinish();
        //шаг три
        R1Sent();
        R2Sent(3, i);
        ClearTablesStyle();
        emit this->StepFinish();
        // шаг 4
        R2Sent(4, i);
        N2Sent(4, i);
        CM2Sent(4, i);
        ClearTablesStyle();
        emit this->StepFinish();
        // шаг 5
        if (i != 31) {
        N1Sent(5, i);
        N2Sent(5, i);
        ClearTablesStyle();
        } else {
           CM2Sent(5, i);
           N2Sent(5, i);
           ClearTablesStyle();
           SentButtonsEnabled();
           return;
        }
        emit this->StepFinish();
        // шаг 6
        CM2Sent(6, i);
        N1Sent(6, i);
        ClearTablesStyle();
        if (m_imito && i == 15) {
            SentButtonsEnabled();
        }
        emit this->StepFinish();
     }
}

void GostSleeperThread::ClearTablesStyle() {
    this->msleep(m_ms);
    emit SentClearTablesStyle();
}

void GostSleeperThread::CM1Sent(char cycle) {
    this->msleep(m_ms);
    emit SentCM1(cycle);
}

void GostSleeperThread::KZYSent(char cycle) {
    this->msleep(m_ms);
    emit SentKZY(cycle);
}

void GostSleeperThread::KSent(char iter, char cycle) {
    this->msleep(m_ms);
    emit SentK(iter, cycle);
}

void GostSleeperThread::R1Sent() {
    this->msleep(m_ms);
    emit SentR1();
}


void GostSleeperThread::R2Sent(char step, char cycle) {
    this->msleep(m_ms);
    emit SentR2(step, cycle);
}

void GostSleeperThread::CM2Sent(char step, char cycle) {
    this->msleep(m_ms);
    emit SentCM2(step, cycle);
}

void GostSleeperThread::N1Sent(char step, char cycle) {
    this->msleep(m_ms);
    emit SentN1(step, cycle);
}

void GostSleeperThread::N2Sent(char step, char cycle) {
    this->msleep(m_ms);
    emit SentN2(step, cycle);
}

void GostSleeperThread::CM1ItemSent(char column) {
    this->msleep(m_ms);
    emit SentCM1Item(column);
}

void GostSleeperThread::R1ItemSent(char column, char cycle) {
    this->msleep(m_ms);
    emit SentR1Item(column, cycle);
}
