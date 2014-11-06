#ifndef NETSERVICES_H
#define NETSERVICES_H

#include <QObject>
#include <QNetworkAccessManager>

#include "elasticsearch.h"

class NetServices : public QObject
{
    Q_OBJECT

    private:
        static NetServices *m_instance;

        NetServices(QObject *parent = 0);

    public:
        static NetServices *instance();
        ~NetServices();

        ElasticSearch *elasticsearchManager();

    private:
        QNetworkAccessManager m_netManager;

        ElasticSearch *m_elasticsearchManager;

};

#endif
