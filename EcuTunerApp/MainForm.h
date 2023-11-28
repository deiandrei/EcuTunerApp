#pragma once

#include <QtWidgets/QMainWindow>
#include <qsortfilterproxymodel.h>
#include "ui_MainForm.h"

#include "Ecu/File.h"

class MainForm : public QMainWindow
{
    Q_OBJECT

    public:
        MainForm(QWidget *parent = nullptr);
        ~MainForm();

        void SetECUFile(const QString& file);

    public slots:
        void OpenDescriptorForm(const QModelIndex& index);

    private:
        Ui::MainFormClass ui;
        QSortFilterProxyModel* m_sortModel;

        EcuTuner::EcuFile* m_file;

};
