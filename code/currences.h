#ifndef CURRENCES_H
#define CURRENCES_H
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>

class Currences : public QObject
{
Q_OBJECT
public:
    Currences();
    void load();
    double rate(int i);
    QString name(int i);
    void setBase(int i) { BaseIndex = i;}

private:
    struct RateInfo
    {
        double  Rate;
        QString Name;
    };
    QVector<RateInfo> Rates;
    int BaseIndex = 0;
    QString EXCHANGERATE_API_TOKEN;


public slots:
    void replyFinished(QNetworkReply *reply);
    void slotReadyRead();
    void slotError(QNetworkReply::NetworkError);

signals:
    void refreshFinished();
};

#endif // CURRENCES_H
