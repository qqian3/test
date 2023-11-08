#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "hidapi.h"
#include <QToolButton>
#include <QMenuBar>
#include <QLineEdit>
#include <QLabel>
#include <QTextEdit>
#include <QAction>
#include <QString>
#include <QFileDialog>
#include <QMessageBox>
#include <QByteArray>
#include "table.h"
#include <QStandardItemModel>
#include <QAxObject>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_connectAct_triggered();
    void on_saveAct_triggered();
    void on_refreshAct_triggered();
    void on_closeAct_triggered();

    void on_readbackBtn_clicked();
    void on_inquireBtn_clicked();
    void on_realtimeBtn_clicked();

private:
    Ui::MainWindow *ui;
    QStandardItemModel *model;
    hid_device *handle;//usb句柄
    int res;

    QString text;
    float decimal_value;

    uint8_t read_buf[65];

    void initView();




};
#endif // MAINWINDOW_H
