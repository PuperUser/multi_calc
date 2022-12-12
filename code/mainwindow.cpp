#include "mainwindow.h"
#include "ui_mainwindow.h"

// Сохранение на android:
// https://forum.qt.io/topic/105406/how-to-save-files-in-android-in-downloads

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    centralWidget()->setLayout(ui->verticalLayout);
    ui->lbExpression->clear();
    resize(350, 500);

    connect(&Cur, &Currences::refreshFinished, [=]{multi_display(input);});
    Cur.load();

    QVector<QPair<QPushButton*, Qt::Key>> keyTable =
    {
        { ui->pb_0,     Qt::Key_0 },
        { ui->pb_1,     Qt::Key_1 },
        { ui->pb_2,     Qt::Key_2 },
        { ui->pb_3,     Qt::Key_3 },
        { ui->pb_4,     Qt::Key_4 },
        { ui->pb_5,     Qt::Key_5 },
        { ui->pb_6,     Qt::Key_6 },
        { ui->pb_7,     Qt::Key_7 },
        { ui->pb_8,     Qt::Key_8 },
        { ui->pb_9,     Qt::Key_9 },
        { ui->pb_dot,   Qt::Key_Period },
        { ui->pb_backspace, Qt::Key_Backspace },
        { ui->pb_c,     Qt::Key_Escape },
        { ui->pb_enter, Qt::Key_Enter },
        { ui->pb_plus,  Qt::Key_Plus },
        { ui->pb_minus, Qt::Key_Minus },
        { ui->pb_mul,   Qt::Key_Asterisk },
        { ui->pb_div,   Qt::Key_Slash },
    };
    for (auto& [pb, key]: keyTable)
    {
        Keys.push_back(new QShortcut(key, this));
        connect(Keys.back(), &QShortcut::activated, this, [=]{button_handler(key);});
        connect(pb, &QPushButton::clicked, [=]{button_handler(key);});
    }

    QVector<QPushButton*> pbs = {ui->pb_cur_1, ui->pb_cur_2, ui->pb_cur_3, ui->pb_cur_4};
    for (int i=0;i<4;++i) {
        connect(pbs[i], &QPushButton::clicked,
            [=]{
                input = QString::number(input.toDouble()*Cur.rate(i));
                Cur.setBase(i);
                multi_display(input);
                currentOperator = NONE;
            }
        );
    }

    //multi_display("0");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::digit_handler(const char c)
{
    if (input.size()<ui->lcdNumber->digitCount())
        input += c;
    multi_display(input);
}

void MainWindow::button_handler(const Qt::Key key)
{
    switch (key) {
    case Qt::Key_0:
        if ((input.size() == 0) || (input.size() == 1 && input[0] == '0'))
            break;
        Q_FALLTHROUGH();
    case Qt::Key_1: Q_FALLTHROUGH();
    case Qt::Key_2: Q_FALLTHROUGH();
    case Qt::Key_3: Q_FALLTHROUGH();
    case Qt::Key_4: Q_FALLTHROUGH();
    case Qt::Key_5: Q_FALLTHROUGH();
    case Qt::Key_6: Q_FALLTHROUGH();
    case Qt::Key_7: Q_FALLTHROUGH();
    case Qt::Key_8: Q_FALLTHROUGH();
    case Qt::Key_9:
        if (input.size() == 1 && input[0] == '0')
            input.clear();
        digit_handler('0'+(key-Qt::Key_0));
        break;
    case Qt::Key_Period:
        if (input.count('.'))
            break;
        if (input.size()==0)
            input += '0';
        digit_handler('.');
        break;
    case Qt::Key_Backspace:
        if (input.size() < 1)
            break;
        input.chop(1);
        multi_display(input);
        break;
    case Qt::Key_Escape:
        input.clear();
        multi_display(input);
        currentOperator = NONE;
        break;
    case Qt::Key_Enter:
        calc();
        break;
    case Qt::Key_Plus:
        operator_handler(PLUS);
        break;
    case Qt::Key_Minus:
        operator_handler(MINUS);
        break;
    case Qt::Key_Asterisk:
        operator_handler(MUL);
        break;
    case Qt::Key_Slash:
        operator_handler(DIV);
        break;
    default:
        break;
    }

}

void MainWindow::operator_handler(const Operator o)
{
    if (calc())
        leftNumber = result;
    else
        leftNumber = input;
    currentOperator = o;
    input.clear();
    ui->lbExpression->setText(leftNumber+OperatorSymbol[int(o)]);
}

bool MainWindow::calc()
{
    if (currentOperator==NONE || input.size()==0 || leftNumber.size()==0)
        return false;
    double left = leftNumber.toDouble();
    double right = input.toDouble();
    double res=0;

    switch (currentOperator) {
    case PLUS:
        res = left + right;
        break;
    case MINUS:
        res = left - right;
        break;
    case MUL:
        res = left * right;
        break;
    case DIV:
        if (right!=0)
           res = left / right;
        break;
    case NONE:
    default:
        break;
    }

    result = QString::number(res);
    multi_display(result);
    ui->lbExpression->setText(leftNumber+OperatorSymbol[int(currentOperator)]+input+"="+result);

    return true;
}

void MainWindow::multi_display(const QString v)
{
    ui->lcdNumber->display(v);
    double d = v.size()==0? 1: v.toDouble();
    ui->pb_cur_1->setText(QString::number(d*Cur.rate(0)) + " " + Cur.name(0));
    ui->pb_cur_2->setText(QString::number(d*Cur.rate(1)) + " " + Cur.name(1));
    ui->pb_cur_3->setText(QString::number(d*Cur.rate(2)) + " " + Cur.name(2));
    ui->pb_cur_4->setText(QString::number(d*Cur.rate(3)) + " " + Cur.name(3));
}

void MainWindow::on_pbRefresh_clicked()
{
    Cur.load();
}
