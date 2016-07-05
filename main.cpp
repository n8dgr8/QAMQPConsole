#include <QtGui/QApplication>
#include "AMQPFooleryWidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setStyle("GTK");
    AMQPFooleryWidget w;
    w.show();
    
    return a.exec();
}
