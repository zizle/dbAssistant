#ifndef CMYSQLDIALOG_H
#define CMYSQLDIALOG_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QDialog>
#include <QMap>

class CmysqlDialog : public QDialog
{
    Q_OBJECT
public:
    explicit CmysqlDialog(QWidget *parent = nullptr);
    bool connectingDB();

    QLineEdit * m_HostEdit = NULL; // HOST
    QLineEdit * m_UserName = NULL; // username
    QLineEdit * m_Password = NULL; // password
    QLineEdit * m_Port = NULL;

signals:
    void makeDBConnection(QMap<QString, QString> *);

public slots:
};

#endif // CMYSQLDIALOG_H
