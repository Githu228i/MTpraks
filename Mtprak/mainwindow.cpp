#include "mainwindow.h"
#include "./ui_mainwindow.h"

//Connect with other structures:
#include "alphabetenter.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

//conecters:
    connect(ui->AlphabetEnterBut, &QPushButton::clicked, this, &MainWindow::AlphabetEnterOpen);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//My coms:

void MainWindow::AlphabetEnterOpen()
{
    //ui->AlphabetEnterBut->setText("Нажали");
    AlphabetEnter AlphaWindow(this);

    if (AlphaWindow.exec() == QDialog::Accepted)
    {
        QString MainText = AlphaWindow.MainGetter(), AddText = AlphaWindow.AddGetter();
        Alphabet.clear();
        AddAlphabet.clear();
        for (QChar c : MainText)
            Alphabet.insert(c);

        for (QChar c : AddText)
            AddAlphabet.insert(c);

        QString text;

        for (QChar c : Alphabet)
            text += c;

        ui->check1->setText(text);
        text = "";

        for (QChar c : AddAlphabet)
            text += c;

        ui->check2->setText(text);
    }
}
