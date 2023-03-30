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
    action_modify = ui->actionModifyElement;
    action_new_value_element = ui->actionInsertValueElement;

    toolbar = ui->toolBar;
    action_modify->setEnabled(false);
    action_new_array_element->setEnabled(false);
    action_new_object_element->setEnabled(false);
    action_save->setEnabled(false);
    action_save_as->setEnabled(false);
    action_delete_element->setEnabled(false);
    action_new_value_element->setEnabled(false);
    model = dynamic_cast<QStandardItemModel*>(ui->treeView->model());

    //connect(model, &QStandardItemModel::dataChanged, this, &MainWindow::checkModelEmpty);


}

MainWindow::~MainWindow(){
    delete treeView;
    delete action_new_array_element;
    delete action_save;
    delete action_save_as;
    delete action_open;
    delete toolbar;
};

void MainWindow::checkModelEmpty()
{
    qDebug()<<"got here";
    bool modelEmpty = true;
    QTreeView* treeView = ui->treeView;
    QStandardItemModel* model = dynamic_cast<QStandardItemModel*>(treeView->model());

    if (model) {
        QModelIndex rootIndex = model->index(0, 0, QModelIndex());
        if (rootIndex.isValid() && model->rowCount(rootIndex) > 0) {
            modelEmpty = false;
        }
    }

    ui->actionNewArrayElement->setEnabled(!modelEmpty);
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

    action_modify->setEnabled(false);
    //action_new_array_element->setEnabled(false);
    action_new_object_element->setEnabled(false);
    action_save->setEnabled(false);
    action_save_as->setEnabled(false);
    action_delete_element->setEnabled(false);
}

void MainWindow::on_actionSave_triggered()
{
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(ui->treeView->model());
    QJsonObject jsonObj = saveTree(model->invisibleRootItem());
    //jsonObj = modelToJson(model->invisibleRootItem());
    qDebug()<<jsonObj;
    //QJsonObject jsonObj;
    //saveTree(jsonObj,model->invisibleRootItem());

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
    /*
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
    jsonArr.append(jsonObject);*/
    for (int j = 0; j < item->rowCount(); j++) {
        QStandardItem* subChildItem = item->child(j, 0);

        // Check if the sub-child item represents a JSON object or array
        if (subChildItem->data(Qt::UserRole).toString() == "object") {
            QJsonObject childObj;
            saveTree(childObj, subChildItem);
            QJsonObject newObject;
            newObject.insert(subChildItem->text(),childObj);
            jsonArr.append(newObject);
        }else if(subChildItem->rowCount() == 0 && subChildItem->text()!=""){
            QJsonObject childObj;
            childObj[subChildItem->text()] = item->child(j,1)->text();
            jsonArr.append(childObj);
        } else if(item->child(j, 1)->data(Qt::UserRole).toString()=="number"){
            qDebug()<<item->child(j, 1)->data(Qt::UserRole).toString();
            //QString value = childItem->child(j, 1)->text().toDouble();
            QJsonValue jsonValue(item->child(j, 1)->text().toDouble());
            jsonArr.append(jsonValue);
        }else{
            QString value = item->child(j, 1)->text();
            QJsonValue jsonValue(value);
            jsonArr.append(jsonValue);
        }
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


QJsonObject MainWindow::saveTree(QStandardItem* item)
{
    QJsonObject result;
    qDebug()<<item->rowCount();
    // Iterate over all child items of the current item
    for (int i = 0; i < item->rowCount(); i++) {
        QStandardItem* childItem = item->child(i, 0);
        qDebug() << childItem->text();
        if(childItem->rowCount() == 0 && childItem->text()!=""){
            qDebug() << "test";
            result[childItem->text()] = item->child(i, 1)->text();
        }
        // Check if the child item represents a JSON object
        else if (childItem->data(Qt::UserRole).toString() == "object") {
            qDebug() << "test1";
            //QJsonObject childObj = saveTree(childItem);
            QJsonObject childObj;
            saveTree(childObj, childItem);
            result[childItem->text()] = childObj;
        }
        // Check if the child item represents a JSON array
        else if (childItem->data(Qt::UserRole).toString() == "array") {
            qDebug() << "test2";
            QJsonArray childArr;
            for (int j = 0; j < childItem->rowCount(); j++) {
                QStandardItem* subChildItem = childItem->child(j, 0);

                // Check if the sub-child item represents a JSON object or array
                if (subChildItem->data(Qt::UserRole).toString() == "object") {
                    QJsonObject childObj;
                    saveTree(childObj, subChildItem);
                    QJsonObject newObject;
                    newObject.insert(subChildItem->text(),childObj);
                    childArr.append(newObject);
                }else if(subChildItem->rowCount() == 0 && subChildItem->text()!=""){
                    QJsonObject childObj;
                    childObj[subChildItem->text()] = subChildItem->child(i, 1)->text();
                    childArr.append(childObj);
                } else if(childItem->child(j, 1)->data(Qt::UserRole).toString()=="number"){
                    qDebug()<<childItem->child(j, 1)->data(Qt::UserRole).toString();
                    //QString value = childItem->child(j, 1)->text().toDouble();
                    QJsonValue jsonValue(childItem->child(j, 1)->text().toDouble());
                    childArr.append(jsonValue);
                }else{
                    QString value = childItem->child(j, 1)->text();
                    QJsonValue jsonValue(value);
                    childArr.append(jsonValue);
                }
            }
            result[childItem->text()] = childArr;
        }
        // Otherwise, the child item represents a key-value pair
        else {
            qDebug() << "test3";
            QString key = childItem->text();
            qDebug() << item->child(i,1);
            //QString value = childItem->child(1, 0)->text();
            QString value = item->child(i,1)->text();
            qDebug() << key;

            QJsonValue jsonValue(value);
            result[key] = jsonValue;
        }
    }

    return result;
}

void MainWindow::on_actionSave_as_triggered()
{
    QString file_name = QFileDialog::getSaveFileName(this, tr("Save JSON File"), QString(), tr("JSON Files (*.json)"));

        // If the user cancels the save dialog, return without doing anything
        if (file_name.isNull())
            return;

        QStandardItemModel *model = qobject_cast<QStandardItemModel*>(ui->treeView->model());
        QJsonObject jsonObj = saveTree(model->invisibleRootItem());

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

    QStandardItemModel *model = new QStandardItemModel(this);
    buildTree(model, jsonObj, QModelIndex());
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("Key"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Value"));
    // Set the model in the tree view
    ui->treeView->setModel(model);

    // Expand the tree view to show the root item
    ui->treeView->expand(model->index(0, 0, QModelIndex()));


    action_modify->setEnabled(true);
    action_save_as -> setEnabled(true);
    action_save->setEnabled(file_path_!="");
    action_delete_element->setEnabled(false);
    action_new_object_element->setEnabled(true);
    action_new_array_element->setEnabled(true);
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
                    QStandardItem *newItem = new QStandardItem(value.toString());
                    newItem->setData("number",Qt::UserRole);
                    QList<QStandardItem*> rowItems;
                    rowItems << new QStandardItem("");
                    rowItems << newItem;
                    item->appendRow(rowItems);
                } else {
                    QString value = element.toString();
                    QStandardItem *newItem = new QStandardItem(value);
                    newItem->setData("string",Qt::UserRole);
                    QList<QStandardItem*> rowItems;
                    rowItems << new QStandardItem("");
                    rowItems << newItem;

                    item->appendRow(rowItems);
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
    bool isArray = false;
    bool isObject = false;

    // Check if the clicked item is a value with an object child
    if (index.isValid()) {
        isValueValid = true;

        QStandardItem *clickedItem = dynamic_cast<QStandardItemModel*>(ui->treeView->model())->itemFromIndex(index);

        if(clickedItem->data(Qt::UserRole) == "object") {
           isObject=true;
        }
        else if(clickedItem->data(Qt::UserRole) == "array"){
            isArray=true;
        }

    }

    if(isValueValid){
        action_modify->setEnabled(true);
        action_delete_element->setEnabled(true);

        action_save_as -> setEnabled(true);

        if(file_path_!=""){
            action_save->setEnabled(true);
        }else{
            action_save->setEnabled(false);
        }

    }else{
        action_delete_element->setEnabled(false);
        action_save->setEnabled(false);
        action_save_as -> setEnabled(false);
    }
    if(isArray){
        //action_new_array_element->setEnabled(true);
        action_new_value_element->setEnabled(true);
    }else{
        //action_new_array_element->setEnabled(false);
        action_new_value_element->setEnabled(false);
    }
    if(isObject){
        action_new_object_element->setEnabled(true);
    }else{
        action_new_object_element->setEnabled(false);
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

        QTreeView* treeView = ui->treeView;
        QStandardItemModel* model = dynamic_cast<QStandardItemModel*>(treeView->model());

        QModelIndex selectedIndex = treeView->currentIndex();
        QStandardItem* selectedItem = model->itemFromIndex(selectedIndex);

        QJsonObject jsonObject = saveTree(model->invisibleRootItem());

        //qDebug() << selectedItem->text();
        //qDebug() << selectedItem->data(Qt::UserRole);

        // Create a new dialog to get user input
        dialogInsertObject = new DialogInsertObject(this);
        dialogInsertObject->show();

        int result = dialogInsertObject->exec();

        // If the user clicked OK, add the new object to the model
        if (result == QDialog::Accepted) {

            // Get the user's input from the dialog
            QString key = dialogInsertObject->getKey();
            QString value = dialogInsertObject->getValue();
            QList<QStandardItem*> rowItems;
            rowItems << new QStandardItem(key);
            rowItems << new QStandardItem(value);
            if (selectedItem!=nullptr && selectedItem->data(Qt::UserRole) == "object") {
                selectedItem->appendRow(rowItems);
            }else if(jsonObject.empty()){
                model->appendRow(rowItems);
            }
        }
        // Clean up the dialog
        delete dialogInsertObject;
        dialogInsertObject = nullptr;

}


void MainWindow::on_actionModifyElement_triggered()
{
    QTreeView* treeView = ui->treeView;
    QStandardItemModel* model = dynamic_cast<QStandardItemModel*>(treeView->model());

    QModelIndex selectedIndex = treeView->currentIndex();
    QStandardItem* selectedItem = model->itemFromIndex(selectedIndex);

    QJsonObject object = saveTree(model->invisibleRootItem());
    dialogModify = new DialogModify(this,&object);
    dialogModify->show();

    int result = dialogModify->exec();

    if (result == QDialog::Accepted) {
        ui->treeView->setModel(dialogModify->getNewModal());
    }

    delete dialogModify;
    dialogModify = nullptr;
}


void MainWindow::on_actionNewArrayElement_triggered()
{
    QTreeView* treeView = ui->treeView;
    QStandardItemModel* model = dynamic_cast<QStandardItemModel*>(treeView->model());
    QModelIndex selectedIndex = treeView->currentIndex();
    QStandardItem* selectedItem = model->itemFromIndex(selectedIndex);

    QJsonObject jsonObject = saveTree(model->invisibleRootItem());

    // Create a new dialog to get user input
    dialogInsertArray = new DialogInsertArray(this);
    dialogInsertArray->show();

    int result = dialogInsertArray->exec();

    // If the user clicked OK, add the new object to the model
    if (result == QDialog::Accepted) {

        // Get the user's input from the dialog

        if (selectedItem->data(Qt::UserRole) == "array") {
            //TODO
            // Get the user's input from the dialog
                    QString value = dialogInsertArray->getString();

                    if (selectedItem->data(Qt::UserRole) == "array") {
                        QJsonArray jsonArray = jsonObject[selectedItem->text()].toArray();
                        QJsonArray* newJsonArray = new QJsonArray();
                        newJsonArray->append(value);
                        jsonArray.append(*newJsonArray);
                        qDebug()<<jsonArray;
                        jsonObject[selectedItem->text()] = jsonArray;
                        qDebug()<<jsonObject;
                        // Update the model
                        model->clear();
                        QStandardItemModel *model_new = new QStandardItemModel();
                        buildTree(model_new, jsonObject);
                        model_new->setHeaderData(0, Qt::Horizontal, QObject::tr("Key"));
                        model_new->setHeaderData(1, Qt::Horizontal, QObject::tr("Value"));
                        ui->treeView->setModel(model_new);
                    }
        }
    }
    // Clean up the dialog
    delete dialogInsertArray;
    dialogInsertArray = nullptr;
}


void MainWindow::on_actionInsertValueElement_triggered()
{
    QTreeView* treeView = ui->treeView;
    QStandardItemModel* model = dynamic_cast<QStandardItemModel*>(treeView->model());
    QModelIndex selectedIndex = treeView->currentIndex();
    QStandardItem* selectedItem = model->itemFromIndex(selectedIndex);

    QJsonObject jsonObject = saveTree(model->invisibleRootItem());

    // Create a new dialog to get user input
    dialogInsertArray = new DialogInsertArray(this);
    dialogInsertArray->show();

    int result = dialogInsertArray->exec();

    // If the user clicked OK, add the new object to the model
    if (result == QDialog::Accepted) {

        // Get the user's input from the dialog

        if (selectedItem->data(Qt::UserRole) == "array") {
            //TODO
            // Get the user's input from the dialog


                        QJsonArray jsonArray = jsonObject[selectedItem->text()].toArray();
                        if(dialogInsertArray->isStringChecked()){
                            QString value = dialogInsertArray->getString();
                            jsonArray.append(value);
                        }

                        else{
                            double value = dialogInsertArray->getDouble();
                            jsonArray.append(value);
                        }

                        qDebug()<<jsonArray;
                        jsonObject[selectedItem->text()] = jsonArray;
                        qDebug()<<jsonObject;
                        // Update the model
                        model->clear();
                        QStandardItemModel *model_new = new QStandardItemModel();
                        buildTree(model_new, jsonObject);
                        model_new->setHeaderData(0, Qt::Horizontal, QObject::tr("Key"));
                        model_new->setHeaderData(1, Qt::Horizontal, QObject::tr("Value"));
                        ui->treeView->setModel(model_new);

        }
    }
    // Clean up the dialog
    delete dialogInsertArray;
    dialogInsertArray = nullptr;
}

