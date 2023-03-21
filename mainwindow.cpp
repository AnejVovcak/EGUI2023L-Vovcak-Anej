#include "mainwindow.h"
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
#include "qjsonmodel.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actionOpen_triggered()
{
    QString file_name = QFileDialog::getOpenFileName(this, "Open the file", "", "JSON files (*.json)");
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
    //model->appendRow({new QStandardItem(QString("key")), new QStandardItem(QString("value"))});
    buildTree2(model, jsonObj);
    ui->treeView->setModel(model);
    file.close();
}

void MainWindow::on_actionSave_triggered()
{
    /*
    // Get the current model from the tree view
    QJsonModel *model = static_cast<QJsonModel*>(ui->treeView->model());

    // Get the JSON data from the model
    QByteArray jsonData = model->json();

    // Write the JSON data back to the file
    QFile file(file_path_);
    if (file.open(QFile::WriteOnly | QFile::Text)) {
        QTextStream out(&file);
        out << jsonData;
        file.close();
    }
    */
    convertToModel();
}


void MainWindow::on_actionSave_as_triggered()
{
    QString file_name = QFileDialog::getSaveFileName(this, tr("Save JSON File"), QString(), tr("JSON Files (*.json)"));

        // If the user cancels the save dialog, return without doing anything
        if (file_name.isNull())
            return;

        // Get the current model from the tree view
        QJsonModel *model = static_cast<QJsonModel*>(ui->treeView->model());

        // Get the JSON data from the model
        QByteArray jsonData = model->json();

        // Write the JSON data to the specified file
        QFile file(file_name);
        if (file.open(QFile::WriteOnly | QFile::Text)) {
            QTextStream out(&file);
            out << jsonData;
            file.close();
        }
}


void MainWindow::on_actionNew_triggered()
{
    //Create a new, empty JSON object
    //QJsonObject jsonObject;

    // Create a new QJsonModel with the empty JSON object as the root
    //QJsonModel *model = new QJsonModel;
    //model->

    // Set the model on the tree view
    //ui->treeView->setModel(model);
    // Create an empty JSON object
    QJsonObject jsonObj;

    // Add an empty key-value pair
    jsonObj.insert("", "");

    // Create a standard item model from the JSON object
    QStandardItemModel *model = new QStandardItemModel(this);
    buildTree(model, jsonObj, QModelIndex());

    // Set the model in the tree view
    ui->treeView->setModel(model);

    // Expand the tree view to show the root item
    ui->treeView->expand(model->index(0, 0, QModelIndex()));
}

void MainWindow::convertToModel()
{
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(ui->treeView->model());
    QJsonObject jsonObject = buildObjectFromModel(model->invisibleRootItem());
    QJsonDocument jsonDocument(jsonObject);
    QFile file(file_path_);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << jsonDocument.toJson(QJsonDocument::Indented);
        file.close();
    }
}

QJsonObject MainWindow::buildObjectFromModel(QStandardItem *item)
{
    QJsonObject jsonObject;
    for (int i = 0; i < item->rowCount(); i++) {
        QStandardItem *keyItem = item->child(i,1);
        QStandardItem *valueItem = item->child(i, 1);

        QString key = keyItem->data().toString();
        qDebug() << key;
        QVariant value = valueItem->data();
        if (value.typeId() == QMetaType::QVariantMap) {
            QJsonObject nestedObject = buildObjectFromModel(valueItem);
            jsonObject.insert(key, nestedObject);
        } else if (value.typeId() == QMetaType::QVariantList) {
            QJsonArray jsonArray = buildArrayFromModel(valueItem);
            jsonObject.insert(key, jsonArray);
        } else {
            QString valueString = value.toString();
            qDebug() << valueString;
            jsonObject.insert(key, valueString);
        }
    }
    return jsonObject;
}

QJsonArray MainWindow::buildArrayFromModel(QStandardItem *item)
{
    QJsonArray jsonArray;
    for (int i = 0; i < item->rowCount(); i++) {
        QStandardItem *valueItem = item->child(i, 0);
        QVariant value = valueItem->data();
        if (value.typeId() == QMetaType::QVariantMap) {
            QJsonObject nestedObject = buildObjectFromModel(valueItem);
            jsonArray.append(nestedObject);
        } else if (value.typeId() == QMetaType::QVariantList) {
            QJsonArray nestedArray = buildArrayFromModel(valueItem);
            jsonArray.append(nestedArray);
        } else {
            QString valueString = value.toString();
            jsonArray.append(valueString);
        }
    }
    return jsonArray;
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

