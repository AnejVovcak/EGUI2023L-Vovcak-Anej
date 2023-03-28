#ifndef DIALOGINSERTARRAY_H
#define DIALOGINSERTARRAY_H

#include "qlineedit.h"
#include <QDialog>

namespace Ui {
class DialogInsertArray;
}

class DialogInsertArray : public QDialog
{
    Q_OBJECT

public:
    explicit DialogInsertArray(QWidget *parent = nullptr);
    ~DialogInsertArray();
    QString getValue();

private:
    Ui::DialogInsertArray *ui;
    QLineEdit* value_line;
};

#endif // DIALOGINSERTARRAY_H
