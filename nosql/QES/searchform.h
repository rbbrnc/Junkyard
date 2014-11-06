#ifndef SEARCHFORM_H
#define SEARCHFORM_H

#include "basepage.h"

class ElasticSearch;

namespace Ui {
    class SearchForm;
}

class SearchForm : public BasePage
{
    Q_OBJECT

    public:
        explicit SearchForm(QWidget *parent = 0);
        ~SearchForm();

        virtual void createOptionsMenu(QWidget *parent);


    private:
        QString prepareSearchQuery(int searchType, const QString &text);

    private slots:
        void onMappingsDataReady();
        void onSearchDataReady(const QJsonDocument &doc);

        void on_goButton_clicked();
        void on_indicesComboBox_currentIndexChanged(const QString &arg1);
        void on_typesComboBox_currentIndexChanged(const QString &arg1);

        void on_actionEditDSLQuery_triggered();

        void on_actionShowOptions_triggered();

        void on_searchLineEdit_returnPressed();

private:
        Ui::SearchForm *ui;
        ElasticSearch *m_es;

        QString m_currentEndpoint;
};

#endif // SEARCHFORM_H
