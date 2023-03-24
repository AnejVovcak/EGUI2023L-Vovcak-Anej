#ifndef DIALOGINSERTOBJECT_H
#define DIALOGINSERTOBJECT_H

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

private:
    Ui::DialogInsertObject *ui;
};

#endif // DIALOGINSERTOBJECT_H
