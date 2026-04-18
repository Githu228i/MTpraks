#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "rule.h"

//my includes:
#include <QSet>
#include <QString>
#include <QMap>
#include <QTimer>
#include <QLabel>
#include <QPropertyAnimation>

QT_BEGIN_NAMESPACE

namespace Ui {
    class MainWindow;
}

QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void showError(const QString &msg);

    void WordChange(const QString &text);

    void WordReset();

    void updateTable();

    void addState();

    void removeState();

    Rule parseRule(int row, int col, const QString &text);

    void collectRules();

    QChar getSymbol(int pos);

    void setSymbol(int pos, QChar c);

    void loadWordToTape();

    void step();

    void stopMachine();

    void StartMachineForOneStep();

    void runMachine();

    void startMachine();

    void updateViewOffset();

    void updateTapeView();

    void highlightCurrent();

    void updateHeadPosition();

    void animateTapeShift(int delta);

    void setUiRunning(bool running);

    void blockButtons(bool running);

    bool validateRules();


private slots:
    void AlphabetEnterOpen();
    void WordEnterCheck();
    void ruleChanged(int row, int col);

private:
    Ui::MainWindow *ui;

//my params:
    QSet<QChar> Alphabet;
    QSet<QChar> AddAlphabet;
    QString Word;
    QVector<Rule> Rules;
    QMap<int, QChar> tape;
    int head = 0;
    int currentState = 0;
    bool isRunning = false;
    QTimer *timer;
    int viewOffset = -6;
    QLabel *headLabel;
    QPropertyAnimation *headAnim;
    QPropertyAnimation *tapeAnim;
    int lastViewOffset = 0;

};
#endif // MAINWINDOW_H
