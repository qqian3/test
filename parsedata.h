#ifndef PARSEDATA_H
#define PARSEDATA_H

#include <QObject>
#include <QThread>
#include <QDebug>
class ParseRealTimeData : public QObject
{
    Q_OBJECT
public:
    explicit ParseRealTimeData(QObject *parent = nullptr);
    //在子线程中执行的工作函数函数
    void working(uint8_t read_buf[]);


signals:
    //将得到的total_num传递给主线程，显示在界面
    void finish(QStringList resistanceValues,int num);

};
class ParseReadBackData : public QObject
{
    Q_OBJECT
public:
    explicit ParseReadBackData(QObject *parent = nullptr);
    //在子线程中执行的工作函数函数
    void working(uint8_t read_buf[]);

signals:
    //将得到的total_num传递给主线程，显示在界面
    void finish(QStringList resistanceValues,int num);

};
class ParseInquireData : public QObject
{
    Q_OBJECT
public:
    explicit ParseInquireData(QObject *parent = nullptr);
    //在子线程中执行的工作函数函数
    void working(uint8_t read_buf[]);

signals:
    //将得到的total_num传递给主线程，显示在界面
    void finish(int num);

};

#endif // PARSEDATA_H
