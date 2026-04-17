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
    connect(ui->WordEnter, &QLineEdit::textChanged, this, &MainWindow::WordEnterCheck);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//My coms:

void MainWindow::AlphabetEnterOpen()
{
    AlphabetEnter AlphaWindow(this); // new window

    if (AlphaWindow.exec() == QDialog::Accepted) // Entering new alphabets
    {
        QString MainText = AlphaWindow.MainGetter(), AddText = AlphaWindow.AddGetter();
        Alphabet.clear();
        AddAlphabet.clear();
        ui->WordEnter->setText("");
        WordChange("");
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

void MainWindow::showError(const QString &msg)
{
    statusBar()->showMessage(msg, 2000);
}

void MainWindow::WordEnterCheck() {
    QString text = ui->WordEnter->text();
    if (text.isEmpty())
        return;

    int pos = ui->WordEnter->cursorPosition();

    if (Alphabet.isEmpty()) {
        text.chop(1);

        ui->WordEnter->blockSignals(true);
        ui->WordEnter->setText(text);
        ui->WordEnter->setCursorPosition(qMax(0, pos - 1));
        ui->WordEnter->blockSignals(false);

        showError("Сначала введите алфавит");
    } else if (!Alphabet.contains(text[text.size()-1])) { // if simbol is not allowed
        text.chop(1);

        ui->WordEnter->blockSignals(true);
        ui->WordEnter->setText(text);
        ui->WordEnter->setCursorPosition(qMax(0, pos - 1));
        ui->WordEnter->blockSignals(false);

        showError("Символа нет в алфавите");
    }
}

