#include "jsoneditorform.h"
#include "ui_jsoneditorform.h"

#include <QFile>
#include <QJsonDocument>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextCodec>
#include <QTextDocumentWriter>
#include <QJsonParseError>
#include <QDebug>

#include "jsontreemodel.h"

JsonEditorForm::JsonEditorForm(QWidget *parent) :
    BasePage(parent),
    ui(new Ui::JsonEditorForm),
    m_model(nullptr)
{
    ui->setupUi(this);

    ui->filePathLabel->setText(QString());

    ui->jsonTreeView->setUniformRowHeights(true);
    ui->jsonTreeView->setContextMenuPolicy(Qt::CustomContextMenu);

    //ui->jsonTreeView->header()->setCascadingSectionResizes(true);
    //ui->jsonTreeView->resizeColumnsToContents(1);
    //ui->jsonTreeView->setEditTriggers(QAbstractItemView::DoubleClicked);

    m_jsonTreeActions.clear();
    m_jsonTreeActions.append(ui->actionAddItem);
    m_jsonTreeActions.append(ui->actionEditItem);
    m_jsonTreeActions.append(ui->actionDeleteItem);

    // connect custom context menu
    connect(ui->jsonTreeView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onContextMenu(QPoint)));
}

JsonEditorForm::~JsonEditorForm()
{
    if (m_model) {
        delete m_model;
        m_model = nullptr;
    }

    delete ui;
}

void JsonEditorForm::onContextMenu(const QPoint &pnt)
{
    if ((ui->jsonTreeView->indexAt(pnt).isValid()) && (m_jsonTreeActions.count() > 0)) {
        QMenu::exec(m_jsonTreeActions, ui->jsonTreeView->mapToGlobal(pnt));
    }
}

void JsonEditorForm::createOptionsMenu(QWidget *parent)
{
    if (m_optionsMenu == nullptr) {
        m_optionsMenu = new QMenu(parent);
        m_optionsMenu->addAction(ui->actionLoadJsonFile);
        m_optionsMenu->addAction(ui->actionSaveJsonFile);
        m_optionsMenu->addAction(ui->actionValidateJson);
    }
}

void JsonEditorForm::on_actionLoadJsonFile_triggered()
{
    setDocumentFromFile();
}

void JsonEditorForm::on_actionSaveJsonFile_triggered()
{
    saveDocument();
}

void JsonEditorForm::on_actionValidateJson_triggered()
{
    validateDocument();
}

void JsonEditorForm::setDocument(const QJsonDocument &doc)
{
    if (m_model) {
        delete m_model;
        m_model = nullptr;
    }
    m_model = new JsonTreeModel(doc);
    ui->jsonTreeView->setModel(m_model);

/*
    for (int column = 0; column < m_model->columnCount(); ++column) {
        ui->jsonTreeView->resizeColumnToContents(column);
    }
*/
    ui->jsonTextEdit->setPlainText(QString(doc.toJson()));
}

void JsonEditorForm::setDocumentFromFile(const QString &fname)
{
    // Ask to save before open another file.

    QString f;
    if (fname.isEmpty()) {
        f = QFileDialog::getOpenFileName(0, "Open JSON File");
        if (f.isEmpty()) {
            return;
        }
    } else {
        f = fname;
    }

    if (!QFile::exists(f)) {
        QMessageBox::warning(0, "Load JSON", QString("File: %1\nNot found!").arg(f));
        return;
    }

    QFile file(f);
    if (!file.open(QFile::ReadOnly)) {
        QMessageBox::warning(0, "Load JSON", QString("File: %1\nCan't be open: %2").arg(f).arg(file.errorString()));
        return;
    }

    QByteArray data = file.readAll();

    QJsonParseError err;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data, &err);
    if (err.error != QJsonParseError::NoError) {
        QMessageBox::warning(0, "Load JSON", QString("File: %1\nPARSE ERROR: %2\nat offset:%3")
                             .arg(f)
                             .arg(err.errorString())
                             .arg(err.offset));

        // XXX LOAD ANYWAY!?!?
        return;
    }

    setDocument(jsonDoc);
    ui->jsonTextEdit->document()->setModified(false);

    ui->filePathLabel->setText(f);
}

void JsonEditorForm::saveDocument()
{
    QString f = QFileDialog::getSaveFileName(this, tr("Save JSON"), ui->filePathLabel->text());
    if (f.isEmpty()) {
        return;
    }

    QTextDocumentWriter writer(f, "plaintext");
    writer.setCodec(QTextCodec::codecForName("UTF-8"));

    if (writer.write(ui->jsonTextEdit->document())) {
        ui->jsonTextEdit->document()->setModified(false);
        ui->filePathLabel->setText(f);
        return;
    }

    QMessageBox::warning(this, "Save JSON", QString("File %1 cannot be saved: %2")
                         .arg(f).arg(writer.device()->errorString()));
}

void JsonEditorForm::validateDocument()
{
    QJsonParseError e;
    (void) QJsonDocument::fromJson(ui->jsonTextEdit->toPlainText().toUtf8(), &e);

    if (e.error != QJsonParseError::NoError) {
        QString s = QString("Invalid JSON at offset: %1\n%2")
                .arg(e.offset).arg(e.errorString());

        QMessageBox::warning(this, tr("JSON Validate"), s);

        // XXX: PLACE CURSOR ON e.offset
        QTextCursor cursor = ui->jsonTextEdit->textCursor();
        cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor, 1);
        cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, e.offset);
        //cursor.setPosition(e.offset, QTextCursor::MoveAnchor);
        qDebug() << __func__ << cursor.position();
    } else {
        QMessageBox::information(this, tr("Validate JSON"), tr("Document is VALID!"));
    }
}

void JsonEditorForm::on_jsonTextEdit_cursorPositionChanged()
{
    ui->positionLabel->setText(QString::number(ui->jsonTextEdit->textCursor().position()));
}


#if 0
void MainWindow::setCurrentFileName(const QString &/*fileName*/)
{
    m_currentFileName = fileName;
    ui->textEdit->document()->setModified(false);

    QString shownName;
    if (fileName.isEmpty())
        shownName = "<untitled>";
    else
        shownName = QFileInfo(fileName).fileName();

    setWindowTitle(tr("%1[*]").arg(shownName));
    setWindowModified(false);
}

void MainWindow::fileNew()
{
    if (maybeSave()) {
        ui->textEdit->clear();
        setCurrentFileName(QString());
    }
}

bool MainWindow::maybeSave()
{
    if (!ui->textEdit->document()->isModified()) {
        return true;
    }

    QMessageBox::StandardButton ret;
    ret = QMessageBox::warning(this, tr("JSON Edit"),
                tr("The document has been modified.\n"
                   "Do you want to save your changes?"),
                QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    if (ret == QMessageBox::Save) {
        return fileSave(m_currentFileName);
    } else if (ret == QMessageBox::Cancel) {
        return false;
    }
    return true;
}
#endif

void JsonEditorForm::on_actionAddItem_triggered()
{
    qDebug() << __PRETTY_FUNCTION__;

    QModelIndex index = ui->jsonTreeView->selectionModel()->currentIndex();

    if (!m_model->insertRow(index.row()+1, index.parent())) {
        qDebug() << "HERE";
        return;
    }

    for (int column = 0; column < m_model->columnCount(index.parent()); ++column) {
        QModelIndex child = m_model->index(index.row()+1, column, index.parent());
        m_model->setData(child, QVariant("[No data]"), Qt::EditRole);
    }
}

void JsonEditorForm::on_actionEditItem_triggered()
{
    QModelIndex idx = ui->jsonTreeView->currentIndex();
    if (idx.isValid()) {
        qDebug() << __PRETTY_FUNCTION__ << "EDIT ITEM:" << idx.column() << idx.data();
    } else {
        qDebug() << __PRETTY_FUNCTION__ << "Invalid model index";
    }
}

void JsonEditorForm::on_actionDeleteItem_triggered()
{
    QModelIndex idx = ui->jsonTreeView->currentIndex();
    if (!idx.isValid()) {
        qDebug() << __PRETTY_FUNCTION__ << "Invalid model index";
        return;
    }

    if (QMessageBox::Ok == QMessageBox::warning(this, tr("JSON Edit"), tr("Remove Entry?"), QMessageBox::Ok | QMessageBox::Cancel)) {
        qDebug() << "DELETE ITEM:" << idx.row() << idx.column() << idx.data();
        bool rc = m_model->removeRow(idx.row(), idx.parent());
        qDebug() << rc;
    }

}
