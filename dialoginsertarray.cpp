#include "dialoginsertarray.h"
#include "ui_dialoginsertarray.h"

DialogInsertArray::DialogInsertArray(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogInsertArray)
{
    ui->setupUi(this);
    value_line = ui->value;
}

DialogInsertArray::~DialogInsertArray()
{
    delete ui;
}

QString DialogInsertArray::getValue(){
    return value_line->text();
}
