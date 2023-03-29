#ifndef DIALOGINSERTARRAY_H
#define DIALOGINSERTARRAY_H

#include "qlineedit.h"
#include "qradiobutton.h"
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
    QString getString();
    double getDouble();
    bool isStringChecked();

private:
    Ui::DialogInsertArray *ui;
    QLineEdit* value_line;
    QRadioButton *is_string;
    QRadioButton *is_double;
    bool is_string_pressed;
    bool is_double_pressed;

};

#endif // DIALOGINSERTARRAY_H
