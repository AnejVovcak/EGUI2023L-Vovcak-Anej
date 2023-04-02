#include "dialoginsertobject.h"
#include "ui_dialoginsertobject.h"

DialogInsertObject::DialogInsertObject(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogInsertObject)
{
    ui->setupUi(this);
    key_line = ui->key;
    value_line = ui->value;

}

DialogInsertObject::~DialogInsertObject()
{
    delete ui;

}

QString DialogInsertObject::getKey(){
    qDebug()<<key_line->text();
    return key_line->text();
}

QString DialogInsertObject::getValue(){
    return value_line->text();
}




