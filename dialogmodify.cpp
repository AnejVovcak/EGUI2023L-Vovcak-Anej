#include "dialogmodify.h"
#include "qjsonobject.h"
#include "ui_dialogmodify.h"
#include "mainwindow.h"

DialogModify::DialogModify(QWidget *parent,  QJsonObject *object) :
    QDialog(parent),
    ui(new Ui::DialogModify)
{
    m_mainWindow = dynamic_cast<MainWindow*>(parent);


        ui->setupUi(this);

        QStandardItemModel *model = new QStandardItemModel();
        QJsonObject old = *new QJsonObject(*object);
        m_mainWindow->buildTree(model, old);
        model->setHeaderData(0, Qt::Horizontal, QObject::tr("Key"));
        model->setHeaderData(1, Qt::Horizontal, QObject::tr("Value"));

        ui->treeView->setModel(model);

        // Expand the tree view to show the root item
        ui->treeView->expand(model->index(0, 0, QModelIndex()));

}

DialogModify::~DialogModify()
{
    delete ui;
}

QStandardItemModel* DialogModify::getNewModal(){
    return dynamic_cast<QStandardItemModel*>(ui->treeView->model());
}
