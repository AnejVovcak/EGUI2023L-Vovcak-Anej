#ifndef DIALOGMODIFY_H
#define DIALOGMODIFY_H

#include "qabstractitemmodel.h"
#include "qstandarditemmodel.h"
#include <QDialog>

namespace Ui {
class DialogModify;
}

class DialogModify : public QDialog
{
    Q_OBJECT

public:
    explicit DialogModify(QWidget *parent = nullptr, QStandardItemModel* model=nullptr, QStandardItem* selectedItem=nullptr);
    ~DialogModify();

private:
    Ui::DialogModify *ui;

public:
    QStandardItemModel* getNewModal();
};

#endif // DIALOGMODIFY_H
