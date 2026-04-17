#ifndef RULE_H
#define RULE_H

#include <QMainWindow>

class Rule
{
public:
    Rule();

    // откуда
    int fromState;   // строка (q0 → 0)
    QChar input;     // символ (столбец)

    // что делаем
    bool hasWrite;
    QChar write;

    bool hasDir;
    QChar dir;       // '>' или '<'

    bool hasState;
    int toState;     // q1 → 1
};

#endif // RULE_H
