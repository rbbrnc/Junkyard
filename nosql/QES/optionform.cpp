#include "optionform.h"
#include "ui_optionform.h"

#include "netservices.h"

OptionForm::OptionForm(QWidget *parent) :
    BasePage(parent),
    ui(new Ui::OptionForm)
{
    ui->setupUi(this);
    m_es = NetServices::instance()->elasticsearchManager();
    connect(m_es, SIGNAL(mappingsDataReady()), this, SLOT(onEsReady()));
}

OptionForm::~OptionForm()
{
    delete m_es;
    delete ui;
}

// [VIRTUAL public]
void OptionForm::createOptionsMenu(QWidget *parent)
{
    if (m_optionsMenu == nullptr) {
        m_optionsMenu = new QMenu(parent);
        m_optionsMenu->addAction(ui->actionReLoad);
        m_optionsMenu->addAction(ui->actionSave);
    }
}

void OptionForm::on_testButton_clicked()
{
    m_es->mappings();
}

void OptionForm::onEsReady()
{
    ui->indicesComboBox->clear();
/*

    for (int i = 0; i < indices.count(); i++) {
        ui->indicesComboBox->addItem(indices.at(i));
    }
    */
}

void OptionForm::on_actionReLoad_triggered()
{
    qDebug() << __PRETTY_FUNCTION__;
}

void OptionForm::on_actionSave_triggered()
{
    qDebug() << __PRETTY_FUNCTION__;
}
