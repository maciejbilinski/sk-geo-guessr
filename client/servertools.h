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
    Q_INVOKABLE void vote(QString player, QString team);

    enum CLIENT_STATE {
        ERROR = -1, // error occurs
        INTRO = 0, // in intro view
        VOTING = 1, // in voting view
        ADMIN_PANEL = 2,
        GAME = 3,
        STATUS = 4,
        WAIT_FOR_GAME = 5, // game is already started
        WAIT_FOR_RANKING = 6
    };
    Q_ENUM(CLIENT_STATE)

    Q_PROPERTY(CLIENT_STATE state MEMBER _state NOTIFY stateChanged);
    Q_PROPERTY(QVector<QString> players MEMBER _players NOTIFY playersChanged);
    Q_PROPERTY(QVector<QString> ranking MEMBER _ranking NOTIFY rankingChanged);

signals:
    void stateChanged(ServerTools::CLIENT_STATE);
    void playersChanged();
    void rankingChanged();
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
    QVector<QString> _players;
    QVector<QString> _ranking;

};

#endif // SERVERTOOLS_H
