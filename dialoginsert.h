#ifndef DIALOGINSERT_H
#define DIALOGINSERT_H

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

private:
    Ui::DialogInsert *ui;
};

#endif // DIALOGINSERT_H
