#ifndef JSONEDITORFORM_H
#define JSONEDITORFORM_H

#include <basepage.h>

class JsonTreeModel;

namespace Ui {
    class JsonEditorForm;
}

class JsonEditorForm : public BasePage
{
    Q_OBJECT

    public:
        explicit JsonEditorForm(QWidget *parent = 0);
        ~JsonEditorForm();

        void setDocumentFromFile(const QString &fname = QString());
        void setDocument(const QJsonDocument &doc);
        void validateDocument();

        virtual void createOptionsMenu(QWidget *parent);

    private slots:
        void saveDocument();

        void on_jsonTextEdit_cursorPositionChanged();

        void on_actionLoadJsonFile_triggered();
        void on_actionSaveJsonFile_triggered();
        void on_actionValidateJson_triggered();

        void onContextMenu(const QPoint &pnt);

        void on_actionAddItem_triggered();
        void on_actionEditItem_triggered();
        void on_actionDeleteItem_triggered();

private:
        Ui::JsonEditorForm *ui;
        JsonTreeModel *m_model;

        QList<QAction *> m_jsonTreeActions;

};

#endif // JSONEDITORFORM_H
