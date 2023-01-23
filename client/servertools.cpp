#include "servertools.h"
#include <QDebug>
#include <QHostAddress>
#include <QDateTime>
ServerTools::ServerTools(QObject* parent) : QObject(parent), timer(this)
{
    _state = CLIENT_STATE::INTRO;
    timer.setInterval(1000);
    QObject::connect(&timer, SIGNAL(timeout()), this, SLOT(updateTimeLeft()));

}

void ServerTools::connect(QString name, int port)
{
    if(_state <= 0){
        this->name = name;
        emit meChanged();
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
        emit meChanged();
        _socket.abort();
        _socket.close();
        _players.clear();
        emit playersChanged();
    }
    _state = CLIENT_STATE::INTRO;
    emit stateChanged(_state);

}

void ServerTools::vote(QString player, QString team)
{
    _team = team;
    _socket.write(("action:vote;content:" + player + ";\n").toLocal8Bit());
    qDebug() << "SENT action:vote;content:" + player + ";\n";
    _socket.write(("action:team;content:" + team + ";\n").toLocal8Bit());
    qDebug() << "SENT action:team;content:" + team + ";\n";

}

void ServerTools::sendPhoto(QString photo, double latitude, double longitude)
{
    _state = CLIENT_STATE::WAIT_FOR_RANKING;
    emit stateChanged(_state);
    _socket.write(("action:host_place;content:" + photo + " " + QString::number(latitude) + " " + QString::number(longitude) + " ;\n").toLocal8Bit());
    qDebug() << "SENT action:host_place;content:" + photo + " " + QString::number(latitude) + " " + QString::number(longitude) + " ;\n";
}

void ServerTools::sendAnswer(double latitude, double longitude)
{
    _socket.write(("action:set_place;content:" + QString::number(latitude) + " " + QString::number(longitude) + " ;\n").toLocal8Bit());
    qDebug() << "SENT action:set_place;content:" + QString::number(latitude) + " " + QString::number(longitude) + " ;\n";
}

void ServerTools::connected()
{
    qDebug() << "connected...";
    _socket.write(("content:"+ this->name + ";action:player_intro;\n").toLocal8Bit());
    qDebug() << "SENT content:"+ this->name + ";action:player_intro;\n";
}

void ServerTools::disconnected()
{
    qDebug() << "disconnected...";

    if(_state != CLIENT_STATE::NAME_EXISTS){
        qDebug() << "error state" << _state;
        _state = CLIENT_STATE::ERROR;
        emit stateChanged(_state);
    }
}

void ServerTools::bytesWritten(qint64 bytes)
{
    qDebug() << bytes << " bytes written...";
}

void ServerTools::readyRead()
{
    qDebug() << "reading...";
    QString d(_socket.readAll());
    QStringList pieces = d.split( "\n" );
    for(int i=0; i<pieces.size(); i++){
        QString data = pieces.at(i);
        qDebug() << data;
        if(data.contains("action:player_intro;content:ok")){
            _state = CLIENT_STATE::VOTING;
            _players.append(name);
            emit playersChanged();
            emit stateChanged(_state);
        }else if(data.contains("action:player_intro;content:game_started")){
            _state = CLIENT_STATE::GAME_STARTED;
            emit stateChanged(_state);
        }else if(data.contains("action:new_player;content:")){ // po polaczeniu serwer powinien do nowego gracza wyslac kilka takich komunikatow
            QString playerName = data.mid(26);
            _players.append(playerName);
            emit playersChanged();
        }else if(data.contains("action:player_disconnected;content:")){
            QString playerName = data.mid(35);
            _players.removeAll(playerName);
            emit playersChanged();
        }else if(data == "action:player_vote;content:ok"){
            _ranking.clear();
            emit rankingChanged();
            _state = CLIENT_STATE::WAIT_FOR_RANKING;
            emit stateChanged(_state);
        }else if(data == "action:voting_failed;content:"){
            _ranking.clear();
            emit rankingChanged();
            _state = CLIENT_STATE::VOTING;
            emit stateChanged(_state);
        }else if(data.contains("action:ranking;content:")){
            QString rank = data.mid(23);
            QStringList ranks = rank.split( " " );
            _ranking.clear();
            for(int j=0; j<ranks.size()-1; j++){
                QString r = ranks.at(j);
                _ranking.append(r);
            }
            emit rankingChanged();
            if(_state == CLIENT_STATE::GAME){
                _state = CLIENT_STATE::WAIT_FOR_RANKING;
                emit stateChanged(_state);
            }

        }else if(data.contains("action:host;content:")){
            _round = 1;
            emit roundChanged();
            QString user = data.mid(20);
            if(user == name){
                _state = CLIENT_STATE::ADMIN_PANEL;
                emit stateChanged(_state);
            }
        }else if(data.contains("action:round;content:")){
            _round = data.mid(21).toInt();
            emit roundChanged();
            _state = CLIENT_STATE::ADMIN_PANEL;
            emit stateChanged(_state);
        }else if(data.contains("action:place;content:")){
            QStringList subdata = data.mid(21).split(" ");
            _round = subdata[0].toInt();
            _answers.clear();
            _photoURL = subdata[1];
            _timeLeft = (subdata[2].toLongLong()-QDateTime::currentMSecsSinceEpoch())/1000;
            timer.stop();
            timer.start();
            _state = CLIENT_STATE::GAME;
            emit timeLeftChanged();
            emit photoURLChanged();
            emit stateChanged(_state);
            emit answersChanged();
            emit roundChanged();
        }else if(data.contains("action:user_set_place;content:")){
            QString answer = data.mid(30);
            QStringList answerData = answer.split(",");
            _answers[answerData[0]] = QPointF(answerData[1].toDouble(), answerData[2].toDouble());
            emit answersChanged();
        }else if(data.contains("action:error;content:")){
            QString content = data.mid(13+8);
            if(content == "name_exists"){
                _state = CLIENT_STATE::NAME_EXISTS;
                emit stateChanged(_state);
            }
        }
    }
}

void ServerTools::socketError()
{
    qDebug() << "error...";

    _socket.abort();
    _socket.close();
    _players.clear();
    if(_state != CLIENT_STATE::NAME_EXISTS){
        qDebug() << "error state" << _state;
        _state = CLIENT_STATE::ERROR;
        emit stateChanged(_state);
    }
    emit playersChanged();
}

void ServerTools::updateTimeLeft()
{
    _timeLeft--;
    emit timeLeftChanged();
    if(_timeLeft == 0){
        timer.stop();
    }
}
