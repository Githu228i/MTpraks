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

    ui->RulesTable->setStyleSheet(
        "QTableWidget { background-color: white; }"
        );

    ui->TapeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->TapeView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->TapeView->verticalHeader()->setVisible(false);

    headLabel = new QLabel(this);
    headLabel->setPixmap(QPixmap(":/arrow"));
    headLabel->setScaledContents(true);
    headLabel->setFixedSize(30, 30); // подгони под размер ячейки
    headLabel->hide();

    headLabel->raise(); // чтобы была поверх таблицы

    tapeAnim = new QPropertyAnimation(ui->TapeView, "pos", this);
    tapeAnim->setDuration(150);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::step);

    headAnim = new QPropertyAnimation(headLabel, "pos", this);
    headAnim->setDuration(200); // скорость (мс)

    QTimer::singleShot(0, this, [this]() {
        updateTapeView();
        updateHeadPosition();
        headLabel->show();
        setUiRunning(false);
        blockButtons(true);
    });


//connecters:
    connect(ui->AlphabetEnterBut, &QPushButton::clicked, this, &MainWindow::AlphabetEnterOpen);
    connect(ui->GetWordBut, &QPushButton::clicked, this, [this]() {
        WordChange(ui->WordEnter->text());
    });
    connect(ui->WordEnter, &QLineEdit::textChanged, this, &MainWindow::WordEnterCheck);
    connect(ui->addStateBut, &QPushButton::clicked, this, &MainWindow::addState);
    connect(ui->removeStateBut, &QPushButton::clicked, this, &MainWindow::removeState);
    connect(ui->RulesTable, &QTableWidget::cellChanged, this, &MainWindow::ruleChanged);
    connect(ui->startBut, &QPushButton::clicked, this, &MainWindow::StartMachineForOneStep);
    connect(ui->runBut, &QPushButton::clicked, this, &MainWindow::runMachine);
    connect(ui->stopBut, &QPushButton::clicked, this, [this]() {
        if (timer->isActive()) timer->stop();
    });
    connect(ui->stopBut2, &QPushButton::clicked, this, [this]() {
        stopMachine();
        head = 0;
        updateTapeView();
        updateHeadPosition();

    });
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
        WordReset();

        for (QChar c : MainText){
            Alphabet.insert(c);
        }

        for (QChar c : AddText){
            if(!Alphabet.contains(c)) AddAlphabet.insert(c);
        }

        if (Alphabet.isEmpty()) {
            blockButtons(true);
        } else {
            blockButtons(false);
        }
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

void MainWindow::WordChange(const QString &text) {
    Word = text;
    loadWordToTape();
    if(Word != "") {
        ui->CurWordIs->setText(text);
    } else {
        ui->CurWordIs->setText("-----------");
    }
}
void MainWindow::WordReset() {
    Word = "";
    loadWordToTape();
    ui->CurWordIs->setText("-----------");
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
    // loadWordToTape();
    // for (int i = -1; i < 5; i++){
    //     qDebug() << i << ":" << getSymbol(i);
    // }
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

    viewOffset = -6; // ← фиксируем старт как в ТЗ

    updateTapeView();
    updateHeadPosition();
    headLabel->show();
}

void MainWindow::step()
{
    if (!isRunning) return;
    QChar current = getSymbol(head);

    // ищем правило
    for (const Rule &r : Rules)
    {
        if (r.fromState == currentState && r.input == current)
        {
            // 1. запись
            if (r.hasWrite)
                setSymbol(head, r.write);

            // 2. движение
            if (r.hasDir)
            {
                if (r.dir == '>')
                    head++;
                else if (r.dir == '<')
                    head--;
            }

            // 3. смена состояния
            if (r.hasState)
                currentState = r.toState;

            qDebug() << "STEP:"
                     << "state q" << currentState
                     << "head" << head
                     << "symbol" << getSymbol(head);

            updateViewOffset();
            updateTapeView();
            highlightCurrent();
            updateHeadPosition();

            return; // правило найдено → выходим
        }
    }

    // если правило не найдено
    qDebug() << "NO RULE → STOP";
    stopMachine();
}

void MainWindow::stopMachine()
{
    isRunning = false;
    timer->stop();
    qDebug() << "MACHINE STOPPED";
    setUiRunning(false);
}

void MainWindow::StartMachineForOneStep() {
    startMachine();
    if (timer->isActive())
        timer->stop();
    step();
}
void MainWindow::runMachine()
{
    startMachine();

    if (timer->isActive())
        return;

    timer->start(300);
}

void MainWindow::startMachine(){
    if (!isRunning)
    {
        collectRules();

        if (!validateRules()){
            showError("В правилах присутствует несуществующее состояние! Ошибка запуска!");
        return;
        }

        if (Word == ""){
            showError("Пустое слово! Ошибка запуска!");
            return;
        }

        if (Rules.isEmpty()){
            showError("Не записано ни одного правила! Ошибка запуска!");
            return;
        }

        loadWordToTape();
        currentState = 0;
        isRunning = true;
        highlightCurrent();
        setUiRunning(true);
    }
}

void MainWindow::updateViewOffset()
{
    int oldOffset = viewOffset;

    int leftEdge = viewOffset;
    int rightEdge = viewOffset + 19;

    if (head > rightEdge)
        viewOffset = head - 19;
    else if (head < leftEdge)
        viewOffset = head;

    if (viewOffset != oldOffset)
    {
        animateTapeShift(viewOffset - oldOffset);
    }
}

void MainWindow::updateTapeView()
{
    ui->TapeView->clear();

    int size = 20;

    ui->TapeView->setRowCount(1);
    ui->TapeView->setColumnCount(size);

    QStringList headers;

    for (int i = 0; i < size; i++)
    {
        int pos = viewOffset + i;
        headers << QString::number(pos);

        QChar c = getSymbol(pos);
        QTableWidgetItem *item = new QTableWidgetItem(QString(c));
        ui->TapeView->setItem(0, i, item);
    }

    ui->TapeView->setHorizontalHeaderLabels(headers);
}

void MainWindow::highlightCurrent()
{
    int rows = ui->RulesTable->rowCount();
    int cols = ui->RulesTable->columnCount();

    // 1. Сброс всей подсветки
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            QTableWidgetItem *item = ui->RulesTable->item(i, j);
            if (item)
                item->setBackground(QBrush());
        }
    }

    // 2. Подсветка строки текущего состояния
    for (int j = 0; j < cols; j++)
    {
        QTableWidgetItem *item = ui->RulesTable->item(currentState, j);
        if (item)
            item->setBackground(Qt::yellow);
    }

    // 3. Подсветка активного правила
    QChar current = getSymbol(head);

    for (int j = 0; j < cols; j++)
    {
        QString header = ui->RulesTable->horizontalHeaderItem(j)->text();

        if (!header.isEmpty() && header[0] == current)
        {
            QTableWidgetItem *item = ui->RulesTable->item(currentState, j);
            if (item)
                item->setBackground(QColor(200, 150, 0)); // перекрывает серый

            break;
        }
    }
}

void MainWindow::updateHeadPosition()
{
    int col = head - viewOffset;

    if (col < 0 || col >= ui->TapeView->columnCount())
    {
        headLabel->hide();
        return;
    }

    QRect cellRect = ui->TapeView->visualRect(
        ui->TapeView->model()->index(0, col)
        );

    QPoint tablePos = ui->TapeView->viewport()->mapTo(this, cellRect.topLeft());

    int x = tablePos.x() + cellRect.width() / 2 - headLabel->width() / 2;
    int y = tablePos.y() + cellRect.height() + 2;

    QPoint newPos(x, y);

    // если первый раз — просто ставим
    if (!headLabel->isVisible())
    {
        headLabel->move(newPos);
        headLabel->show();
        return;
    }

    // анимация
    headAnim->stop();
    headAnim->setStartValue(headLabel->pos());
    headAnim->setEndValue(newPos);
    headAnim->start();
}

void MainWindow::animateTapeShift(int delta)
{
    int cellWidth = ui->TapeView->columnWidth(0);

    QPoint startPos = ui->TapeView->pos();
    QPoint endPos = startPos;

    // если лента сдвигается вправо → визуально двигаем влево
    endPos.setX(startPos.x() - delta * cellWidth);

    tapeAnim->stop();
    tapeAnim->setStartValue(startPos);
    tapeAnim->setEndValue(endPos);

    connect(tapeAnim, &QPropertyAnimation::finished, this, [this, startPos]() {
        ui->TapeView->move(startPos);   // возвращаем назад
        updateTapeView();               // перерисовываем с новым offset
        updateHeadPosition();           // синхронизируем стрелку
    });

    tapeAnim->start();
}

void MainWindow::setUiRunning(bool running)
{
    // Таблица правил
    ui->RulesTable->setEnabled(!running);

    // Ввод слова и алфавита
    ui->WordEnter->setEnabled(!running);
    ui->AlphabetEnterBut->setEnabled(!running);

    // Кнопки управления
    ui->addStateBut->setEnabled(!running);
    ui->removeStateBut->setEnabled(!running);

    // Кнопка стоп наоборот
    ui->stopBut->setEnabled(running);
    ui->stopBut2->setEnabled(running);
}

void MainWindow::blockButtons(bool running)
{
    // Кнопка стоп
    ui->runBut->setEnabled(!running);
    ui->startBut->setEnabled(!running);
    ui->stopBut->setEnabled(!running);
    ui->stopBut2->setEnabled(!running);
}

bool MainWindow::validateRules()
{
    int maxState = ui->RulesTable->rowCount();

    for (const Rule &r : Rules)
    {
        if (r.hasState)
        {
            if (r.toState < 0 || r.toState >= maxState)
            {
                showError("Переход в несуществующее состояние q" + QString::number(r.toState));
                return false;
            }
        }
    }

    return true;
}
