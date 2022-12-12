#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QShortcut>
#include "currences.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pbRefresh_clicked();

private:
    Ui::MainWindow  *ui;
    Currences       Cur;
    QString         input;
    QString         leftNumber;
    QString         result;
    const int       InputLimit = 10;

    QVector<QShortcut*> Keys;

    enum Operator { NONE = 0, PLUS, MINUS, MUL, DIV, OPERATORS_COUNT };
    char OperatorSymbol[OPERATORS_COUNT] = { ' ', '+', '-', '*', '/'};
    Operator currentOperator = NONE;

private:
    void digit_handler(const char c);
    void button_handler(const Qt::Key key);
    void operator_handler(const Operator o);
    bool calc();
    void multi_display(const QString v);
};

#endif // MAINWINDOW_H
