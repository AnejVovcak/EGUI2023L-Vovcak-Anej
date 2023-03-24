#include "dialoginsertobject.h"
#include "ui_dialoginsertobject.h"

DialogInsertObject::DialogInsertObject(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogInsertObject)
{
    ui->setupUi(this);
}

DialogInsertObject::~DialogInsertObject()
{
    delete ui;
}
