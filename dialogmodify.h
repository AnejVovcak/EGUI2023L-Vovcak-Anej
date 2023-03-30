#ifndef DIALOGMODIFY_H
#define DIALOGMODIFY_H

#include "qstandarditemmodel.h"
#include <QDialog>

class MainWindow;

namespace Ui {
class DialogModify;
}

class DialogModify : public QDialog
{
    Q_OBJECT

public:
    explicit DialogModify(QWidget *parent = nullptr, QJsonObject *object=nullptr);
    ~DialogModify();

private:
    Ui::DialogModify *ui;

public:
    QStandardItemModel* getNewModal();
    MainWindow* m_mainWindow;
};

#endif // DIALOGMODIFY_H
