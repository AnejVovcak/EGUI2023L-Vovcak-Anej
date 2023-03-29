#include "dialoginsertarray.h"
#include "ui_dialoginsertarray.h"

DialogInsertArray::DialogInsertArray(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogInsertArray)
{
    ui->setupUi(this);
    value_line = ui->value;
    is_double_pressed=false;
    is_string_pressed=false;
    is_double = ui->double_value;
    is_string = ui->string_value;
}

DialogInsertArray::~DialogInsertArray()
{
    delete ui;
    delete is_double;
    delete is_string;

}

QString DialogInsertArray::getString(){
    return value_line->text();
}

double DialogInsertArray::getDouble(){
    QString value = value_line->text();
    return value.toDouble();
}

bool DialogInsertArray::isStringChecked(){
    qDebug()<<is_string->isChecked();
    qDebug()<<is_double->isChecked();
    return is_string->isChecked();
}


