#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QGridLayout>
#include <QProcess>
#include <QMessageBox>
#include <QFile>
#include <QDebug>
#include <QRegularExpression>
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

    setFixedSize(1200, 900);
    // 设置样式表
    QString styleSheet = "QWidget {"
                         "    background-color: #F3F3F3;" // 设置窗口背景颜色为浅色
                         "    color: #000000;" // 设置窗口字体颜色为黑色
                         "}"
                         "QLineEdit {"
                         "    border: 2px solid #CCCCCC;"
                         "    border-radius: 10px;"
                         "    padding: 0 8px;"
                         "    background: #FFFFFF;" // 设置输入框背景颜色为白色
                         "    selection-background-color: #D3D3D3;"
                         "    font-size: 25px;" // 增加字体大小
                         "    min-height: 3em;" // 增加输入框高度
                         "    color: #000000;" // 设置输入框字体颜色为黑色
                         "    font-weight: bold;" // 设置输入框字体加粗
                         "}"
                         "QPushButton {"
                         "    background-color: #E0E0E0;" // 设置按钮背景颜色为浅色
                         "    border-style: outset;"
                         "    border-width: 2px;"
                         "    border-radius: 10px;"
                         "    border-color: #CCCCCC;"
                         "    font: bold 18px;" // 增加字体大小
                         "    min-width: 6em;" // 增加按钮宽度
                         "    min-height: 3em;" // 增加按钮高度
                         "    padding: 8px;" // 增加内边距
                         "    color: #000000;" // 设置按钮字体颜色为黑色
                         "    font-weight: bold;" // 设置按钮字体加粗
                         "}"
                         "QTextEdit {"
                         "    background-color: #FFFFFF;" // 设置显示框背景颜色为白色
                         "    border: 1px solid #CCCCCC;"
                         "    border-radius: 10px;"
                         "    padding: 8px;"
                         "    font-size: 25px;" // 增加字体大小
                         "    color: #000000;" // 设置显示框字体颜色为黑色
                         "    font-weight: bold;" // 设置显示框字体加粗
                         "}";

    this->setStyleSheet(styleSheet);

    QVBoxLayout *layout = new QVBoxLayout;
    expr_entry = new QLineEdit(this);
    result_text_edit = new QTextEdit(this);
    latex_label = new QLabel(this); // 初始化 QLabel
    result_text_edit->setReadOnly(true);

    latex_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    // latex_label->setSizePolicy(result_text_edit->sizePolicy());
    // latex_label->setMinimumSize(result_text_edit->minimumSize());
    // latex_label->setMaximumSize(result_text_edit->maximumSize());

    QPushButton *calculateButton = new QPushButton("Calculate", this);
    connect(calculateButton, &QPushButton::clicked, this, &MainWindow::on_calculateButtonClicked);

    // 增加点击效果
    connect(calculateButton, &QPushButton::pressed, [=]() {
        calculateButton->setStyleSheet("background-color: lightgray;");
    });
    connect(calculateButton, &QPushButton::released, [=]() {
        calculateButton->setStyleSheet("");
    });

    QPushButton *numericCalculateButton = new QPushButton("Numeric Calculate", this); // 新增数值计算按钮
    connect(numericCalculateButton, &QPushButton::clicked, this, &MainWindow::on_numericCalculateButtonClicked);

    // 增加点击效果
    connect(numericCalculateButton, &QPushButton::pressed, [=]() {
        numericCalculateButton->setStyleSheet("background-color: lightgray;");
    });
    connect(numericCalculateButton, &QPushButton::released, [=]() {
        numericCalculateButton->setStyleSheet("");
    });

    connect(expr_entry, &QLineEdit::returnPressed, this, &MainWindow::on_calculateButtonClicked);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    layout->addWidget(expr_entry);
    // layout->addWidget(result_text_edit);
    layout->addWidget(latex_label);
    layout->addWidget(createButtonGrid());
    buttonLayout->addWidget(calculateButton);
    buttonLayout->addWidget(numericCalculateButton); // 将数值计算按钮添加到水平布局中
    layout->addLayout(buttonLayout); // 将水平布局添加到主布局中

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);
    renderLatexToLabel(latex_label, "\LaTeX");
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
        connect(button, &QPushButton::pressed, [=]() {
            button->setStyleSheet("background-color: lightgray;");
        });
        connect(button, &QPushButton::released, [=]() {
            button->setStyleSheet("");
        });
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

    pos = 0;
    while ((pos = str.find('*', pos)) != std::string::npos) {
        if (pos < str.size() - 1 && isalpha(str[pos + 1])) {
            str.erase(pos, 1);
        } else {
            pos += 1;
        }
    }

    return str;
}

void MainWindow::renderLatexToLabel(QLabel *label, const QString &latex) {
    QFile::remove("latex.png");
    // 使用正则表达式将除法替换为分数形式
    QString processedLatex = latex;

    // 使用 matplotlib 渲染 LaTeX 字体
    int width = 1600;
    int height = 200;
    QProcess process;
    process.start("python3", QStringList() << "-c" <<
                  "import matplotlib.pyplot as plt; "
                  "fig, ax = plt.subplots(figsize=(" + QString::number(width / 100.0) + ", " + QString::number(height / 100.0) + ")); "
                  "ax.text(0.01, 0.5, r'" + processedLatex + "', fontsize=20, ha='left', va='center'); "
                  "ax.axis('off'); "
                  "plt.savefig('latex.png', bbox_inches='tight', pad_inches=0);");
    process.waitForFinished();

    // 检查退出代码
    int exitCode = process.exitCode();
    if (exitCode != 0) {
        qDebug() << "Python process failed with exit code:" << exitCode;
    }

    // 检查标准输出和标准错误
    QString stdout = process.readAllStandardOutput();
    QString stderr = process.readAllStandardError();
    qDebug() << "Standard Output:" << stdout;
    qDebug() << "Standard Error:" << stderr;

    // 加载生成的图像并显示在 QLabel 中
    QPixmap pixmap("latex.png");
    if (!pixmap.isNull()) {
        label->setPixmap(pixmap);
    } else {
        qDebug() << "Failed to load latex.png";
    }
}

void MainWindow::on_buttonClicked() {
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (button) {
        QString text = button->text();
        if (text == "C") {
            expr_entry->clear(); // 清除输入框内容
            expr_entry->setFocus(); // 设置焦点
            result_text_edit->clear();
            result_text_edit->setText(0);
            renderLatexToLabel(latex_label, "$0$");
        } else if(text == '=') {
            if(result_text_edit->toPlainText().isEmpty()) {
                on_calculateButtonClicked(); // 点击等号按钮时执行计算
            }
            QString result = result_text_edit->toPlainText(); // 获取计算结果
            if (!result.isEmpty()) { // 判断result是否为空
                expr_entry->setText(result);
                result_text_edit->clear(); // 清空显示框
            }
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

void MainWindow::on_calculateButtonClicked() {
    QString expression = expr_entry->text();
    try {
        SymEngine::Expression expr = SymEngine::parse(expression.toStdString());
        SymEngine::Expression result = expand(expr);
        std::string result_str = SymEngine::str(result);
        display_result(trans(result_str));
    } catch (const std::exception &e) {
        display_result(std::string("Error: ") + e.what());
    }
}

void MainWindow::on_numericCalculateButtonClicked() {
    QString expression = expr_entry->text();
    try {
        SymEngine::Expression expr = SymEngine::parse(expression.toStdString());
        double result = SymEngine::eval_double(expr);
        display_result(std::to_string(result));
    } catch (const std::exception &e) {
        display_result(std::string("Error: ") + e.what());
    }
}

void MainWindow::display_result(const std::string &result) {
    QString result_qstr = QString::fromStdString(result);
    result_text_edit->setPlainText(result_qstr);

    // 使用正则表达式将 ^ 后面的数字用 {} 括起来
    QString latex = QString::fromStdString(result);
    QRegularExpression expRegex(R"((\w+|\([^()]+\)|\{[^{}]+\})\^(\w+|\([^()]+\)|\{[^{}]+\}))");
    latex.replace(expRegex, "\\1^{\\2}");

    // 使用正则表达式将除法替换为分数形式
    QRegularExpression divisionRegex(R"((\w+(\^\{[^{}]+\})?|\([^()]+\)(\^\{[^{}]+\})?|\{[^{}]+\}(\^\{[^{}]+\})?)\s*/\s*(\w+(\^\{[^{}]+\})?|\([^()]+\)(\^\{[^{}]+\})?|\{[^{}]+\}(\^\{[^{}]+\})?))");
    latex.replace(divisionRegex, "\\frac{\\1}{\\5}");

    latex = "$" + latex + "$";

    std::cerr<<latex.toStdString()<<std::endl;
    renderLatexToLabel(latex_label, latex);
}