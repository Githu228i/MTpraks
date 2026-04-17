#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

//my includes:
#include <QSet>
#include <QString>

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

    void updateTable();

    void addState();

    void removeState();


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

};
#endif // MAINWINDOW_H
