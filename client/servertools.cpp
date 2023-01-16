#include "servertools.h"
#include <QDebug>
#include <QHostAddress>
ServerTools::ServerTools(QObject* parent) : QObject(parent)
{
    _state = CLIENT_STATE::INTRO;
}

void ServerTools::connect(QString name, int port)
{
    if(_state <= 0){
        this->name = name;
        QObject::connect(&_socket, SIGNAL(connected()),this, SLOT(connected()));
        QObject::connect(&_socket, SIGNAL(disconnected()),this, SLOT(disconnected()));
        QObject::connect(&_socket, SIGNAL(bytesWritten(qint64)),this, SLOT(bytesWritten(qint64)));
        QObject::connect(&_socket, SIGNAL(readyRead()),this, SLOT(readyRead()));
        QObject::connect(&_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError()));
        _socket.connectToHost(QHostAddress("127.0.0.1"), port);
    }else{
        qDebug() << "Server already connected!";
    }
}

void ServerTools::quit()
{
    if(_state <= 0){
        this->name = "";
        _socket.abort();
        _socket.close();
    }
    _state = CLIENT_STATE::INTRO;
    emit stateChanged(_state);

}

void ServerTools::connected()
{
    qDebug() << "connected...";
    _socket.write(("New player: " + this->name + "\n").toLocal8Bit());
}

void ServerTools::disconnected()
{
    qDebug() << "disconnected...";

    _state = CLIENT_STATE::ERROR;
    emit stateChanged(_state);
}

void ServerTools::bytesWritten(qint64 bytes)
{
    qDebug() << bytes << " bytes written...";
}

void ServerTools::readyRead()
{
    qDebug() << "reading...";
    QString data(_socket.readAll());
    qDebug() << data;

    if(data == "add player"){
        _state = CLIENT_STATE::VOTING;
        emit stateChanged(_state);
    }else if(data == "waiting for game"){
        _state = CLIENT_STATE::WAIT_FOR_GAME;
        emit stateChanged(_state);
    }
}

void ServerTools::socketError()
{
    qDebug() << "error...";

    _socket.abort();
    _socket.close();
    _state = CLIENT_STATE::ERROR;
    emit stateChanged(_state);
}
