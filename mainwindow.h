#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QJsonDocument>
#include <QStandardItemModel>
#include <QJsonObject>
#include <QMainWindow>

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

    void on_actionactionNewNode_triggered();

private:
    void buildTree(QStandardItemModel *model, const QJsonObject &jsonObj, const QModelIndex &parentIndex = QModelIndex());
    void buildTree2(QStandardItemModel *model, const QJsonObject &jsonObj, const QModelIndex &parentIndex = QModelIndex());
    void convertToModel();
    QJsonObject buildObjectFromModel(QStandardItem *item);
    QJsonArray buildArrayFromModel(QStandardItem *item);

private:
    Ui::MainWindow *ui;
    QString file_path_;
};
#endif // MAINWINDOW_H
