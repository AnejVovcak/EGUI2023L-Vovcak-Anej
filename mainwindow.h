#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "dialoginsertarray.h"
#include "dialoginsertobject.h"
#include "dialogmodify.h"
#include <QJsonDocument>
#include <QStandardItemModel>
#include <QJsonObject>
#include <QMainWindow>
#include <QTreeView>
#include <QAction>
#include <ui_dialoginsertobject.h>
#include <ui_dialogmodify.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionOpen_triggered();

    void on_actionSave_triggered();

    void on_actionSave_as_triggered();

    void on_actionNew_triggered();

    void on_treeView_pressed(const QModelIndex &index);

    void on_actiondeleteNode_triggered();

    void on_insert_object_element_triggered();

    void on_actionModifyElement_triggered();

    void on_actionNewArrayElement_triggered();

    void on_actionInsertValueElement_triggered();

private:
    void buildTree(QStandardItemModel *model, const QJsonObject &jsonObj, const QModelIndex &parentIndex = QModelIndex());
    void buildTree2(QStandardItemModel *model, const QJsonObject &jsonObj, const QModelIndex &parentIndex = QModelIndex());
    QJsonObject modelToJson(QStandardItem *item);
    void saveTree(QJsonObject& jsonObj, QStandardItem* item);
    QJsonObject saveTree(QStandardItem* item);
    void saveTree2(QJsonArray& jsonArr, QStandardItem* item);

private:
    Ui::MainWindow *ui;
    QString file_path_;
    QTreeView* treeView;
    QAction* action_new_array_element;
    QAction* action_new_object_element;
    QAction* action_save;
    QAction* action_save_as;
    QAction* action_open;
    QAction* action_delete_element;
    QAction* action_modify;
    QAction* action_new_value_element;
    QToolBar* toolbar;
    QStandardItemModel * model;
    DialogInsertObject *dialogInsertObject;
    DialogModify *dialogModify;
    DialogInsertArray *dialogInsertArray;

};
#endif // MAINWINDOW_H
