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
    //treeView = new QTreeView(this);
    action_new_array_element = ui->actionNewArray;
    toolbar = ui->toolBar;
    action_new_array_element->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}


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
    qDebug()<<jsonObj;

    QFile file(file_path_);
        if (file.open(QFile::WriteOnly | QFile::Text)) {
            QTextStream out(&file);
            out << &jsonObj;
            file.close();
        }
}

void MainWindow::saveTree(QJsonObject& jsonObj, QStandardItem* item)
{

    for(int i = 0; i<item->rowCount(); i++){

       if(item->child(i,0)->rowCount() == 0){
           jsonObj.insert(item->child(i,0)->text(), item->child(i,1)->text());
       }
       else{
           QJsonObject nestedObj;
           for (int j = 0; j < item->child(i,0)->rowCount(); j++) {
               QStandardItem* childItem = item->child(j);
               saveTree(nestedObj, childItem);
           }
           jsonObj[item->child(i,0)->text()] = nestedObj;
       }

    }
}

QJsonObject MainWindow::modelToJson(QStandardItem *item)
{
    QJsonObject jsonObj;

    int numRows = item->rowCount();
    int numCols = item->columnCount();

    if (numCols == 2) { // Key-value pair
        QString key = item->child(0, 0)->text();
        QString value = item->child(0, 1)->text();
        jsonObj.insert(key, value);
    } else if (numCols == 1 && numRows > 0) { // Object or array
        bool isArray = true;
        for (int i = 0; i < numRows; i++) {
            if (item->child(i, 0)->columnCount() != 2) {
                isArray = false;
                break;
            }
        }
        if (isArray) { // Array
            QJsonArray jsonArray;
            for (int i = 0; i < numRows; i++) {
                QJsonObject nestedObj = modelToJson(item->child(i, 0));
                jsonArray.append(nestedObj);
            }
            jsonObj = QJsonObject{{"array", jsonArray}};
        } else { // Object
            for (int i = 0; i < numRows; i++) {
                QStandardItem *childItem = item->child(i, 0);
                QString key = childItem->child(0, 0)->text();
                QJsonObject nestedObj = modelToJson(childItem);
                jsonObj.insert(key, nestedObj);
            }
        }
    }

    return jsonObj;
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


void MainWindow::on_actionactionNewNode_triggered()
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

void MainWindow::on_treeView_clicked(const QModelIndex &index)
{

}


void MainWindow::on_treeView_pressed(const QModelIndex &index)
{
    action_new_array_element->setEnabled(true);
}

