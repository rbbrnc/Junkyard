#ifndef OPTIONFORM_H
#define OPTIONFORM_H

#include "basepage.h"

class ElasticSearch;

namespace Ui {
    class OptionForm;
}

class OptionForm : public BasePage
{
    Q_OBJECT

    public:
        explicit OptionForm(QWidget *parent = 0);
        ~OptionForm();

        virtual void createOptionsMenu(QWidget *parent);

    private slots:
        void onEsReady();
        void on_testButton_clicked();

        void on_actionReLoad_triggered();
        void on_actionSave_triggered();

    private:
        Ui::OptionForm *ui;
        ElasticSearch *m_es;

};

#endif // OPTIONFORM_H
