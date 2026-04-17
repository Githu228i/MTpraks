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


private slots:
    void AlphabetEnterOpen();

private:
    Ui::MainWindow *ui;

//my params:
    QSet<QChar> Alphabet;
    QSet<QChar> AddAlphabet;
};
#endif // MAINWINDOW_H
