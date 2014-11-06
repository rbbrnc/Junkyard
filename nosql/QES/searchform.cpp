#include "searchform.h"
#include "ui_searchform.h"

#include "netservices.h"

#include <QDebug>


enum {
    SearchByFreeText = 0,
    SearchByFieldValue,
    SearchById
};

SearchForm::SearchForm(QWidget *parent) :
    BasePage(parent),
    ui(new Ui::SearchForm)
{
    ui->setupUi(this);
    ui->searchLineEdit->setClearButtonEnabled(true);

    m_currentEndpoint.clear();// = "/_search";
    //m_currentEndpoint = "/_search";

    m_es = NetServices::instance()->elasticsearchManager();
    connect(m_es, SIGNAL(mappingsDataReady()), this, SLOT(onMappingsDataReady()));
    connect(m_es, SIGNAL(dataReady(QJsonDocument)), this, SLOT(onSearchDataReady(QJsonDocument)));

    m_es->mappings();
}

SearchForm::~SearchForm()
{
    delete ui;
}

void SearchForm::createOptionsMenu(QWidget *parent)
{
    if (m_optionsMenu == nullptr) {
        m_optionsMenu = new QMenu(parent);
        m_optionsMenu->addAction(ui->actionEditDSLQuery);
        m_optionsMenu->addAction(ui->actionShowOptions);
    }
}

void SearchForm::on_indicesComboBox_currentIndexChanged(const QString &arg1)
{
    // Fill types combobox
    ui->typesComboBox->clear();
    QHash<QString, QStringList> map = m_es->indicesMap();

    // this trigger the currentIndexChanged signal
    ui->typesComboBox->addItems(map.value(arg1));
}

void SearchForm::on_typesComboBox_currentIndexChanged(const QString &arg1)
{
    // Update Current Query (endpoint) String

    if (arg1 == "_all_") {
        m_currentEndpoint.clear();
    } else {
        m_currentEndpoint = ui->indicesComboBox->currentText() + "/" + arg1;
    }
    //m_currentEndpoint += "/_search";

    ui->currentQueryLabel->setText(m_es->host() + m_currentEndpoint);
}

/*
void SearchForm::updateEndpoint()
{
    // Se ricerca x Id no _all_

    QString index = ui->indicesComboBox->currentText();
    if (index == "_all_") {
        m_currentEndpoint.clear();
    }
    QString type  = ui->typesComboBox->currentText();


}
*/

void SearchForm::onMappingsDataReady()
{
    ui->indicesComboBox->clear();

    // Fill indices combobox (this trigger the currentIndexChanged signal)
    QHash<QString, QStringList> map = m_es->indicesMap();
    ui->indicesComboBox->addItems(map.keys());
}

void SearchForm::on_searchLineEdit_returnPressed()
{
    on_goButton_clicked();
}


#define SEARCH_BY_FREE_TEXT   "{\"query\": {\"query_string\": {\"query\":\"%1\"}}}"
#define SEARCH_BY_FIELD_VALUE "{\"query\": {\"filtered\": {\"query\": {\"match_all\": { }}, \"filter\": {\"term\": {\"%1\":\"%2\"}}}}}"


QString SearchForm::prepareSearchQuery(int searchType, const QString &text)
{
    if (text.isEmpty()) {
        return QString();
    }

    QString qry;
    QStringList params;

    switch (searchType) {
    case SearchByFreeText:
        m_currentEndpoint += "/_search";
        qry = QString(SEARCH_BY_FREE_TEXT).arg(text);
        break;

    case SearchByFieldValue:
        m_currentEndpoint += "/_search";

        params = text.split(':');
        if (params.count() < 2) {
            qWarning() << "Search by field-value but wrong syntax, fallback to free-search";
            qry = QString(SEARCH_BY_FREE_TEXT).arg(text);
        } else {
            qry = QString(SEARCH_BY_FIELD_VALUE).arg(params.at(0)).arg(params.at(1));
            // In fields-value search field/value must be in lower case??
            qry.toLower();
        }
        break;

    case SearchById:
        m_currentEndpoint = ui->indicesComboBox->currentText() + "/" + ui->typesComboBox->currentText() + "/" + text;
        ui->currentQueryLabel->setText(m_es->host() + m_currentEndpoint);
        break;

    default:
        break;
    }

    return qry;
}

void SearchForm::on_goButton_clicked()
{
    QString term = ui->searchLineEdit->text();
    if (term.isEmpty()) {
        return;
    }

    QJsonDocument dslQuery;
    QJsonParseError dslError;
    QString qry = prepareSearchQuery(ui->comboBox->currentIndex(), ui->searchLineEdit->text());
    if (!qry.isEmpty()) {
        dslQuery = QJsonDocument::fromJson(qry.toUtf8(), &dslError);
        if (dslError.error != QJsonParseError::NoError) {
            qDebug() << __func__ << dslError.errorString();
            return;
        } else {
            qDebug() << __func__ << "Url:" << m_currentEndpoint << dslQuery.toJson();
        }
    }

    //"783SLZ3vRjaVZ__8cUMY7w"
    m_es->search(m_currentEndpoint, dslQuery);
}

void SearchForm::onSearchDataReady(const QJsonDocument &doc)
{
    int hits = 0;

    QJsonObject rootObj =  doc.object();
    if (rootObj.isEmpty()) {
        qDebug() << doc.toJson();
        qDebug() << "_________EMPTY obj --> hits=0";
        return;
    }

    //QJsonObject hitsObj = rootObj["hits"]
    hits = rootObj["hits"].toObject()["total"].toInt();

    qDebug() << "Hits:" << hits;
    qDebug() << doc.toJson();

}

void SearchForm::on_actionEditDSLQuery_triggered()
{
    emit changePage(JsonEditorPage);
}

void SearchForm::on_actionShowOptions_triggered()
{
    emit changePage(OptionsPage);
}

