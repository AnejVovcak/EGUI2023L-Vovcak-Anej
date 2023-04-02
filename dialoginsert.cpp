#include "dialoginsert.h"
#include "ui_dialoginsert.h"

DialogInsert::DialogInsert(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogInsert)
{
    ui->setupUi(this);
    value_line = ui->value;
    key_line = ui->key;
    is_double = ui->double_value;
    is_string = ui->string_value;

}

DialogInsert::~DialogInsert()
{
    delete ui;
}

QString DialogInsert::getString(){
    return value_line->text();
}

double DialogInsert::getDouble(){
    QString value = value_line->text();
    return value.toDouble();
}

bool DialogInsert::isStringChecked(){
    return is_string->isChecked();
}

QString DialogInsert::getKey(){
    return key_line->text();
}
