#include "dialoginsertarray.h"
#include "ui_dialoginsertarray.h"

DialogInsertArray::DialogInsertArray(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogInsertArray)
{
    ui->setupUi(this);
}

DialogInsertArray::~DialogInsertArray()
{
    delete ui;
}
