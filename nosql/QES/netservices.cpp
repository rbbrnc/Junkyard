#include "netservices.h"

NetServices *NetServices::m_instance = nullptr;

NetServices *NetServices::instance()
{
    if (m_instance == 0) {
        m_instance = new NetServices();
    }
    return m_instance;
}

NetServices::NetServices(QObject *parent) :
    QObject(parent)
{
    m_elasticsearchManager = new ElasticSearch(&m_netManager);
}

NetServices::~NetServices()
{
    delete m_elasticsearchManager;
}

ElasticSearch *NetServices::elasticsearchManager()
{
    return m_elasticsearchManager;
}
