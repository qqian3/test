#include "parsedata.h"
//实时数据
ParseRealTimeData::ParseRealTimeData(QObject *parent) : QObject(parent)
{

}

void ParseRealTimeData::working(uint8_t read_buf[])
{
    qDebug()<<"解析返回的实时型数据的线程地址"<<QThread::currentThread();
    QString text;
    QStringList  resistanceValues;
    float decimal_value;

    /*解析数据总条数*/
    int total_num=1;

    if(read_buf[1]==0xAB&&read_buf[2]==0xCD)
    {
        qDebug()<<"received real time data";

        if(read_buf[5]==0x52)
        {
            uint16_t value=(read_buf[7]<<8)|read_buf[6];
            uint16_t flag=(read_buf[9]<<8)|read_buf[8];

            /* 解析FLAG的不同位*/
            //单位
            uint8_t unit = flag & 0xF;
            //数据显示位数
            uint8_t displayDigits = (flag >> 4) & 0xF;
            //阻值大小区间
            uint8_t resistanceRange=(flag >> 8) & 0xF;
            //修饰位
            uint8_t valueModifier = (flag >> 12) & 0xF;
            decimal_value=value;
            if(valueModifier==0x1)
            {
                decimal_value=value;
                decimal_value+=65536;
            }
            if(resistanceRange==0x8) decimal_value/=10000;
            else if(resistanceRange==0x4) decimal_value/=1000;
            else if(resistanceRange==0x2) decimal_value/=100;
            else if(resistanceRange==0x1) decimal_value/=10;

            if(displayDigits==0x1) text=QString::number(decimal_value,'f',1);
            else if(displayDigits==0x2) text=QString::number(decimal_value,'f',2);
            else if(displayDigits==0x4) text=QString::number(decimal_value,'f',3);

            if(unit==0x0) text+=tr("uΩ");
            else if (unit==0x1)  text+=tr("mΩ");
            else if (unit==0x2) text+=tr("Ω");
            else if (unit==0x4)
            {
                text+=tr("kΩ");
                if(decimal_value>304)
                    text="OL";
            }
            resistanceValues<<text;
        }
    }
}
//回读数据
ParseReadBackData::ParseReadBackData(QObject *parent)
{

}

void ParseReadBackData::working(uint8_t read_buf[])
{
     qDebug()<<"解析返回的回读型数据的线程地址"<<QThread::currentThread();
     QString text;
     QStringList  resistanceValues;
     float decimal_value;

     /*解析数据总条数*/
     int total_num=read_buf[7]*256+read_buf[6];
     qDebug()<<"数据总条数："<<total_num;

     if(read_buf[1]==0xAB&&read_buf[2]==0xCD)
     {
         qDebug()<<"received back data";
         if(read_buf[5]==0x53)
         {
             for (int i=0;i<total_num;++i)
             {
                 uint16_t value=(read_buf[9+4*i]<<8)|read_buf[8+4*i];
                 uint16_t flag=(read_buf[11+4*i]<<8)|read_buf[10+4*i];

                 /* 解析FLAG的不同位*/
                 //单位
                 uint8_t unit = flag & 0xF;
                 //数据显示位数
                 uint8_t displayDigits = (flag >> 4) & 0xF;
                 //阻值大小区间
                 uint8_t resistanceRange=(flag >> 8) & 0xF;
                 //修饰位
                 uint8_t valueModifier = (flag >> 12) & 0xF;
                 decimal_value=value;
                 if(valueModifier==0x1)
                 {
                     decimal_value=value;
                     decimal_value+=65536;
                 }

                 if(resistanceRange==0x8) decimal_value/=10000;
                 else if(resistanceRange==0x4) decimal_value/=1000;
                 else if(resistanceRange==0x2) decimal_value/=100;
                 else if(resistanceRange==0x1) decimal_value/=10;

                 if(displayDigits==0x1) text=QString::number(decimal_value,'f',1);
                 else if(displayDigits==0x2) text=QString::number(decimal_value,'f',2);
                 else if(displayDigits==0x4) text=QString::number(decimal_value,'f',3);

                 if(unit==0x0) text+=tr("uΩ");
                 else if (unit==0x1)  text+=tr("mΩ");
                 else if (unit==0x2) text+=tr("Ω");
                 else if (unit==0x4)
                 {
                     text+=tr("kΩ");
                     if(decimal_value>304)
                         text="OL";
                 }
                 resistanceValues<<text;
             }
         }
     }

}
//查询数据

ParseInquireData::ParseInquireData(QObject *parent)
{

}

void ParseInquireData::working(uint8_t read_buf[])
{
     qDebug()<<"解析返回的查询型数据的线程地址"<<QThread::currentThread();

}
