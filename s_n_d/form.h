#ifndef FORM_H
#define FORM_H

#include "ui_form.h"

#include <QFutureWatcher>

#include "myclass.h"

class QProgressDialog;

class Form : public QWidget, private Ui::Form
{
Q_OBJECT

public slots:

  void slot_finished();
  void on_pushButton_clicked();

public:
    Form(QWidget *parent = 0);

private:
  QFutureWatcher<void> FutureWatcher;
  MyClass MyObject;
  QProgressDialog* ProgressDialog;
};

#endif

