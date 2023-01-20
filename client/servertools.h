#ifndef SERVERTOOLS_H
#define SERVERTOOLS_H

#include <QObject>
#include <QTcpSocket>
#include <QMap>
#include <QPointF>
#include <QTimer>

class ServerTools : public QObject
{
    Q_OBJECT
public:
    explicit ServerTools(QObject* parent = nullptr);

    Q_INVOKABLE void connect(QString name, int port);
    Q_INVOKABLE void quit();
    Q_INVOKABLE void vote(QString player, QString team);
    Q_INVOKABLE void sendPhoto(QString photo, double latitude, double longitude);
    Q_INVOKABLE void sendAnswer(double latitude, double longitude);

    enum CLIENT_STATE {
        ERROR = -1, // error occurs
        INTRO = 0, // in intro view
        VOTING = 1, // in voting view
        ADMIN_PANEL = 2,  // in game as host
        GAME = 3,
        STATUS = 4,
        WAIT_FOR_GAME = 5, // game is already started
        WAIT_FOR_RANKING = 6
    };
    Q_ENUM(CLIENT_STATE)

    Q_PROPERTY(CLIENT_STATE state MEMBER _state NOTIFY stateChanged);
    Q_PROPERTY(QVector<QString> players MEMBER _players NOTIFY playersChanged);
    Q_PROPERTY(QVector<QString> ranking MEMBER _ranking NOTIFY rankingChanged);        
    Q_PROPERTY(int round MEMBER _round NOTIFY roundChanged);
    Q_PROPERTY(QString me MEMBER name NOTIFY meChanged);
    Q_PROPERTY(QVariantMap answers MEMBER _answers NOTIFY answersChanged);
    Q_PROPERTY(QString photoURL MEMBER _photoURL NOTIFY photoURLChanged);
    Q_PROPERTY(int timeLeft MEMBER _timeLeft NOTIFY timeLeftChanged);

signals:
    void stateChanged(ServerTools::CLIENT_STATE);
    void playersChanged();
    void rankingChanged();
    void roundChanged();
    void meChanged();
    void answersChanged();
    void photoURLChanged();
    void timeLeftChanged();

public slots:
    void connected();
    void disconnected();
    void bytesWritten(qint64 bytes);
    void readyRead();
    void socketError();
private slots:
    void updateTimeLeft();

private:
    QTcpSocket  _socket;
    QString name;
    CLIENT_STATE _state;
    QVector<QString> _players;
    QVector<QString> _ranking;
    QString _team;
    int _round;
    QVariantMap _answers;
    QString _photoURL;
    int _timeLeft;


    QTimer timer;
};

#endif // SERVERTOOLS_H
