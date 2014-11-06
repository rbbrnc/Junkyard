#ifndef ELASTICSEARCH_H
#define ELASTICSEARCH_H

#include <QtNetwork>

#define DEFAULT_HOST "http://localhost:9200/"
#define DEFAULT_ENDPOINT "ricaricaev/stations/_search"


struct ElasticSearchIndex {
    QString     name;
    QStringList types;
};


struct ElasticSearchNode {
        QString host;  //curl 127.0.0.1:9200/_cat/nodes?v\&h=host,ip,port
        QList<ElasticSearchIndex> indexList;
};


class ElasticSearch : public QObject
{
	Q_OBJECT

	public:
        ElasticSearch(QNetworkAccessManager *mgr, const QString &host = DEFAULT_HOST);
        ~ElasticSearch();

        enum RequestType {
            SearchRequest = 0,
            MappingRequest
        };

		void setHost(const QString &baseUrl);
        QString host() const;

        QHash<QString, QStringList> indicesMap() const;

        bool isReady() const;

        void setDSL(const QString &dsl);

	private:
        void debugNetResponse(QNetworkReply *nr);

        void getRequest(const QString &request, ElasticSearch::RequestType type);
        void postRequest(const QString &request, const QByteArray &postData, ElasticSearch::RequestType type);

        void parseResponseData(const QJsonDocument &data, int type);
        void parseMappings(const QJsonDocument &data);

	signals:
        void dataReady(const QJsonDocument &doc);
        void error(const QString &errorString);

        void mappingsDataReady();

	public slots:
        // _search
		void search(const QString &endPoint, const QJsonDocument &dslQuery);

        // _mappings
        void mappings(bool reload = false, const QString &index = QString(), const QString &type = QString());

	private slots:
        // Valid for GET and POST requests.
        void requestFinished();

	private:
        QNetworkAccessManager *m_netManager;
        bool m_ready;


		QString m_baseUrl;
        QHash<QString, QStringList> m_indicesMap;

        QJsonDocument m_dslQuery;

};

#endif
