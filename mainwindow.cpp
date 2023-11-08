#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("Micro Ohm Meter"));
    initView();

}
void MainWindow::initView()
{
    ui->messageEdit->setText(tr("Welcome!"));
    ui->connectEdit->setText(tr("Wait Connect..."));
    ui->numLabel->setNum(0);
    ui->label->setText(tr("Data total"));
    ui->realtimeBtn->setText(tr("实时测量"));
    ui->readbackBtn->setText(tr("回读数据"));
    ui->inquireBtn->setText(tr("查询数据存储数量"));

    model=new QStandardItemModel(this);
    model->setColumnCount(2);
    ui->tableView->setModel(model);

    model->setHeaderData(0, Qt::Horizontal, QString(tr("Number")),Qt::DisplayRole);
    model->setHeaderData(1, Qt::Horizontal, QString(tr("Resistances")),Qt::DisplayRole);

    ui->tabWidget->setTabText(0, QString(tr("Current")));
    ui->tabWidget->setTabText(1, QString(tr("Other")));
    ui->tabWidget->setCurrentWidget(ui->tab);

}
void MainWindow::on_connectAct_triggered()
{
    /*打开指定VID PID设备*/
    handle=hid_open(0x1A86,0xE429,NULL);
    if(handle)
    {
        ui->connectEdit->setText(tr("Connect"));
        ui->statusbar->showMessage(tr("Ready···"));
        /*设置为非阻塞*/
        hid_set_nonblocking(handle, 1);
        /*请求回读测量数据*/
    }
    else
    {
        ui->connectEdit->setText(tr("Unconnect"));
        QMessageBox::warning(this, tr("error"), tr("Can't open device"));
    }
}
/*send_buf[6]=0xXX---0x05为实时型数据测量上传、0x07为回读型测量数据上传、0x08为测量数据存储数量上传*/
void MainWindow::on_realtimeBtn_clicked()
{
    uint8_t send_buf[]={0x00,0x05,0xAB,0xCD,0x05,0x00,0x05};
    res=hid_write(handle,send_buf,7);
    res=hid_read_timeout(handle,read_buf,13,1000);
    if(res>0)
    {
        ui->statusbar->showMessage(tr("received real time data"));
       if(read_buf[1]==0xAB&&read_buf[2]==0xCD)
       {
           qDebug()<<"received real time data";
           if(read_buf[5]==0x51)
           {
               ui->statusbar->showMessage(tr("Device response data"));
               if(read_buf[6]==0x4f&&read_buf[7]==0x4b)
                   ui->messageEdit->setText((tr("OK:Success")));
               else if(read_buf[6]==0x45&&read_buf[7]==0x52)
                   ui->messageEdit->setText((tr("ER:Failure")));
               else if(read_buf[6]==0x4e&&read_buf[7]==0x4f)
                   ui->messageEdit->setText((tr("NO:Unknown Command")));
           }
           else if(read_buf[5]==0x52)
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
               if(valueModifier!=0x0)
               {
                   decimal_value=value;
               }
               else
               {
                   decimal_value += 65536;
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
               else if (unit==0x4) text+=tr("kΩ");
               else if (unit==0x8) text+=tr("MΩ");
              model->setItem(0,0,new QStandardItem(QString::number(1)));
              model->setItem(0,1,new QStandardItem(text));

           }
       }
    }
    else
    {
        QMessageBox::warning(this,tr("error"),tr("Transfer data failed!"));
    }
}

void MainWindow::on_readbackBtn_clicked()
{
    uint8_t send_buf[]={0x00,0x05,0xAB,0xCD,0x05,0x00,0x07};
    res=hid_write(handle,send_buf,7);
    res=hid_read_timeout(handle,read_buf,64,1000);

    /*解析数据总条数*/
    int total_num=read_buf[7]*256+read_buf[6];
    qDebug()<<"数据总条数："<<total_num;
    ui->numLabel->setNum(total_num);

    for (int i=1;i<static_cast<int>(total_num/14)+1;i++) {
        res=hid_read_timeout(handle,read_buf+64*i,64,1000);
    }

    if(res>0)
    {
        ui->statusbar->showMessage(tr("received back data"));
       if(read_buf[1]==0xAB&&read_buf[2]==0xCD)
       {
           qDebug()<<"received back data";
           if(read_buf[5]==0x51)
           {
               ui->statusbar->showMessage(tr("Device response data"));
               if(read_buf[6]==0x4f&&read_buf[7]==0x4b)
                   ui->messageEdit->setText((tr("OK:Success")));
               else if(read_buf[6]==0x45&&read_buf[7]==0x52)
                   ui->messageEdit->setText((tr("ER:Failure")));
               else if(read_buf[6]==0x4e&&read_buf[7]==0x4f)
                   ui->messageEdit->setText((tr("NO:Unknown Command")));
           }
           else if(read_buf[5]==0x52)
           {
               for (int i=0;i<total_num+1;i++)
               {
                   uint16_t value=(read_buf[7+4*i]<<8)|read_buf[6+4*i];
                   uint16_t flag=(read_buf[9+4*i]<<8)|read_buf[8+4*i];

               /* 解析FLAG的不同位*/
               //单位
               uint8_t unit = flag & 0xF;
               //数据显示位数
               uint8_t displayDigits = (flag >> 4) & 0xF;
               //阻值大小区间
               uint8_t resistanceRange=(flag >> 8) & 0xF;
               //修饰位
               uint8_t valueModifier = (flag >> 12) & 0xF;
               if(valueModifier!=0x0) decimal_value=value;
               else decimal_value += 65536;

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
               else if (unit==0x4) text+=tr("kΩ");
               else if (unit==0x8) text+=tr("MΩ");
               }
           }
       }
    }
    else
    {
        QMessageBox::warning(this,tr("error"),tr("Transfer data failed!"));
    }


    /*将序号和电阻值放入表格中*/
    model->setRowCount(total_num);
    QList<QStandardItem *> resistanceValues;
    static int itemCount = 1; // 计数器变量
    resistanceValues<<new QStandardItem(QString::number(itemCount));
    resistanceValues<<new QStandardItem(text);
    for (int i=0;i<resistanceValues.size();i++) {
         //wprintf(L"data:%x %x %x %x %x\n",read_buf[0+5*i],read_buf[1+5*i],read_buf[2+5*i],read_buf[3+5*i],read_buf[4+5*i]);
         /*遍历item设置文本居中*/
         resistanceValues.at(i)->setTextAlignment(Qt::AlignCenter);
//        model->setItem(i,0,new QStandardItem(QString::number(i+1)));
//        model->setItem(i,1,new QStandardItem(resistanceValues.at(i)));
    }
    model->appendRow(resistanceValues);
    itemCount++;
}

void MainWindow::on_inquireBtn_clicked()
{
    uint8_t send_buf[]={0x00,0x05,0xAB,0xCD,0x05,0x00,0x08};
    res=hid_write(handle,send_buf,7);
    res=hid_read_timeout(handle,read_buf,12,1000);

    if(res>0)
    {
        ui->statusbar->showMessage(tr("received inquire data"));
       if(read_buf[1]==0xAB&&read_buf[2]==0xCD)
       {
           qDebug()<<"received inquire data";
           if(read_buf[5]==0x51)
           {
               ui->statusbar->showMessage(tr("Device response data"));
               if(read_buf[6]==0x4f&&read_buf[7]==0x4b)
                   ui->messageEdit->setText((tr("OK:Success")));
               else if(read_buf[6]==0x45&&read_buf[7]==0x52)
                   ui->messageEdit->setText((tr("ER:Failure")));
               else if(read_buf[6]==0x4e&&read_buf[7]==0x4f)
                   ui->messageEdit->setText((tr("NO:Unknown Command")));
           }
           else if(read_buf[5]==0x54)
           {
               if(read_buf[6]==0x8)
               {
                   /*解析数据总条数*/
                   int total_num=read_buf[8]*256+read_buf[7];
                   qDebug()<<"数据总条数："<<total_num;
                   ui->label->setText(tr("Data Total"));
                   ui->numLabel->setNum(total_num);

               }
               else if(read_buf[6]==0x20)
               {
                   int powerNum=read_buf[8]*256+read_buf[7];
                   ui->label->setText(tr("Power Num"));
                   ui->numLabel->setNum(powerNum);
               }
           }
       }
    }
}


void MainWindow::on_saveAct_triggered()
{
    if (model==NULL)
    {
        qDebug() << "Invalid model";
        QMessageBox::information(this,tr("note"),tr("No data!"));
        return;
    }
    else
    {
        // 创建一个Excel应用对象
        QAxObject excel("Excel.Application");
        // 启动Excel应用
        excel.dynamicCall("SetVisible(bool)", false);
        // 创建一个工作簿对象
        QAxObject* workbooks = excel.querySubObject("Workbooks");
        QAxObject* workbook = workbooks->querySubObject("Add");
        // 获取工作表对象
        QAxObject* worksheets = workbook->querySubObject("Worksheets");
        QAxObject* worksheet = worksheets->querySubObject("Item(int)", 1);

        // 获取tableView的行数和列数
        int rowCount = model->rowCount();
        int columnCount = model->columnCount();

        // 遍历tableView的每个单元格，将数据写入Excel文件
        for (int row = 0; row < rowCount; ++row)
        {
            for (int column = 0; column < columnCount; ++column)
            {
                // 获取tableView中指定单元格的数据
                QModelIndex index = model->index(row, column);
                QString data = model->data(index).toString();

                // 获取Excel中指定单元格的对象
                QAxObject* cell = worksheet->querySubObject("Cells(int,int)", row + 1, column + 1);
                // 设置单元格的值
                cell->dynamicCall("SetValue(const QVariant&)", data);
                // 释放单元格对象
                cell->dynamicCall("Release()");
            }
        }

        // 保存工作簿
        workbook->dynamicCall("SaveAs(const QString&)", QFileDialog::getSaveFileName(nullptr, "Save Excel File", "/", "Excel Files (*.xlsx)"));
        // 关闭工作簿
        workbook->dynamicCall("Close()");
        // 退出Excel应用
        excel.dynamicCall("Quit()");
    }
}
void MainWindow::on_refreshAct_triggered()
{
    initView();
}

void MainWindow::on_closeAct_triggered()
{
    hid_close(handle);
    handle=nullptr;
    initView();
}


MainWindow::~MainWindow()
{
    delete ui;
}


