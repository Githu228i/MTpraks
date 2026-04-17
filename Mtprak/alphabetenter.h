#ifndef ALPHABETENTER_H
#define ALPHABETENTER_H

#include <QDialog>

namespace Ui {
class AlphabetEnter;
}

class AlphabetEnter : public QDialog
{
    Q_OBJECT

public:
    explicit AlphabetEnter(QWidget *parent = nullptr);
    ~AlphabetEnter();

    QString MainGetter();
    QString AddGetter();

//my slots:
private slots:
    void SaveAndExit();


private:
    Ui::AlphabetEnter *ui;
};

#endif // ALPHABETENTER_H
