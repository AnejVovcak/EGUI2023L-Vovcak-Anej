#include "dialogmodify.h"
#include "qjsonobject.h"
#include "ui_dialogmodify.h"

DialogModify::DialogModify(QWidget *parent,  QStandardItemModel *model, QStandardItem* selectedItem) :
    QDialog(parent),
    ui(new Ui::DialogModify)
{

        ui->setupUi(this);

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
