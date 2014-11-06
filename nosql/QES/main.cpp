#include <QApplication>
//#include <QDebug>

#include "netservices.h"
#include "mainwindow.h"

int main(int argc, char **argv)
{
	QApplication app(argc, argv);

    NetServices *ns = NetServices::instance();

    // Wait mappings...
    ElasticSearch *es = ns->elasticsearchManager();
    es->mappings(true);
    while (!es->isReady()) {
        app.processEvents();
    }

    MainWindow w;
    w.show();

	return app.exec();
}
