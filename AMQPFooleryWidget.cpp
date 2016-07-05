#include "AMQPFooleryWidget.h"
#include "ui_AMQPFooleryWidget.h"

#include <QHostInfo>
#include <QtDebug>

AMQPFooleryWidget::AMQPFooleryWidget(QWidget *parent) :
  QWidget(parent)
  , ui(new Ui::AMQPFooleryWidget)
  , RabbitClient(new QAMQP::Client(this))
  , Exchange(NULL)
  , MyQueue(NULL)
  , CurrentRoutingKey(QString(""))
  , CurrentExchangeName(QString(""))
  , CurrentQueue(NULL)
  , CurrentMessage(NULL)
{
    ui->setupUi(this);
    connect(ui->ConnectPushButton, SIGNAL(clicked()), this, SLOT(ConnectToServer()));
    connect(ui->BindPushButton, SIGNAL(clicked()), this, SLOT(BindQueue()));
    connect(ui->ACKPushButton, SIGNAL(clicked()), this, SLOT(HandleACK()));

    connect(RabbitClient, SIGNAL(connected()), this, SLOT(HandleConnected()));
    connect(RabbitClient, SIGNAL(disconnected()), this, SLOT(HandleDisconnected()));

}

AMQPFooleryWidget::~AMQPFooleryWidget()
{
    delete MyQueue;
    MyQueue = NULL;

    delete Exchange;
    Exchange = NULL;

    delete RabbitClient;
    RabbitClient = NULL;

    delete ui;
    ui = NULL;
}

void AMQPFooleryWidget::on_AutoACKCheckBox_stateChanged(int arg1)
{
    ui->ACKPushButton->setDisabled(arg1);
}

void AMQPFooleryWidget::ConnectToServer()
{
    QString url = "amqp://guest:guest@" + ui->ServerAddressLineEdit->text() + ":5672/";
    qDebug() << "Connecting to " << url;
    RabbitClient->open(QUrl(url));
}

void AMQPFooleryWidget::BindQueue()
{

    if(CurrentRoutingKey != "")
    {
        MyQueue->unbind(CurrentExchangeName, CurrentRoutingKey);
    }

    Exchange = RabbitClient->createExchange(ui->ExchangeLineEdit->text());
    CurrentExchangeName = ui->ExchangeLineEdit->text();

    if(ui->QueueNameLineEdit->text() != "")
    {
        MyQueue = RabbitClient->createQueue();
        MyQueue->declare(ui->QueueNameLineEdit->text(), QAMQP::Queue::Durable);
    }
    else
    {
        MyQueue = RabbitClient->createQueue(QHostInfo::localHostName() + "_" + QString::number(QApplication::applicationPid()));

        MyQueue->bind(Exchange, ui->RoutingKeyLineEdit->text());
        MyQueue->declare(MyQueue->name(), QAMQP::Queue::AutoDelete);

        CurrentRoutingKey = ui->RoutingKeyLineEdit->text();
    }

    connect(MyQueue, SIGNAL(declared()), this, SLOT(HandleQueueDeclared()));
    connect(MyQueue, SIGNAL(messageReceived(QAMQP::Queue*)), this, SLOT(MessageReceived(QAMQP::Queue *)));


}

void AMQPFooleryWidget::HandleConnected()
{
    ui->BindPushButton->setEnabled(true);
}

void AMQPFooleryWidget::HandleDisconnected()
{
    ui->BindPushButton->setEnabled(false);
}

void AMQPFooleryWidget::MessageReceived(QAMQP::Queue *queue)
{

    qDebug() << "MessageReceived";
    qDebug() << queue->hasMessage();

    while(queue->hasMessage())
    {
        QString outMessage = "";

        QAMQP::MessagePtr message = queue->getMessage();

        outMessage += "Exchange Name: " + message->exchangeName + "\n";
        outMessage += "Routing Key:   " + message->routeKey + "\n";

        if(message->headers.count() > 0)
        {
            outMessage += "Headers:\n";
            QHashIterator<QString, QVariant> i(message->headers);
            while(i.hasNext())
            {
                i.hasNext();
                outMessage += "   " + i.key() + " => " + i.value().toString() + "\n";
            }
        }

        outMessage += "Message:\n";
        outMessage += message->payload + "\n";

        ui->plainTextEdit->appendPlainText(outMessage);

        if(ui->AutoACKCheckBox->checkState() == Qt::Checked)
        {
            queue->ack(message);
        }
        else
        {
            CurrentQueue = queue;
            CurrentMessage = message;
            ui->ACKPushButton->setEnabled(true);
        }
    }
}

void AMQPFooleryWidget::HandleQueueDeclared()
{
    MyQueue->setQOS(0,1);
    MyQueue->consume();
}

void AMQPFooleryWidget::HandleACK()
{
    CurrentQueue->ack(CurrentMessage);
    ui->ACKPushButton->setEnabled(false);
}
