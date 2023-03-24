#include "dialoginsert.h"
#include "ui_dialoginsert.h"

DialogInsert::DialogInsert(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogInsert)
{
    ui->setupUi(this);
}

DialogInsert::~DialogInsert()
{
    delete ui;
}
