#include "mainwindow.h"
#include "./ui_mainwindow.h"

//Connect with other structures:
#include "alphabetenter.h"

//my includes:
#include <algorithm>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

//connecters:
    connect(ui->AlphabetEnterBut, &QPushButton::clicked, this, &MainWindow::AlphabetEnterOpen);
    connect(ui->GetWordBut, &QPushButton::clicked, this, [this]() {
        WordChange(ui->WordEnter->text());
    });
    connect(ui->WordEnter, &QLineEdit::textChanged, this, &MainWindow::WordEnterCheck);
    connect(ui->addStateBut, &QPushButton::clicked, this, &MainWindow::addState);
    connect(ui->removeStateBut, &QPushButton::clicked, this, &MainWindow::removeState);
    connect(ui->RulesTable, &QTableWidget::cellChanged, this, &MainWindow::ruleChanged);
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

        for (QChar c : MainText){
            Alphabet.insert(c);
        }

        for (QChar c : AddText){
            if(!Alphabet.contains(c)) AddAlphabet.insert(c);
        }

        //checks
        {QString text;

        for (QChar c : Alphabet)
            text += c;

        ui->check1->setText(text);
        text = "";

        for (QChar c : AddAlphabet)
            text += c;

        ui->check2->setText(text);}
        //checks end

        updateTable();
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

void MainWindow::WordChange(const QString &text){
    Word = text;
    ui->checkword->setText(text);
}

void MainWindow::updateTable() {
    ui->RulesTable->clear();

    QList<QChar> AlphaList = Alphabet.values();
    QList<QChar> AddAlphaList = AddAlphabet.values();
    std::sort(AlphaList.begin(), AlphaList.end());
    std::sort(AddAlphaList.begin(), AddAlphaList.end());

    QStringList headers;
    for (QChar c : AlphaList) headers << QString(c);
    headers << QString('^');
    for (QChar c : AddAlphaList) headers << QString(c);
    ui->RulesTable->setColumnCount(headers.size()); // задаём количество колонок
    ui->RulesTable->setHorizontalHeaderLabels(headers);

    ui->RulesTable->setRowCount(1);
    ui->RulesTable->setVerticalHeaderItem(0, new QTableWidgetItem("q0"));
}

void MainWindow::addState()
{
    if(Alphabet.isEmpty()) {
        showError("Сначала введите алфавит");
        return;
    }
    int row = ui->RulesTable->rowCount();
    ui->RulesTable->insertRow(row);
    ui->RulesTable->setVerticalHeaderItem(row, new QTableWidgetItem("q" + QString::number(row)));
}

void MainWindow::removeState() {
    int rows = ui->RulesTable->rowCount();
    if (rows <= 0) {
        showError("Удалять нечего");
        return;
    } else if (rows <= 1) {
        showError("Нельзя удалить начальное состояние q0");
        return;
    }
    ui->RulesTable->removeRow(rows - 1);
}

void MainWindow::ruleChanged(int row, int col)
{
    QTableWidgetItem *item = ui->RulesTable->item(row, col);
    if (!item) return;

    QString text = item->text().trimmed();

    if (text.isEmpty())
        return;

    QStringList parts = text.split(" ", Qt::SkipEmptyParts);

    QChar write;
    QChar dir;
    QString state;

    bool hasWrite = false;
    bool hasDir = false;
    bool hasState = false;

    for (const QString &part : parts)
    {
        // dir check
        if (part == ">" || part == "<")
        {
            if (hasDir)
            {
                showError("Нельзя вводить 2 направления");

                ui->RulesTable->blockSignals(true);
                item->setText("");
                ui->RulesTable->blockSignals(false);
                return;
            }

            dir = part[0];
            hasDir = true;
        }
        // simbol check
        else if (part.size() == 1)
        {
            if (hasWrite)
            {
                showError("Нельзя вводить 2 символа");

                ui->RulesTable->blockSignals(true);
                item->setText("");
                ui->RulesTable->blockSignals(false);
                return;
            }
            if (Alphabet.contains(part[0]) || AddAlphabet.contains(part[0]) || part[0] == '^') write = part[0];
            else {
                showError("Данного символа нет в алфавите");

                ui->RulesTable->blockSignals(true);
                item->setText("");
                ui->RulesTable->blockSignals(false);
                return;

            }
            hasWrite = true;
        }
        // state check
        else if (part.startsWith('q'))
        {
            if (hasState)
            {
                showError("Нельзя вводить 2 состояния");

                ui->RulesTable->blockSignals(true);
                item->setText("");
                ui->RulesTable->blockSignals(false);
                return;
            }

            state = part;
            hasState = true;
        }
        // trash check
        else
        {
            showError("Неверный формат");

            ui->RulesTable->blockSignals(true);
            item->setText("");
            ui->RulesTable->blockSignals(false);
            return;
        }
    }
}
