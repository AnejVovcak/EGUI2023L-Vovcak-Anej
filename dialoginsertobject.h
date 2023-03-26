#ifndef DIALOGINSERTOBJECT_H
#define DIALOGINSERTOBJECT_H

#include "qlineedit.h"
#include <QDialog>

namespace Ui {
class DialogInsertObject;
}

class DialogInsertObject : public QDialog
{
    Q_OBJECT

public:
    explicit DialogInsertObject(QWidget *parent = nullptr);
    ~DialogInsertObject();
    QString getKey();
    QString getValue();

private:
    Ui::DialogInsertObject *ui;
    QString key;
    QString value;
    QLineEdit* key_line;
    QLineEdit* value_line;
};

#endif // DIALOGINSERTOBJECT_H
