#include "mainwindow.h"
#include "qjsonarray.h"
#include "qtoolbar.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QFontDialog>
#include <QFont>
#include <QColorDialog>
#include <QColor>
#include <QJsonDocument>
#include <QStandardItemModel>
#include <QJsonObject>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    action_new_array_element = ui->actionNewArrayElement;
    action_new_object_element = ui->actionNewObject;
    action_save = ui->actionSave;
    action_save_as = ui->actionSave_as;
    action_delete_element = ui->actiondeleteNode;

    toolbar = ui->toolBar;
    action_new_array_element->setEnabled(false);
    action_new_object_element->setEnabled(false);
    //action_save->setEnabled(false);
    //action_save_as->setEnabled(false);
    //action_delete_element->setEnabled(false);
}

MainWindow::~MainWindow(){
    delete treeView;
    delete action_new_array_element;
    delete action_new_array_element;
    delete action_save;
    delete action_save_as;
    delete action_open;
    delete toolbar;
};

void MainWindow::on_actionOpen_triggered()
{
    QString file_name = QFileDialog::getOpenFileName(this, "Open the file", "", "JSON files (*.json)");

    if (file_name.isNull())
        return;

    QFile file(file_name);
    file_path_ = file_name;

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, "Error", "Failed to open file");
        return;
    }

    // Check if the file contains valid JSON
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);
    if (doc.isNull()) {
        QMessageBox::warning(this, "Error", "Invalid JSON file: " + error.errorString());
        file.close();
        return;
    }

    QJsonObject jsonObj = doc.object();

    QStandardItemModel *model = new QStandardItemModel();


    buildTree(model, jsonObj);
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("Key"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Value"));

    ui->treeView->setModel(model);

    file.close();
}

void MainWindow::on_actionSave_triggered()
{
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(ui->treeView->model());
    QJsonObject jsonObj;
    saveTree(jsonObj, model->invisibleRootItem());
    //jsonObj = modelToJson(model->invisibleRootItem());
    qDebug()<<jsonObj;

    QJsonDocument document;
    document.setObject( jsonObj );
    QByteArray bytes = document.toJson( QJsonDocument::Indented );


    // Write the JSON data to the specified file
    QFile file(file_path_);
    if (file.open(QFile::WriteOnly | QFile::Text)) {
        QTextStream out(&file);
        out << bytes;
        file.close();
    }
}

void MainWindow::saveTree(QJsonObject& jsonObj, QStandardItem* item)
{
    for(int i = 0; i<item->rowCount(); i++){
        QStandardItem* childItem = item->child(i, 0);
        if(childItem->rowCount() == 0){
            jsonObj.insert(childItem->text(), item->child(i, 1)->text());
        }
        else{
            QJsonObject nestedObj;
            saveTree(nestedObj, childItem);
            jsonObj.insert(childItem->text(), nestedObj);
        }
    }
}

void MainWindow::on_actionSave_as_triggered()
{
    QString file_name = QFileDialog::getSaveFileName(this, tr("Save JSON File"), QString(), tr("JSON Files (*.json)"));

        // If the user cancels the save dialog, return without doing anything
        if (file_name.isNull())
            return;

        // Get the current model from the tree view
        QStandardItemModel *model = qobject_cast<QStandardItemModel*>(ui->treeView->model());

        // Get the JSON data from the model
        QJsonObject jsonObj;
        saveTree(jsonObj, model->invisibleRootItem());

        QJsonDocument document;
        document.setObject( jsonObj );
        QByteArray bytes = document.toJson( QJsonDocument::Indented );


        // Write the JSON data to the specified file
        QFile file(file_name);
        if (file.open(QFile::WriteOnly | QFile::Text)) {
            QTextStream out(&file);
            out << bytes;
            file.close();
        }
}


void MainWindow::on_actionNew_triggered()
{

    QJsonObject jsonObj;

    jsonObj.insert("", "");

    // Create a standard item model from the JSON object
    QStandardItemModel *model = new QStandardItemModel(this);
    buildTree(model, jsonObj, QModelIndex());

    // Set the model in the tree view
    ui->treeView->setModel(model);

    // Expand the tree view to show the root item
    ui->treeView->expand(model->index(0, 0, QModelIndex()));
}


void MainWindow::buildTree(QStandardItemModel *model, const QJsonObject &jsonObj, const QModelIndex &parentIndex)
{
    QMap<QString, QVariant> jsonMap = jsonObj.toVariantMap();
    QMapIterator<QString, QVariant> iter(jsonMap);
    while (iter.hasNext()) {
        iter.next();
        QString key = iter.key();
        QVariant value = iter.value();

        if (value.typeId() == QMetaType::QVariantMap) {
            QJsonObject nestedObj = value.toJsonObject();
            QStandardItem *item = new QStandardItem(key);
            if (parentIndex.isValid()) {
                model->itemFromIndex(parentIndex)->appendRow(item);
            } else {
                model->appendRow(item);
            }
            buildTree(model, nestedObj, item->index());
        } else if (value.typeId() == QMetaType::QVariantList) {
            QJsonArray array = value.toJsonArray();
            QStandardItem *item = new QStandardItem(key);
            if (parentIndex.isValid()) {
                model->itemFromIndex(parentIndex)->appendRow(item);
            } else {
                model->appendRow(item);
            }
            for (int i = 0; i < array.size(); i++) {
                QJsonObject nestedObj = array[i].toObject();
                buildTree(model, nestedObj, item->index());
            }
        } else {
            QList<QStandardItem*> rowItems;
            rowItems << new QStandardItem(key);
            rowItems << new QStandardItem(value.toString());
            if (parentIndex.isValid()) {
                model->itemFromIndex(parentIndex)->appendRow(rowItems);
            } else {
                model->appendRow(rowItems);
            }
        }
    }
}


void MainWindow::on_treeView_pressed(const QModelIndex &index)
{
    /*
    bool isValueWithObjectChild = false;

    // Check if the clicked item is a value with an object child
    if (index.isValid()) {
        QStandardItem *clickedItem = dynamic_cast<QStandardItemModel*>(treeView->model())->itemFromIndex(index);
        if (clickedItem->columnCount() == 2 && clickedItem->child(0, 0) != nullptr && clickedItem->child(0, 0)->rowCount() > 0) {
            isValueWithObjectChild = true;
        }
    }

    // Enable the add action if the clicked item is not a value with an object child
    if (!isValueWithObjectChild) {
        action_new_array_element->setEnabled(true);
        action_new_object_element->setEnabled(true);
        action_delete_element->setEnabled(true);
    }
    */
}


void MainWindow::on_actionNewObject_triggered()
{
    QTreeView* treeView = ui->treeView;
    QStandardItemModel* model = dynamic_cast<QStandardItemModel*>(treeView->model());

    QModelIndex selectedIndex = treeView->currentIndex();
    QStandardItem* selectedItem = model->itemFromIndex(selectedIndex);

    QList<QStandardItem*> rowItems;
    rowItems << new QStandardItem("");
    rowItems << new QStandardItem("");

    selectedItem->appendRow(rowItems);
}

