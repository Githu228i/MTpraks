#ifndef RULE_H
#define RULE_H

#include <QMainWindow>

class Rule
{
public:
    Rule();
    int fromState;
    QChar input;

    bool hasWrite;
    QChar write;
    bool hasDir;
    QChar dir;
    bool hasState;
    int toState;
};
#endif // RULE_H
