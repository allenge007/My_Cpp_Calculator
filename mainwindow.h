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
#include <QKeyEvent>
#include <QMap>
#include <QSet>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class CustomLineEdit : public QLineEdit {
    Q_OBJECT

public:
    CustomLineEdit(QWidget *parent = nullptr) : QLineEdit(parent) {}

    void setButtonMap(QMap<QString, QPushButton*> *map) {
        buttonMap = map;
    }
    QMap<QString, QPushButton*> *buttonMap = nullptr;
    QSet<QString> pressedKeys;

protected:
    void keyPressEvent(QKeyEvent *event) override {
        QString keyText = event->text();
        if (!keyText.isEmpty() && buttonMap && buttonMap->contains(keyText)) {
            QPushButton *button = buttonMap->value(keyText);
            if (!button->isDown()) { // 仅在按钮未按下时改变样式
                button->setStyleSheet("background-color: lightgray;");
                button->setDown(true); // 设置按钮为按下状态
                pressedKeys.insert(keyText); // 记录按下的键
            }
        }
        QLineEdit::keyPressEvent(event);
    }

    void keyReleaseEvent(QKeyEvent *event) override {
        QString keyText = event->text();
        if (!keyText.isEmpty() && buttonMap && buttonMap->contains(keyText)) {
            QPushButton *button = buttonMap->value(keyText);
            button->setStyleSheet("");
            button->setDown(false); // 恢复按钮为未按下状态
            pressedKeys.remove(keyText); // 移除记录的按下键
        }

        // 检查是否松开了修饰键
        if (!(event->modifiers() & (Qt::ShiftModifier | Qt::ControlModifier | Qt::AltModifier | Qt::MetaModifier))) {
            // 松开了所有修饰键，将所有按下的按钮复原
            for (const QString &key : pressedKeys) {
                if (buttonMap && buttonMap->contains(key)) {
                    QPushButton *button = buttonMap->value(key);
                    button->setStyleSheet("");
                    button->setDown(false); // 恢复按钮为未按下状态
                }
            }
            pressedKeys.clear(); // 清空记录的按下键
        }
        QLineEdit::keyReleaseEvent(event);
    }
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_buttonClicked();
    void on_calculateButtonClicked();
    void on_numericCalculateButtonClicked(); // 恢复数值计算按钮的槽函数声明

private:
    QMap<QString, QPushButton*> buttonMap;
    QTimer *resizeTimer;
    Ui::MainWindow *ui;
    SymEngine::Expression expr;
    CustomLineEdit *expr_entry;
    QTextEdit *result_text_edit;
    QWidget *createButtonGrid();
    QLabel *latex_label; // 添加 QLabel 成员变量
    void display_result(const std::string &result);
    std::string trans(std ::string);
    std::string trans_latex(std::string);
    std::string trans_inv(std::string);
    void Integrate();
    void renderLatexToLabel(QLabel *label, const QString &latex);
    void resizeEvent(QResizeEvent *event) override;
    void onResizeTimeout();
};
#endif // MAINWINDOW_H