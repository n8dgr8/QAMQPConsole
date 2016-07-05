#-------------------------------------------------
#
# Project created by QtCreator 2014-03-03T21:30:20
#
#-------------------------------------------------

QT       += core gui

TARGET = AMQP_Foolery
TEMPLATE = app


SOURCES += main.cpp\
        AMQPFooleryWidget.cpp

HEADERS  += AMQPFooleryWidget.h

FORMS    += AMQPFooleryWidget.ui

include(qamqp/qamqp.pri)
