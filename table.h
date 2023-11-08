#ifndef TABLE_H
#define TABLE_H

#include <QStandardItemModel>
#include <QTableView>
#include <QApplication>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class Table : public QStandardItemModel
{
public:
    Table();
private:
    Ui::MainWindow *ui;
};

#endif // TABLE_H
