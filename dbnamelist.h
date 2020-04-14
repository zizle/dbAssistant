#ifndef DBNAMELIST_H
#define DBNAMELIST_H

#include <QListWidget>

class DBNameList : public QListWidget
{
    Q_OBJECT
public:
    explicit DBNameList(QWidget *parent = nullptr);
    bool m_IsConnected = false;

signals:
    void nameListActionSignal(QString action, QString dbName);

public slots:
};

#endif // DBNAMELIST_H
