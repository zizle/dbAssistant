#include "centerwidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMargins>
#include <QDebug>
#include <QSqlQuery>
#include <QMessageBox>
#include <QSqlRecord>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include "dbnamelist.h"

CenterWidget::CenterWidget(QWidget *parent) : QWidget(parent)
{
    // 设置窗口总左右布局
    QHBoxLayout * mainHLayout = new QHBoxLayout;
    mainHLayout->setContentsMargins(QMargins(0,0,0,0)); // 直接影响子布局
    mainHLayout->setSpacing(0);
    this->m_DbNameList = new DBNameList(this);
    this->m_DbNameList->setMaximumWidth(200);
    // 连接右键菜单信号
    void (CenterWidget:: * actionSlot)(QString action, QString dbName) = &CenterWidget::handlerNameListAction;
    //connect(this->m_DbNameList, &DBNameList::nameListActionSignal,this,SLOT(handlerNameListAction(QString action, QString dbName)));
    connect(this->m_DbNameList, &DBNameList::nameListActionSignal,this,actionSlot);
    mainHLayout->addWidget(m_DbNameList);
    this->setLayout(mainHLayout);
    // 右侧上下布局
    QVBoxLayout * operateVLayout = new QVBoxLayout;
    //    operateVLayout->setContentsMargins(QMargins(0,0,0,0));

    operateVLayout->setSpacing(0);
    QHBoxLayout * tableOperaeHLayout = new QHBoxLayout; // 显示表和删除表按钮左右布局
    tableOperaeHLayout->setContentsMargins(QMargins(0,0,0,0));


    this->m_TableComboBox = new QComboBox(this);// 数据表的下拉列表

    connect(this->m_TableComboBox, &QComboBox::currentTextChanged, this, &CenterWidget::handlerTableNameChanged);
    this->m_DeleteTableBtn = new QPushButton("删除",this); // 删除按钮
    tableOperaeHLayout->addWidget(this->m_TableComboBox);
    tableOperaeHLayout->addWidget(this->m_DeleteTableBtn);
    tableOperaeHLayout->addStretch();  // 添加伸缩
    operateVLayout->addLayout(tableOperaeHLayout); // 加入右侧的上下布局

    this->m_RecordsTable = new QTableWidget(this);
    operateVLayout->addWidget(this->m_RecordsTable);


    mainHLayout->addLayout(operateVLayout); // 加入主布局
}

void CenterWidget::setDbNameList(QStringList dbNames)
{
    this->m_DbNameList->addItems(dbNames);
    DBNameList * dbnames = dynamic_cast<DBNameList *>(this->m_DbNameList);
    dbnames->m_IsConnected = true;
}

// 左侧列表的右击信号
 void CenterWidget::handlerNameListAction(QString action, QString dbName)
 {
     QSqlDatabase db = QSqlDatabase::database();
     if(!db.isOpen()){db.open();}
     QSqlQuery query;
     if (action == "addNew")
     {
         // 新增一个数据库
         qDebug() << "新增一个数据库";
     }
     else if (action == "delete")
     {
         if(QMessageBox::Yes == QMessageBox::question(this, "删除", "确定删除当前数据库吗?", QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel))
         {
             // 删除当前数据库
             query.exec(QString("DROP DATABASE %1;").arg(dbName));
         }
         else{}
     }
     else if(action == "connect")
     {
         query.exec(QString("use %1;").arg(dbName)); // 使用当前数据库
         QStringList tables = db.tables();
         this->m_TableComboBox->clear();
         this->m_TableComboBox->addItems(tables);
     }
     db.close();
 }
 // 选择的表改变
 void CenterWidget::handlerTableNameChanged(QString tbname)
 {
     QSqlDatabase db = QSqlDatabase::database();
     if(!db.isOpen()){db.open();}
     QSqlQuery query;
     query.clear();
     QString dbname = this->m_DbNameList->currentItem()->text();
     query.exec(QString("use %1").arg(dbname));
     query.exec(QString("SELECT * FROM %1").arg(tbname));
     QSqlRecord rec = query.record();
//   qDebug() << "列数:" << rec.count();
//   qDebug() << "行数:" << query.size();
     this->m_RecordsTable->clear();
     // 设置表格的行列数
     this->m_RecordsTable->setRowCount(query.size());
     this->m_RecordsTable->setColumnCount(rec.count());
     // 获取列名，设置成表头
     for(int col = 0;col < rec.count(); col++)
     {
         //qDebug() << "列名:" << rec.fieldName(col);
         this->m_RecordsTable->setHorizontalHeaderItem(col, new QTableWidgetItem(rec.fieldName(col)));
     }
     // 获取数据显示在表中
     for (int row = 0; row < query.size(); row++) {
         if(query.next()){
             for (int col=0; col < rec.count(); col ++) {
                 this->m_RecordsTable->setItem(row, col, new QTableWidgetItem(query.value(col).toString()));
                 //qDebug() << query.value(col).toString();
             }
         }
     }
     db.close();
 }

 // 工具栏操作的槽函数
 void CenterWidget::handlerPageChanged(QString toolAction)
 {
     QListWidgetItem * currentDBItem = this->m_DbNameList->currentItem();
     if (currentDBItem == NULL)
     {
         QMessageBox::information(this, "错误", "请连接数据库后操作!");
         return;
     }
     QString dbname = currentDBItem->text();
     QString tbname = this->m_TableComboBox->currentText();
     if (dbname == "" || tbname == ""){
        QMessageBox::information(this, "错误", "请连接数据库后操作!");
        return;
     }
     QSqlDatabase db = QSqlDatabase::database();
     //qDebug()<<db;
     if(!db.isOpen()){db.open();}
     QSqlQuery query;
     query.clear();
     query.exec(QString("use %1").arg(dbname));
     if (toolAction == "insertRecord")
     {
         this->insertRecord();
     }
     else if(toolAction == "updateRecord")
     {
         this->updateRecord();
     }
     else if(toolAction == "deleteRecord")
     {
         this->deleteRecord();
     }
     else if(toolAction == "prePage")
     {
         this->prePageRecords();
     }
     else if(toolAction == "nextPage")
     {
         this->nextPageRecords();
     }
     else
     {
         QMessageBox::information(this, "错误", "暂不支持此操作!");
     }
     db.close();
 }

 // 对当前数据表插入一条记录
 void CenterWidget::insertRecord()
 {
     QString tbname = this->m_TableComboBox->currentText();
     QStringList headerList;
     QStringList placeholders;
     for (int col=0; col < this->m_RecordsTable->columnCount(); col++)
     {
         QString headerText = this->m_RecordsTable->horizontalHeaderItem(col)->text();
         if(headerText == "id"){continue;}
         headerList << headerText;
         placeholders << QString(":%1").arg(headerText);
     }// 获取列表头数据
     QString headersStr = headerList.join(",");
     QString pholdersStr = placeholders.join(",");
     QString insetStatement = QString("INSERT INTO %1 (%2) VALUES (%3);").arg(tbname).arg(headersStr).arg(pholdersStr);
     //qDebug() << "插入一条记录";
     //qDebug() << insetStatement;
     // 弹窗输入数据
     QDialog *insertDialog = new QDialog;
     insertDialog->setAttribute(Qt::WA_DeleteOnClose);
     QGridLayout *dglayout = new QGridLayout;
     for (int i = 0; i < headerList.size(); i++) {
         dglayout->addWidget(new QLabel(headerList[i], insertDialog), i, 0);
         dglayout->addWidget(new QLineEdit(insertDialog), i, 1);
     }
     QPushButton * addBtn = new QPushButton("确定",insertDialog);
     dglayout->addWidget(addBtn, headerList.size(), 0, 1,2);
     insertDialog->setLayout(dglayout);

     connect(addBtn, &QPushButton::clicked, insertDialog, [=](){
         QSqlQuery query;
         query.prepare(insetStatement);
         for (int row = 0; row < dglayout->rowCount() - 1; row++) {
             QWidget *labelWidget = dglayout->itemAtPosition(row,0)->widget();
             QWidget *editWidget = dglayout->itemAtPosition(row, 1)->widget();
             QLabel *label = dynamic_cast<QLabel *>(labelWidget);
             QLineEdit * lineEdit = dynamic_cast<QLineEdit *>(editWidget);
             query.bindValue(QString(":%1").arg(label->text()), lineEdit->text());
         }
         bool qresult = query.exec();
         if (qresult)
         {
             QMessageBox::information(insertDialog, "成功", "新增记录成功!");
             insertDialog->close();
         }
         else
         {
           QMessageBox::warning(insertDialog, "失败", "插入数据失败!");
         }
     });
     insertDialog->exec();
     this->handlerTableNameChanged(tbname); // 刷新表格
 }

 // 对当前选择的数据行进行更新
 void CenterWidget::updateRecord()
 {
     qDebug() << "更新一条记录";
 }

 // 删除当前选择的数据行
 void CenterWidget::deleteRecord()
 {
     qDebug() << "删除一条记录";
 }
 // 当前表上一页
 void CenterWidget::prePageRecords()
 {
     qDebug() << "上一页记录";
 }

 // 当前表下一页
 void CenterWidget::nextPageRecords()
 {
     qDebug() << "下一页记录";
 }


