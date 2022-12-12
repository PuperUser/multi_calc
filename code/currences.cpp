#include "currences.h"
//#include <QXmlQuery>
#include <QTextCodec>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <qdebug.h>


Currences::Currences()
{
    struct CurInfo
    {
        QString Code;
        QString Name;
        QString Country;
    };
    QVector<CurInfo> CurrencesInfo;

    QFile inFile(":/res/currences.json");
    inFile.open(QIODevice::ReadOnly|QIODevice::Text);
    QByteArray data = inFile.readAll();
    inFile.close();

    QJsonParseError errorPtr;
    QJsonDocument doc = QJsonDocument::fromJson(data, &errorPtr);
    if (doc.isNull()) {
        qDebug() << "Parse failed";
    } else {
        QJsonObject rootObj = doc.object();
        auto m = rootObj["data"].toArray();
        for (auto r: m)
        {
            auto o = r.toObject().toVariantMap();
            CurrencesInfo.push_back({o["code"].toString(), o["name"].toString(), o["country"].toString()});

        }
        qDebug() << CurrencesInfo.size();
    }

    QFile tokenFile(":/res/exchangerate-api");
    tokenFile.open(QIODevice::ReadOnly|QIODevice::Text);
    EXCHANGERATE_API_TOKEN = QString(tokenFile.readAll()).trimmed();
    tokenFile.close();
}

void Currences::load()
{
    qDebug() << "Try load...";
    const QString url = "https://v6.exchangerate-api.com/v6/"+ EXCHANGERATE_API_TOKEN +"/latest/USD";
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)),
              this, SLOT(replyFinished(QNetworkReply*)));

    //connect(reply, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
    auto reply = manager->get(request);
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),this, SLOT(slotError(QNetworkReply::NetworkError)));

}

double Currences::rate(int i)
{
    return i<Rates.size()? Rates[i].Rate / Rates[BaseIndex].Rate: 0;

}

QString Currences::name(int i)
{
    return i<Rates.size()? Rates[i].Name: 0;
}

void Currences::replyFinished(QNetworkReply *r)
{
    QString answer = r->readAll();
    QJsonDocument jsonResponse = QJsonDocument::fromJson(answer.toUtf8());
    QJsonObject jsonObject = jsonResponse.object();

    if (QString result = jsonObject["result"].toString(); result == "success")
    {
         QMap<QString, QVariant> conversionRates = jsonObject["conversion_rates"].toObject().toVariantMap();


         QVector<QString> query = {"USD", "RUB", "TRY", "EUR"};

         for(auto &q: query)
         {
             qDebug() << conversionRates[q].toDouble();
             Rates.push_back({conversionRates[q].toDouble(), q});

         }
         emit refreshFinished();
    } else
        qDebug() << result;

}

void Currences::slotReadyRead()
{

}

void Currences::slotError(QNetworkReply::NetworkError err)
{
    qDebug() << "NetworkError" << err;
}
