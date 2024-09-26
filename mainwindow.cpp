#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QGridLayout>
#include <QMessageBox>
#include <symengine/expression.h>
#include <symengine/parser.h>
#include <symengine/derivative.h>
#include <symengine/basic.h> // 包含这个头文件以使用 SymEngine::str 和 SymEngine::expand 函数
#include <symengine/eval_double.h> // 包含这个头文件以使用数值计算功能
#include <symengine/simplify.h> // 包含这个头文件以使用 SymEngine::simplify 函数
#include <symengine/functions.h>
#include <symengine/sets.h>
#include <symengine/symbol.h>
#include <symengine/printers/latex.h>


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    
    QString styleSheet = "QWidget {"
                         "    background-color: #1E1E1E;" // 设置窗口背景颜色为深色
                         "    color: #FFFFFF;" // 设置窗口字体颜色为白色
                         "}"
                         "QLineEdit {"
                         "    border: 2px solid #3C3C3C;"
                         "    border-radius: 10px;"
                         "    padding: 0 8px;"
                         "    background: #2D2D30;" // 设置输入框背景颜色为深色
                         "    selection-background-color: #3C3C3C;"
                         "    font-size: 35px;" // 增加字体大小
                         "    min-height: 3em;" // 增加输入框高度
                         "    color: #FFFFFF;" // 设置输入框字体颜色为白色
                         "    font-weight: bold;"
                         "}"
                         "QPushButton {"
                         "    background-color: #3C3C3C;" // 设置按钮背景颜色为深色
                         "    border-style: outset;"
                         "    border-width: 2px;"
                         "    border-radius: 10px;"
                         "    border-color: #555555;"
                         "    font: bold 22px;" // 增加字体大小
                         "    min-width: 6em;" // 增加按钮宽度
                         "    min-height: 3em;" // 增加按钮高度
                         "    padding: 8px;" // 增加内边距
                         "    color: #FFFFFF;" // 设置按钮字体颜色为白色
                         "}"
                         "QTextEdit {"
                         "    background-color: #2D2D30;" // 设置显示框背景颜色为深色
                         "    border: 1px solid #3C3C3C;"
                         "    border-radius: 10px;"
                         "    padding: 8px;"
                         "    font-size: 35px;" // 增加字体大小
                         "    color: #FFFFFF;" // 设置显示框字体颜色为白色
                         "    font-weight: bold;"
                         "}";
    this->setStyleSheet(styleSheet);

    QVBoxLayout *layout = new QVBoxLayout;

    expr_entry = new QLineEdit(this);
    result_text_edit = new QTextEdit(this);
    result_text_edit->setReadOnly(true);

    QPushButton *calculateButton = new QPushButton("Calculate", this);
    connect(calculateButton, &QPushButton::clicked, this, &MainWindow::on_calculateButtonClicked);

    QPushButton *numericCalculateButton = new QPushButton("Numeric Calculate", this); // 新增数值计算按钮
    connect(numericCalculateButton, &QPushButton::clicked, this, &MainWindow::on_numericCalculateButtonClicked);

    connect(expr_entry, &QLineEdit::returnPressed, this, &MainWindow::on_calculateButtonClicked);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    layout->addWidget(expr_entry);
    layout->addWidget(result_text_edit);
    layout->addWidget(createButtonGrid());
    buttonLayout->addWidget(calculateButton);
    buttonLayout->addWidget(numericCalculateButton); // 将数值计算按钮添加到水平布局中
    layout->addLayout(buttonLayout); // 将水平布局添加到主布局中
    // layout->addWidget(calculateButton);
    // layout->addWidget(numericCalculateButton); // 添加数值计算按钮到布局中
    
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);
}

MainWindow::~MainWindow() {
    delete ui;
}

QWidget* MainWindow::createButtonGrid() {
    QGridLayout *layout = new QGridLayout;
    QStringList buttons = {"7", "8", "9", "/", "C",
                           "4", "5", "6", "*", "(",
                           "1", "2", "3", "-", ")",
                           ".", "0", "^", "+", "=",
                           "x", "y", "z", "∫", "d/dx"};

    int pos = 0;
    for (const QString &text : buttons) {
        QPushButton *button = new QPushButton(text, this);
        int row = pos / 5;
        int col = pos % 5;
        layout->addWidget(button, row, col);
        connect(button, &QPushButton::clicked, this, &MainWindow::on_buttonClicked);
        pos++;
    }

    QWidget *widget = new QWidget(this);
    widget->setLayout(layout);
    return widget;
}

std::string MainWindow::trans(std::string str) {
    size_t pos = 0;
    while ((pos = str.find("**", pos)) != std::string::npos) {
        str.replace(pos, 2, "^");
        pos += 1; // 移动到下一个位置
    }
    return str;
}

void MainWindow::on_buttonClicked()
{
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (button) {
        QString text = button->text();
        if (text == "C") {
            expr_entry->clear(); // 清除输入框内容
            result_text_edit->clear();
        } else if(text == '=') {
            if(result_text_edit->toPlainText().isEmpty()) on_calculateButtonClicked(); // 点击等号按钮时执行计算
            QString result = result_text_edit->toPlainText(); // 获取计算结果
            expr_entry->setText(result);
            result_text_edit->clear(); // 清空显示框
        } else if(text == "d/dx") {
            QString expression = expr_entry->text();
            SymEngine::Expression expr = SymEngine::parse(expression.toStdString());
            SymEngine::RCP<const SymEngine::Symbol> x = SymEngine::symbol("x");
            auto deri = SymEngine::expand(SymEngine::diff(expr, x));
            display_result(trans(SymEngine::str(*deri))); // 直接传递 std::string
        } else {
            expr_entry->insert(text);
        }
    }
}

void MainWindow::on_calculateButtonClicked()
{
    QString expression = expr_entry->text();
    try {
        SymEngine::Expression expr = SymEngine::parse(expression.toStdString());
        SymEngine::Expression result=expand(expr);
        std::string result_str = SymEngine::str(result);
        display_result(trans(result_str));
    } catch (const std::exception &e) {
        display_result(std::string("Error: ") + e.what());
    }
}

void MainWindow::on_numericCalculateButtonClicked()
{
    QString expression = expr_entry->text();
    try {
        SymEngine::Expression expr = SymEngine::parse(expression.toStdString());
        double result = SymEngine::eval_double(*expr.get_basic()); // 执行数值计算
        display_result(std::to_string(result));
    } catch (const std::exception &e) {
        display_result(std::string("Error: ") + e.what());
    }
}

void MainWindow::display_result(const std::string &result)
{
    QString latex = QString::fromStdString(result);
    QString html = "<html><head><script type='text/javascript' "
                   "src='https://cdnjs.cloudflare.com/ajax/libs/mathjax/2.7.7/MathJax.js?config=TeX-MML-AM_CHTML'></script></head>"
                   "<body><p>" + latex + "</p></body></html>";
    result_text_edit->setHtml(html);

    // 强制刷新MathJax
    result_text_edit->setPlainText(result_text_edit->toPlainText());
}