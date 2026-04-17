#include "alphabetenter.h"
#include "ui_alphabetenter.h"

AlphabetEnter::AlphabetEnter(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AlphabetEnter)
{
    ui->setupUi(this);

//connecters:
    connect(ui->ReadyBut, &QPushButton::clicked, this, &AlphabetEnter::SaveAndExit);
}

AlphabetEnter::~AlphabetEnter()
{
    delete ui;
}

//my slots:

void AlphabetEnter::SaveAndExit() {
    accept();
};

QString AlphabetEnter::MainGetter() {
    return ui->EnterMainAlpha->text();
};
QString AlphabetEnter::AddGetter() {
    return ui->EnterAddAlpha->text();
};
