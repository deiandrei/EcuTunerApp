#pragma once

#include <QtWidgets/QMainWindow>
#include <qsortfilterproxymodel.h>
#include "ui_MainForm.h"

#include "Ecu/File.h"
#include "Descriptor/Descriptor.h"

class MainForm : public QMainWindow
{
    Q_OBJECT

    public:
        MainForm(QWidget *parent = nullptr);
        ~MainForm();

        void SetECUFile(const QString& file);

    public slots:
        void OpenDescriptorForm(const QModelIndex& index);
        void ContextMenuForDescriptorTable(const QPoint& point);

    private:
        void CreateContextMenus();

    private:
        Ui::MainFormClass ui;
        QSortFilterProxyModel* m_sortModel;
        QMenu* m_descriptorsContextMenu;

        EcuTuner::EcuFile* m_file;
        QList<EcuTuner::Descriptor*> m_descriptors;

};
