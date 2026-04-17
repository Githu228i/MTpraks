#include "mainwindow.h"
#include "./ui_mainwindow.h"

//Connect with other structures:
#include "alphabetenter.h"

//my includes:
#include <algorithm>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

//connecters:
    connect(ui->AlphabetEnterBut, &QPushButton::clicked, this, &MainWindow::AlphabetEnterOpen);
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::collectRules);
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

Rule MainWindow::parseRule(int row, int col, const QString &text) {
    Rule rule;

    // откуда
    rule.fromState = row;

    // символ берём из заголовка столбца
    QString header = ui->RulesTable->horizontalHeaderItem(col)->text();
    rule.input = header[0];

    QStringList parts = text.split(" ", Qt::SkipEmptyParts);

    for (const QString &part : parts)
    {
        // направление
        if (part == ">" || part == "<")
        {
            rule.dir = part[0];
            rule.hasDir = true;
        }
        // символ
        else if (part.size() == 1)
        {
            rule.write = part[0];
            rule.hasWrite = true;
        }
        // состояние
        else if (part.startsWith('q'))
        {
            QString num = part.mid(1); // убрали 'q'
            rule.toState = num.toInt();
            rule.hasState = true;
        }
    }
    return rule;
}

void MainWindow::collectRules() {
    Rules.clear();

    int rows = ui->RulesTable->rowCount();
    int cols = ui->RulesTable->columnCount();

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            QTableWidgetItem *item = ui->RulesTable->item(i, j);
            if (!item) continue;

            QString text = item->text().trimmed();
            if (text.isEmpty()) continue;

            Rules.push_back(parseRule(i, j, text));
        }
    }
    //check rules
    {
    //qDebug() << "=== RULES ===";
    // for (const Rule &r : Rules)
    // {
    //     qDebug() << "from q" << r.fromState
    //              << "input:" << r.input
    //              << "| write:" << (r.hasWrite ? QString(r.write) : "-")
    //              << "| dir:" << (r.hasDir ? QString(r.dir) : "-")
    //              << "| to q" << (r.hasState ? QString::number(r.toState) : "-");
    // }
    }
    //check tape
    {
    // tape.clear();
    // head = 0;
    // setSymbol(0, 'a');
    // setSymbol(1, 'b');
    // setSymbol(-1, 'c');
    // qDebug() << getSymbol(-1) << getSymbol(0) << getSymbol(1) << getSymbol(2) << getSymbol(472387);
    }
    //check tapeloading
    {
    loadWordToTape();
    for (int i = -1; i < 5; i++){
        qDebug() << i << ":" << getSymbol(i);
    }
    }
}

QChar MainWindow::getSymbol(int pos) {
    if (tape.contains(pos))
        return tape[pos];
    return '^';
}

void MainWindow::setSymbol(int pos, QChar c)
{
    if (c == '^')
        tape.remove(pos);
    else
        tape[pos] = c;
}

void MainWindow::loadWordToTape()
{
    tape.clear();

    for (int i = 0; i < Word.size(); i++) {
        setSymbol(i, Word[i]);
    }

    head = 0;
}
