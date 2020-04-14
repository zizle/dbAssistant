#include "dbnamelist.h"
#include <QDebug>
#include <QMenu>
#include <QCursor>
#include <QSqlDatabase>

DBNameList::DBNameList(QWidget *parent) : QListWidget(parent)
{
    this->setContextMenuPolicy(Qt::CustomContextMenu);  // 鼠标右击事件
    // 右键菜单
    connect(this, &DBNameList::customContextMenuRequested, this,[=](){
        QMenu * menuContxtMenu = new QMenu;
        if (!this->m_IsConnected)
        {
            return;
        }
        if (this->itemAt(mapFromGlobal(QCursor::pos()))!=NULL)
        {
            //qDebug() << "选择了选项";
            QString dbName = this->currentItem()->text();
            QAction * useAction = menuContxtMenu->addAction("连接");
            QAction * deleteAction = menuContxtMenu->addAction("删除");
            useAction->setParent(menuContxtMenu);
            deleteAction->setParent(menuContxtMenu);

            connect(useAction, &QAction::triggered, this, [=](){
                emit this->nameListActionSignal("connect", dbName);
            });
            connect(deleteAction, &QAction::triggered, this, [=](){
                emit this->nameListActionSignal("delete",dbName);
            });
        }
        else
        {
           QAction * addNew = menuContxtMenu->addAction("新增");
           addNew->setParent(menuContxtMenu);
           connect(addNew, &QAction::triggered, this, [=](){
               emit this->nameListActionSignal("addNew","");
           });
        }
        menuContxtMenu->exec(QCursor::pos());
        delete menuContxtMenu;
        menuContxtMenu= NULL;
    });
}
