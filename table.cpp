#include "table.h"
#include "ui_mainwindow.h"
Table::Table() : ui(new Ui::MainWindow)
{

QStandardItemModel *model=new QStandardItemModel(this);
model->setColumnCount(2);


}
