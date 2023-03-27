#ifndef DIALOGINSERTARRAY_H
#define DIALOGINSERTARRAY_H

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

private:
    Ui::DialogInsertArray *ui;
};

#endif // DIALOGINSERTARRAY_H
