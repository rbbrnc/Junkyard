#include "elasticsearch.h"
#include <QDebug>
#include <QJsonParseError>
#include <QStringList>
#include <QRegExp>

// INDICES
//curl -XGET 'http://127.0.0.1:9200/_cat/indices?v&h=index'
//curl -XGET 'localhost:9200/_stats/'
//curl -XGET 'localhost:9200/_stats/indices'
//curl http://localhost:9200/_aliases

ElasticSearch::ElasticSearch(QNetworkAccessManager *mgr, const QString &host)
{
    m_baseUrl    = host;
    m_netManager = mgr;
    m_ready = true;
}

ElasticSearch::~ElasticSearch()
{
}

bool ElasticSearch::isReady() const
{
    return m_ready;
}

void ElasticSearch::debugNetResponse(QNetworkReply *nr)
{
    QVariant httpCode = nr->attribute(QNetworkRequest::HttpStatusCodeAttribute);

    qDebug() << __PRETTY_FUNCTION__
             << "\nUrl:" << nr->request().url().toString()
             << "\nHTTP Code:" << ((httpCode.isValid()) ? QString::number(httpCode.toInt()) : "Invalid!")
             << nr->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString()
             << "\nNetwork Reply Error:" << nr->errorString() << "\n";

    qDebug() << "Reply text:" << nr->readAll();
}


QString ElasticSearch::host() const
{
    return m_baseUrl;
}

void ElasticSearch::setHost(const QString &baseUrl)
{
    m_baseUrl = baseUrl;
}

//////////////////////////////////////////////////////////////////////////////
///
/// Query/Parse elasticsearch '_mappings' data
///
//////////////////////////////////////////////////////////////////////////////
void ElasticSearch::mappings(bool reload, const QString &index, const QString &type)
{
    // Fix this!!!
    Q_UNUSED(index)
    Q_UNUSED(type)


    if (reload || m_indicesMap.isEmpty()) {
        QString mreq;
//        if (index.isEmpty()) {
            //curl -XGET 'http://localhost:9200/_mapping/'
            mreq = "_mapping/";
//        } else {
            //curl -XGET 'http://localhost:9200/{index}/_mapping/{type}'
//            mreq = QString("%1/_mapping/%2").arg(index).arg(type);
//        }
        getRequest(mreq, ElasticSearch::MappingRequest);
    } else {
        emit mappingsDataReady();
    }
}

void ElasticSearch::parseMappings(const QJsonDocument &data)
{
    m_indicesMap.clear();

    m_indicesMap["_all_"] = QStringList();

    QJsonObject rootObj = data.object();
    QStringList indices = rootObj.keys();

    for (int i = 0; i < indices.size(); i++) {
        QJsonObject indexObj = rootObj[indices.at(i)].toObject();
        QJsonObject mapObj   = indexObj["mappings"].toObject();

        m_indicesMap[indices.at(i)] = mapObj.keys();
    }
    //qDebug() << m_indicesMap;
    emit mappingsDataReady();
}

QHash<QString, QStringList> ElasticSearch::indicesMap() const
{
    return m_indicesMap;
}

//////////////////////////////////////////////////////////////////////////////
/// \brief Generic GET Request
/// \param request
/// \param type
//////////////////////////////////////////////////////////////////////////////
void ElasticSearch::getRequest(const QString &request, ElasticSearch::RequestType type)
{
    m_ready = false;

    QNetworkRequest nreq(QUrl(m_baseUrl + request));
    nreq.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QNetworkReply *reply = m_netManager->get(nreq);

    // Set property to know what type of request is!
    reply->setProperty("type", QVariant(type));

    connect(reply, SIGNAL(finished()), this, SLOT(requestFinished()));
}

//////////////////////////////////////////////////////////////////////////////
/// \brief Generic POST Request
/// \param request
/// \param postData
/// \param type
//////////////////////////////////////////////////////////////////////////////
void ElasticSearch::postRequest(const QString &request, const QByteArray &postData, ElasticSearch::RequestType type)
{
    m_ready = false;

    QNetworkRequest nreq(QUrl(m_baseUrl + request));
    nreq.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QNetworkReply *reply = m_netManager->post(nreq, postData);

    // Set property to know what type of request is!
    reply->setProperty("type", QVariant(type));

    connect(reply, SIGNAL(finished()), this, SLOT(requestFinished()));
}

//////////////////////////////////////////////////////////////////////////////
///
/// [SLOT] for GET, POST requests
///
//////////////////////////////////////////////////////////////////////////////
void ElasticSearch::requestFinished()
{
    QNetworkReply *nr = static_cast<QNetworkReply *>(sender());

//    if (nr->error() != QNetworkReply::NoError) {
//        debugNetResponse(nr);
//    }
//    else {
        QJsonParseError jerr;
        QJsonDocument   reply = QJsonDocument::fromJson(nr->readAll(), &jerr);
        if (jerr.error != QJsonParseError::NoError) {
            debugNetResponse(nr);
            qWarning() << __PRETTY_FUNCTION__ << "JSON ERROR:" << jerr.errorString();
            emit error(jerr.errorString());
        } else {
            parseResponseData(reply, nr->property("type").toInt());
        }
//    }


    if (nr->error() != QNetworkReply::NoError) {
        debugNetResponse(nr);
    }

    nr->deleteLater();
    nr = nullptr;
    m_ready = true;
}

/*
connect(currentDownload, SIGNAL(downloadProgress(qint64,qint64)), SLOT(downloadProgress(qint64,qint64)));
connect(currentDownload, SIGNAL(finished()), SLOT(downloadFinished()));
connect(currentDownload, SIGNAL(readyRead()), SLOT(downloadReadyRead()));
*/

void ElasticSearch::parseResponseData(const QJsonDocument &data, int type)
{
    switch (type) {
    case ElasticSearch::SearchRequest:
        emit dataReady(data);
        break;
    case ElasticSearch::MappingRequest:
        parseMappings(data);
        break;
    default:
        qWarning() << __PRETTY_FUNCTION__ << "Unkown request type:" << type;
        break;
    }
}

//////////////////////////////////////////////////////////////////////////////
/// \brief Query elasticsearch '_search'
/// \param endPoint
/// \param dslQuery
//////////////////////////////////////////////////////////////////////////////
void ElasticSearch::search(const QString &endPoint, const QJsonDocument &dslQuery)
{
    // cfr. http://www.elasticsearch.org/guide/en/elasticsearch/reference/current/search-request-body.html
    // [...] Both HTTP GET and HTTP POST can be used to execute search with
    // body. Since not all clients support GET with body, POST is allowed
    // as well.
    //qDebug() << __PRETTY_FUNCTION__ << endPoint << dslQuery.toJson();

    if (dslQuery.isNull()) {
        getRequest(endPoint, ElasticSearch::SearchRequest);
    } else {
        postRequest(endPoint, dslQuery.toJson(QJsonDocument::Compact), ElasticSearch::SearchRequest);
    }
}

void ElasticSearch::setDSL(const QString &dsl)
{
    qDebug() << dsl;
}

/*
void ElasticSearch::updateDocument(const QString &endPoint, const QJsonDocument &doc)
{
    postRequest(endPoint, doc.toJson(QJsonDocument::Compact), ElasticSearch::UpdateRequest);
}

void ElasticSearch::deleteDocument(const QString &endPoint)
{
    //postRequest(endPoint, dslQuery.toJson(QJsonDocument::Compact), ElasticSearch::UpdateRequest);
}
*/
