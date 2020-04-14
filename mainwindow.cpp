#include "mainwindow.h"
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QStatusBar>
#include <QLabel>
#include <QToolBar>
#include <QMap>
#include <QDebug>
#include "centerwidget.h"
#include "cmysqldialog.h"
#include <QSqlDatabase>
#include <QSqlQuery>

MainWindow::MainWindow(QWidget *parent) :QMainWindow(parent)
{
    this->resize(900, 600); // 设置大小
    this->setWindowTitle("数据库助手");
    QMenuBar * menuBar = this->menuBar();
    this->setMenuBar(menuBar); //  添加菜单栏
    QMenu * dbMenu = menuBar->addMenu("数据库");  // 创建菜单
    // 创建菜单项
    QAction * mysqlAction = dbMenu->addAction("MySQL");
    QAction * sqliteAction = dbMenu->addAction("SQLite");
    CmysqlDialog * cmysql = new CmysqlDialog(this);
    void (MainWindow:: * toConnectDB)(QMap<QString,QString> *map) = &MainWindow::configDataBase;
    connect(cmysql, &CmysqlDialog::makeDBConnection, this, toConnectDB);
    // 菜单项的选择信号
    connect(mysqlAction, &QAction::triggered,this, [=](){
        this->m_DbType->setText("MySQL");
        // 弹出模态框
        cmysql->exec();
    });
    connect(sqliteAction, &QAction::triggered,this, [=](){
        this->m_DbType->setText("SQLite");
    });


//    connect(dialogMysql, &ConnectMySQLDialog::makeDBConnection, [=](){
//        qDebug() << "1111";
//        dialogMysql->connectingDB();
//    });


    // 添加工具栏
    QToolBar * toolBar = new QToolBar(this);
    this->addToolBar(Qt::RightToolBarArea,toolBar);
    toolBar->setFloatable(false);
    toolBar->setAllowedAreas(Qt::RightToolBarArea);
    QAction * insertRecord = new QAction("新增");
    QAction * deleteRecord = new QAction("删除");
    QAction * updateRecord = new QAction("修改");
    QAction * prePage = new QAction("上页");
    QAction * nextPage = new QAction("下页");

    toolBar->addAction(insertRecord);
    toolBar->addAction(updateRecord);
    toolBar->addAction(deleteRecord);
    toolBar->addAction(prePage);
    toolBar->addAction(nextPage);
    // 发送信号
    connect(insertRecord, &QAction::triggered, this,[=](){
        emit this->actionSignals("insertRecord");
    });
    connect(updateRecord, &QAction::triggered, this,[=](){
        emit this->actionSignals("updateRecord");
    });
    connect(deleteRecord, &QAction::triggered, this,[=](){
        emit this->actionSignals("deleteRecord");
    });
    connect(prePage, &QAction::triggered, this,[=](){
        emit this->actionSignals("prePage");
    });
    connect(nextPage, &QAction::triggered, this,[=](){
        emit this->actionSignals("nextPage");
    });

    // 添加状态栏
    QStatusBar * statusBar = this->statusBar();
    this->setStatusBar(statusBar);
    // 在状态栏显示当前的信息
    this->m_DbType = new QLabel("未选择",this);
    this->m_DbAddress = new QLabel("未知",this);
    this->m_DbConnectStatus = new QLabel("未连接",this);
    statusBar->addWidget(this->m_DbType);
    statusBar->addWidget(this->m_DbAddress);
    statusBar->addWidget(this->m_DbConnectStatus);

    // 添加自定义中心窗口
    CenterWidget * centerWindow = new CenterWidget();
    centerWindow->setParent(this);
    this->setCentralWidget(centerWindow);

    // 翻页信号与中心窗口槽函数连接
    connect(this, &MainWindow::actionSignals, centerWindow, &CenterWidget::handlerPageChanged);

}

// 连接数据库
void MainWindow::configDataBase(QMap<QString, QString> *map)
{
    this->m_DbConnectStatus->setText("正在连接...");
    // 移除原有的所有连接
    QStringList cnames = QSqlDatabase::database().connectionNames();
    for (int i = 0; i < cnames.size(); i++) {
        QSqlDatabase::removeDatabase(cnames.at(i));
    }
    QString host = map->value("host");
    QString port = map->value("port");
    QString username = map->value("userName");
    QString password = map->value("password");
    QString dbType = map->value("dbType");
    int portInt = port.toInt();
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName(host);
    db.setPort(portInt);
    db.setUserName(username);
    db.setPassword(password);
    // 连接数据库
    this->connectedDatabase();
    db.close();  // 关掉连接
    this->m_DbConnectStatus->setText("连接成功!");
    this->m_DbAddress->setText(host);
    //qDebug() << QString("host=%1 usetname=%2 password=%3 dbType=%4").arg(host).arg(username).arg(password).arg(dbType);
}


bool MainWindow::connectedDatabase()
{
    QSqlQuery query;
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen())
    {
        //qDebug() << "打开数据库";
        db.open();
    }
    query.exec("show databases;");
    QStringList dbNames;
    while (query.next()) {
        dbNames << query.value(0).toString();
    }
    // 添加左侧的数据库名称列表
    QWidget *centralWidget = this->centralWidget();
    CenterWidget * centralw = dynamic_cast<CenterWidget *>(centralWidget);
    centralw->setDbNameList(dbNames);
    return true;
}
MainWindow::~MainWindow()
{
}
