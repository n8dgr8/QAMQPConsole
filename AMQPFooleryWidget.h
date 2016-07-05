#ifndef AMQPFOOLERYWIDGET_H
#define AMQPFOOLERYWIDGET_H

#include "qamqp/amqp.h"
#include "qamqp/amqp_queue.h"
#include "qamqp/amqp_exchange.h"

#include <QWidget>

namespace Ui {
class AMQPFooleryWidget;
}

class AMQPFooleryWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit AMQPFooleryWidget(QWidget *parent = 0);
    ~AMQPFooleryWidget();
    
private slots:
    void on_AutoACKCheckBox_stateChanged(int arg1);
    void ConnectToServer();
    void HandleConnected();
    void HandleDisconnected();
    void MessageReceived(QAMQP::Queue * queue);
    void HandleQueueDeclared();
    void HandleACK();
    void BindQueue();

private:
    Ui::AMQPFooleryWidget *ui;
    QAMQP::Client *RabbitClient;
    QAMQP::Exchange *Exchange;
    QAMQP::Queue *MyQueue;

    QString CurrentRoutingKey;
    QString CurrentExchangeName;

    QAMQP::Queue *CurrentQueue;
    QSharedPointer<QAMQP::Message> CurrentMessage;


};

#endif // AMQPFOOLERYWIDGET_H
