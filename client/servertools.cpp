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
    _socket.write(("action:team;content:" + team + ";\n").toLocal8Bit());

}

void ServerTools::sendPhoto(QString photo, double latitude, double longitude)
{
    _socket.write(("Round: " + photo + ";Coords: " + QString::number(latitude) + ";" + QString::number(longitude) + "\n").toLocal8Bit());
}

void ServerTools::sendAnswer(double latitude, double longitude)
{
    _socket.write(("Answer: " + QString::number(_round) + ";Coords: " + QString::number(latitude) + ";" + QString::number(longitude) + "\n").toLocal8Bit());
}

void ServerTools::connected()
{
    qDebug() << "connected...";
    _socket.write(("content:"+ this->name + ";action:player_intro;\n").toLocal8Bit());
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
        }else if(data == "vote_accepted"){
            _state = CLIENT_STATE::WAIT_FOR_GAME;
            emit stateChanged(_state);
        }else if(data.contains("action:new_player;content:")){ // po polaczeniu serwer powinien do nowego gracza wyslac kilka takich komunikatow
            QString playerName = data.mid(26);
            _players.append(playerName);
            emit playersChanged();
        }else if(data.contains("del player: ")){
            QString playerName = data.mid(12);
            _players.removeAll(playerName);
            emit playersChanged();
        }else if(data == "ok vote"){
            _state = CLIENT_STATE::WAIT_FOR_RANKING;
            emit stateChanged(_state);
        }else if(data.contains("ranking: ")){
            QString rank = data.mid(9);
            QStringList ranks = rank.split( ";" );
            _ranking.clear();
            for(int j=0; j<ranks.size(); j++){
                QString r = ranks.at(j);
                _ranking.append(r);
            }
            emit rankingChanged();
        }else if(data.contains("game start: ")){
            _round = 1;
            emit roundChanged();
            QString role = data.mid(12);
            if(role == "host"){
                _state = CLIENT_STATE::ADMIN_PANEL;
                emit stateChanged(_state);
            }else if(role.contains("game;")){
                QStringList subdata = role.mid(5).split(";");
                _photoURL = subdata[0];
                _timeLeft = (subdata[1].toLongLong()-QDateTime::currentMSecsSinceEpoch())/1000;
                timer.stop();
                timer.start();
                _state = CLIENT_STATE::GAME;
                emit timeLeftChanged();
                emit photoURLChanged();
                emit stateChanged(_state);
            }
        }else if(data.contains("new round: ")){
            QString round = data.mid(11);
            QStringList roundData = round.split(";");
            _round = roundData[0].toInt();
            _answers.clear();
            _photoURL = roundData[1];
            _timeLeft = (roundData[2].toLongLong()-QDateTime::currentMSecsSinceEpoch())/1000;
            timer.stop();
            timer.start();
            emit timeLeftChanged();
            emit photoURLChanged();
            emit answersChanged();
            emit roundChanged();
        }else if(data.contains("new answer: ")){
            QString answer = data.mid(12);
            QStringList answerData = answer.split(";");
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
