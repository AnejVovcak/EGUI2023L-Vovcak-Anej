#ifndef DIALOGINSERT_H
#define DIALOGINSERT_H

#include "qlineedit.h"
#include "qradiobutton.h"
#include <QDialog>

namespace Ui {
class DialogInsert;
}

class DialogInsert : public QDialog
{
    Q_OBJECT

public:
    explicit DialogInsert(QWidget *parent = nullptr);
    ~DialogInsert();
    QString getString();
    double getDouble();
    bool isStringChecked();
    QString getKey();

private:
    Ui::DialogInsert *ui;
    QLineEdit* value_line;
    QLineEdit* key_line;
    QRadioButton *is_string;
    QRadioButton *is_double;
};

#endif // DIALOGINSERT_H
