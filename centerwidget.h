#ifndef CENTERWIDGET_H
#define CENTERWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QComboBox>
#include <QPushButton>
#include <QTableWidget>
#include <QPoint>
#include "dbnamelist.h"

class CenterWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CenterWidget(QWidget *parent = nullptr);
    void setDbNameList(QStringList dbNames);
    void insertRecord();
    void updateRecord();
    void deleteRecord();
    void prePageRecords();
    void nextPageRecords();

    DBNameList * m_DbNameList = NULL;  // 左侧数据库名列表
    QComboBox * m_TableComboBox = NULL;  // 数据库中标的名称下拉框
    QPushButton * m_DeleteTableBtn = NULL; // 删除当前表的按钮
    QTableWidget * m_RecordsTable = NULL;  // 显示数据的表
    int m_CurrentRow = 0;
    int m_TotalRow = 0;
    int m_SelectedRow = 0;
signals:
    QString contextActionSignal(QString action);

public slots:
    void handlerNameListAction(QString action, QString dbName);
    void handlerTableNameChanged(QString tbname);
    void handlerPageChanged(QString toolAction);// 工具栏槽函数
};

#endif // CENTERWIDGET_H
