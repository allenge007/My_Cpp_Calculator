#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <symengine/expression.h>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QWidget>
#include <QTextEdit>
#include <cstring>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    std::string trans(std ::string);
    void renderLatexToLabel(QLabel *label, const QString &latex);
    QString

private slots:
    void on_buttonClicked();
    void on_calculateButtonClicked();
    void on_numericCalculateButtonClicked(); // 恢复数值计算按钮的槽函数声明

private:
    Ui::MainWindow *ui;
    SymEngine::Expression expr;
    QLineEdit *expr_entry;
    QTextEdit *result_text_edit;
    QWidget *createButtonGrid();
    QLabel *latex_label; // 添加 QLabel 成员变量
    void display_result(const std::string &result);
};

#endif // MAINWINDOW_H