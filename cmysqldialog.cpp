#include "cmysqldialog.h"
#include <QDebug>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QIntValidator>

CmysqlDialog::CmysqlDialog(QWidget *parent) : QDialog(parent)
{
    this->setWindowTitle("连接MySQL");
    this->setFixedSize(330,200);
    this->setAttribute(Qt::WA_DeleteOnClose);  // 关闭后释放空间
    QLabel * hostLabel = new QLabel("地址:", this);
    this->m_HostEdit = new QLineEdit("127.0.0.1");
    this->m_HostEdit->setPlaceholderText("127.0.0.1");
    this->m_HostEdit->setParent(this);
    QLabel * portLabel = new QLabel("端口:", this);
    this->m_Port = new QLineEdit("3306", this);
    this->m_HostEdit->setPlaceholderText("127.0.0.1");
    this->m_Port->setValidator(new QIntValidator());

    QLabel * uNameLabel = new QLabel("用户", this);
    this->m_UserName = new QLineEdit("root", this);
    this->m_UserName->setPlaceholderText("用户名: root");
    QLabel * passwordLabel = new QLabel("密码", this);
    this->m_Password = new QLineEdit(this);
    this->m_Password->setPlaceholderText("密码: xxx");
    QGridLayout * mainLayout = new QGridLayout(this);
    mainLayout->addWidget(hostLabel,0,0);
    mainLayout->addWidget(this->m_HostEdit,0,1);

    mainLayout->addWidget(portLabel, 1,0);
    mainLayout->addWidget(this->m_Port, 1,1);

    mainLayout->addWidget(uNameLabel,2,0);
    mainLayout->addWidget(this->m_UserName,2,1);

    mainLayout->addWidget(passwordLabel,3,0);
    mainLayout->addWidget(this->m_Password,3,1);
    this->setLayout(mainLayout);

    // 确定和取消按钮b
    QWidget * btnsWidget = new QWidget(this);
    QHBoxLayout * btnLayout = new QHBoxLayout(btnsWidget);
    btnLayout->setContentsMargins(QMargins(0,0,0,0));
    btnLayout->setSpacing(3);
    btnsWidget->setLayout(btnLayout);

    QPushButton * makeSureBtn = new QPushButton("确定", this);
    QPushButton * cancelBtn = new QPushButton("取消", this);
    btnLayout->addWidget(makeSureBtn);
    btnLayout->addWidget(cancelBtn);
    mainLayout->addWidget(btnsWidget, 4,1);

    // 确定;取消的信号
    connect(makeSureBtn, &QPushButton::clicked, this, [=](){
        this->m_HostEdit->text();
        if (this->m_HostEdit->text().trimmed().isEmpty() ||
            this->m_UserName->text().trimmed().isEmpty() ||
            this->m_Password->text().trimmed().isEmpty())
        {
            return;
        }
        QMap<QString, QString> map;
        map["host"] = this->m_HostEdit->text();
        map["userName"] = this->m_UserName->text();
        map["password"] = this->m_Password->text();
        map["dbType"] = "MySQL";
        emit this->makeDBConnection(&map);  // 发出参数信号
        this->close();
    });
    connect(cancelBtn, &QPushButton::clicked,this,[=](){
        this->close();
    });
}
