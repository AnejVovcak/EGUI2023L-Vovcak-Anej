#include "mainwindow.h"
#include "dialoginsertobject.h"
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
    action_new_object_element = ui->insert_object_element;
    action_save = ui->actionSave;
    action_save_as = ui->actionSave_as;
    action_delete_element = ui->actiondeleteNode;

    toolbar = ui->toolBar;
    action_new_array_element->setEnabled(false);
    action_new_object_element->setEnabled(false);
    action_save->setEnabled(false);
    action_save_as->setEnabled(false);
    action_delete_element->setEnabled(false);
    model = dynamic_cast<QStandardItemModel*>(ui->treeView->model());
}

MainWindow::~MainWindow(){
    delete treeView;
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
    //qDebug()<<jsonObj;

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


void MainWindow::saveTree2(QJsonArray& jsonArr, QStandardItem* item)
{
    QJsonObject jsonObject;

    for(int i = 0; i<item->rowCount(); i++){
        QStandardItem* childItem = item->child(i, 0);

        if(childItem->rowCount() == 0){
            jsonObject.insert(childItem->text(), item->child(i, 1)->text());
        }
        else if(childItem->data(Qt::UserRole) == "array"){
            QJsonArray nestedArr;
            saveTree2(nestedArr, childItem);
            jsonObject.insert(childItem->text(), nestedArr);
        }
        else{
            QJsonObject nestedObj;
            saveTree(nestedObj, childItem);
            jsonObject.insert(childItem->text(), nestedObj);
        }

    }
    jsonArr.append(jsonObject);

}


void MainWindow::saveTree(QJsonObject& jsonObj, QStandardItem* item)
{
    for(int i = 0; i<item->rowCount(); i++){
        QStandardItem* childItem = item->child(i, 0);

        if(childItem->rowCount() == 0){
            jsonObj.insert(childItem->text(), item->child(i, 1)->text());
        }
        else if(childItem->data(Qt::UserRole) == "array"){
            QJsonArray nestedArray;
            saveTree2(nestedArray,childItem);
            jsonObj[childItem->text()] = nestedArray;
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
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("Key"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Value"));
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
            item->setData("object", Qt::UserRole);
            if (parentIndex.isValid()) {
                model->itemFromIndex(parentIndex)->appendRow(item);
            } else {
                model->appendRow(item);
            }
            buildTree(model, nestedObj, item->index());
        } else if (value.typeId() == QMetaType::QVariantList) {
            QJsonArray array = value.toJsonArray();
            QStandardItem *item = new QStandardItem(key);
            item->setData("array", Qt::UserRole);
            if (parentIndex.isValid()) {
                model->itemFromIndex(parentIndex)->appendRow(item);
            } else {
                model->appendRow(item);
            }
            for (int i = 0; i < array.size(); i++) {

                QJsonValue element = array.at(i);
                if (element.isObject()) {
                    QJsonObject nestedObj = element.toObject();
                    buildTree(model, nestedObj, item->index());
                } else if (element.isDouble() || element.isBool()) {
                    QVariant value = element.toVariant();
                    QList<QStandardItem*> rowItems;
                    rowItems << new QStandardItem("");
                    rowItems << new QStandardItem(value.toString());
                    if (parentIndex.isValid()) {
                        item->appendRow(rowItems);
                    } else {
                        model->appendRow(rowItems);
                    }
                } else {
                    QString value = element.toString();
                    QList<QStandardItem*> rowItems;
                    rowItems << new QStandardItem("");
                    rowItems << new QStandardItem(value);
                    if (parentIndex.isValid()) {
                        item->appendRow(rowItems);
                    } else {
                        model->appendRow(rowItems);
                    }
                }
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

    bool isValueWithObjectChild = false;
    bool isValueValid = false;

    // Check if the clicked item is a value with an object child
    if (index.isValid()) {
        isValueValid = true;

        QStandardItem *clickedItem = dynamic_cast<QStandardItemModel*>(ui->treeView->model())->itemFromIndex(index);
        if (clickedItem != nullptr && clickedItem->rowCount() > 0) {
            isValueWithObjectChild = true;
        }

    }
    // Enable the add action if the clicked item is not a value with an object child
    if (isValueWithObjectChild) {
        action_new_array_element->setEnabled(true);
        action_new_object_element->setEnabled(true);
    }else {
        action_new_array_element->setEnabled(false);
        action_new_object_element->setEnabled(false);
    }

    if(isValueValid){
        action_delete_element->setEnabled(true);
        action_save->setEnabled(true);
        action_save_as -> setEnabled(true);
    }else{
        action_delete_element->setEnabled(false);
        action_save->setEnabled(false);
        action_save_as -> setEnabled(false);
    }

}


void MainWindow::on_actiondeleteNode_triggered()
{
    QTreeView* treeView = ui->treeView;
    QStandardItemModel* model = dynamic_cast<QStandardItemModel*>(treeView->model());

    QModelIndex selectedIndex = treeView->currentIndex();
    QStandardItem* selectedItem = model->itemFromIndex(selectedIndex);

        // Check if the selected item is valid
        if (!selectedItem)
            return;

        // Get the parent item of the selected item
        QStandardItem* parentItem = selectedItem->parent();
        if (!parentItem)
            parentItem = model->invisibleRootItem();

        // Remove the selected item and its children
        parentItem->removeRow(selectedIndex.row());
}


void MainWindow::on_insert_object_element_triggered()
{

        qDebug() << "test";
        QTreeView* treeView = ui->treeView;
        QStandardItemModel* model = dynamic_cast<QStandardItemModel*>(treeView->model());

        QModelIndex selectedIndex = treeView->currentIndex();
        QStandardItem* selectedItem = model->itemFromIndex(selectedIndex);

        qDebug() << selectedItem->text();
        qDebug() << selectedItem->data(Qt::UserRole);

        // Create a new dialog to get user input
        dialogInsertObject = new DialogInsertObject(this);
        dialogInsertObject->show();

        int result = dialogInsertObject->exec();

        // If the user clicked OK, add the new object to the model
        if (result == QDialog::Accepted) {

            // Get the user's input from the dialog
            QString key = dialogInsertObject->getKey();
            QString value = dialogInsertObject->getValue();

            // Create a new item for the object
            QStandardItem *item = new QStandardItem(key);
            item->setData("object", Qt::UserRole);

            // If the user selected an item in the tree, add the new item as a child
            // of that item. Otherwise, add it as a top-level item.
            if (selectedItem->data(Qt::UserRole) == "array") {
                /*
                QList<QStandardItem*> rowItems;
                rowItems << item;
                rowItems << new QStandardItem(value);
                selectedItem->appendRow(rowItems);*/
                QStandardItem *item = new QStandardItem("");
                QJsonObject obj;
                obj.insert(key, value);
                item->setData(obj, Qt::UserRole);
                selectedItem->appendRow(item);
            }
            if (selectedItem->data(Qt::UserRole) == "object") {
                QList<QStandardItem*> rowItems;
                rowItems << item;
                rowItems << new QStandardItem(value);
                selectedItem->appendRow(rowItems);
            }
        }
        // Clean up the dialog
        delete dialogInsertObject;
        dialogInsertObject = nullptr;

}

