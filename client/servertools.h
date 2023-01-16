#ifndef SERVERTOOLS_H
#define SERVERTOOLS_H

#include <QObject>
#include <QTcpSocket>

class ServerTools : public QObject
{
    Q_OBJECT
public:
    explicit ServerTools(QObject* parent = nullptr);

    Q_INVOKABLE void connect(QString name, int port);
    Q_INVOKABLE void quit();

    enum CLIENT_STATE {
        ERROR = -1, // error occurs
        INTRO = 0, // in intro view
        VOTING = 1, // in voting view
        ADMIN_PANEL = 2,
        GAME = 3,
        STATUS = 4,
        WAIT_FOR_GAME = 5 // game is already started
    };
    Q_ENUM(CLIENT_STATE)

    Q_PROPERTY(CLIENT_STATE state MEMBER _state NOTIFY stateChanged)

signals:
    void stateChanged(CLIENT_STATE);
public slots:
    void connected();
    void disconnected();
    void bytesWritten(qint64 bytes);
    void readyRead();
    void socketError();

private:
    QTcpSocket  _socket;
    QString name;
    CLIENT_STATE _state;
};

#endif // SERVERTOOLS_H
