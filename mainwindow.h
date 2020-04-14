#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QSqlDatabase>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    void configDataBase(QMap<QString, QString> *map);
    bool connectedDatabase();

    ~MainWindow();
    QLabel * m_DbType = NULL;
    QLabel * m_DbAddress = NULL;
    QLabel * m_DbConnectStatus = NULL;
signals:
    void actionSignals(QString actionName);

private:

};

#endif // MAINWINDOW_H，主窗口
