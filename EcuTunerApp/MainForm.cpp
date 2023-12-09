#include "MainForm.h"

#include <fstream>
#include <qfiledialog.h>
#include <qbytearray.h>
#include <qtextstream.h>
#include <qmenu.h>
#include <qmessagebox.h>
#include <qshortcut.h>

#include "Common/Utils.h"
#include "Ecu/File.h"
#include "Descriptor/DescriptorReader.h"
#include "Descriptor/DescriptorConverter.h"
#include "UI/UR/URManager.h"

#include "UI/DescriptorModel.h"
#include "UI/DescriptorModelItem.h"
#include "ViewDescriptorForm.h"

MainForm::MainForm(QWidget *parent) : QMainWindow(parent) {
    ui.setupUi(this);

    ui.mapsTreeView->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
    m_sortModel = new QSortFilterProxyModel(this);
    m_sortModel->setFilterCaseSensitivity(Qt::CaseSensitivity::CaseInsensitive);
    m_sortModel->setRecursiveFilteringEnabled(true);

    m_file = nullptr;

    connect(ui.mapsTreeView, &QTableView::doubleClicked, this, &MainForm::OpenDescriptorForm);
    connect(ui.mapsTreeView, &QTreeView::customContextMenuRequested, this, &MainForm::ContextMenuForDescriptorTable);
    connect(ui.actionQuit, &QAction::triggered, this, &MainForm::close);

    connect(ui.actionNew_project, &QAction::triggered, [&]() {
        if (QMessageBox::question(this, "Disclaimer", "Are you sure you want to start a new project?", QMessageBox::Yes | QMessageBox::Cancel) == QMessageBox::Yes) {
            ui.mainArea->closeAllSubWindows();
            ui.descriptorSearchLineEdit->setText("");
            ui.mapsTreeView->setModel(nullptr);
            ui.ecuFileLineEdit->setText("");
            ui.descriptorFileLineEdit->setText("");

            if (m_file) {
                delete m_file;
                m_file = nullptr;
            }

            ui.actionLoad_ECU_File->setEnabled(true);
            ui.actionSave_As->setEnabled(false);
            ui.actionLoad_DescriptorPack->setEnabled(false);
            ui.actionConvert_Csv_to_DescriptorPack->setEnabled(false);
        }
    });

    connect(ui.actionLoad_ECU_File, &QAction::triggered, [&]() {
        QString file = QFileDialog::getOpenFileName(nullptr, "Select ECU bin");
        ui.ecuFileLineEdit->setText(file);
        SetECUFile(file);
    });

    connect(ui.actionSave_As, &QAction::triggered, [&]() {
        QString fileSaveName = QFileDialog::getSaveFileName(nullptr, "Save ECU bin");
        if (!fileSaveName.isEmpty()) {
            std::ofstream writer(fileSaveName.toStdString(), std::ios::binary);

            writer.write((char*)m_file->GetBuffer()->Data(), m_file->GetBuffer()->Size());
        }
    });

    ////////////////////////////////////////////////////////////////////////////////////////

    /*connect(new QShortcut(QKeySequence::Undo, this), &QShortcut::activated, ui.actionUndo, &QAction::trigger);

    connect(URManager::Instance(), &URManager::EnableUndo, [&]() {
        ui.actionUndo->setEnabled(true);
    });

    connect(URManager::Instance(), &URManager::DisableUndo, [&]() {
        ui.actionUndo->setEnabled(false);
    });

    connect(ui.actionUndo, &QAction::triggered, [&]() {
        URManager::Instance()->Undo();
    });*/

    ////////////////////////////////////////////////////////////////////////////////////////

    connect(ui.actionLoad_DescriptorPack, &QAction::triggered, [&]() {
        QString descriptorFile = QFileDialog::getOpenFileName(nullptr, "Select Descriptor file");

        if (!descriptorFile.isEmpty()) {
            m_descriptors = EcuTuner::DescriptorReader::ReadDescriptorsFromFile(descriptorFile);

            DescriptorModel* descriptorModel = new DescriptorModel(m_descriptors);
            m_sortModel->setSourceModel(descriptorModel);
            ui.mapsTreeView->setModel(m_sortModel);

            ui.descriptorFileLineEdit->setText(descriptorFile);
            ui.actionLoad_DescriptorPack->setEnabled(false); // disable it for now until I implement how to reload packs
            ui.actionConvert_Csv_to_DescriptorPack->setEnabled(false); // disable it for now until I implement how to reload packs
        }
    });

    connect(ui.actionConvert_Csv_to_DescriptorPack, &QAction::triggered, [&]() {
        QString descriptorFile = QFileDialog::getOpenFileName(nullptr, "Select CSV file");

        if (!descriptorFile.isEmpty()) {
            m_descriptors = EcuTuner::DescriptorConverter::ConvertFromCsv(descriptorFile);

            DescriptorModel* descriptorModel = new DescriptorModel(m_descriptors);
            m_sortModel->setSourceModel(descriptorModel);
            ui.mapsTreeView->setModel(m_sortModel);
            
            ui.descriptorFileLineEdit->setText(descriptorFile);
            ui.actionLoad_DescriptorPack->setEnabled(false); // disable it for now until I implement how to reload packs
            ui.actionConvert_Csv_to_DescriptorPack->setEnabled(false); // disable it for now until I implement how to reload packs
        }
    });

    connect(ui.actionSave_DescriptorPack, &QAction::triggered, [&]() {
        QString saveFile = QFileDialog::getSaveFileName(nullptr, "Save DescriptorPack file");

        if (!saveFile.isEmpty()) {
            EcuTuner::DescriptorReader::WriteDescriptorsToFile(m_descriptors, saveFile);
        }
    });

    connect(ui.descriptorSearchLineEdit, &QLineEdit::textChanged, [&](QString text) {
        m_sortModel->setFilterFixedString(text);
    });

    CreateContextMenus();
}

MainForm::~MainForm()
{}


void MainForm::OpenDescriptorForm(const QModelIndex& index) {
    EcuTuner::Descriptor* descriptor = static_cast<DescriptorModel*>(m_sortModel->sourceModel())->getItemAtIndex(m_sortModel->mapToSource(index))->GetDescriptor();
    if (!descriptor) return;

    // ShowNormal the current window if the window is maximized
    auto currentWindow = ui.mainArea->currentSubWindow();
    if (currentWindow && currentWindow->isMaximized()) {
        currentWindow->showNormal();
    }

    // Search for another window for the same descriptor and show that instead of creating a new one
    for (auto* openSubWindow : ui.mainArea->subWindowList()) {
        ViewDescriptorForm* castedWindow = dynamic_cast<ViewDescriptorForm*>(openSubWindow->widget());

        if (castedWindow && castedWindow->GetDescriptor() == descriptor) {
            openSubWindow->showNormal();
            ui.mainArea->setActiveSubWindow(openSubWindow);
            return;
        }
    }

    // If no window was found, create the new one and ShowNormal it
	ViewDescriptorForm* vdForm = new ViewDescriptorForm(m_file, descriptor, ui.mainArea);
	ui.mainArea->addSubWindow(vdForm);
	vdForm->showNormal();
}

void MainForm::SetECUFile(const QString& file) {
    if (file.isEmpty()) return;

    if (m_file) delete m_file;

    std::ifstream reader(file.toStdString(), std::ios::binary);

    ByteBuffer* bb = EcuTuner::Utils::ReadFileIntoByteBuffer(reader);
    if (bb != nullptr) {
        m_file = new EcuTuner::EcuFile(bb);

        //QByteArray ba = EcuTuner::Utils::ByteBufferToQByteArray(bb);
        //ui.binaryTextEdit->setText(ba.toHex());

        ui.actionLoad_ECU_File->setEnabled(false);
        ui.actionLoad_DescriptorPack->setEnabled(true);
        ui.actionConvert_Csv_to_DescriptorPack->setEnabled(true);
        ui.actionSave_As->setEnabled(true);
    }
    else {
        m_file = nullptr;
        ui.actionLoad_ECU_File->setEnabled(true);
        ui.actionLoad_DescriptorPack->setEnabled(false);
        ui.actionConvert_Csv_to_DescriptorPack->setEnabled(false);
        ui.actionSave_As->setEnabled(false);
    }
}

void MainForm::ContextMenuForDescriptorTable(const QPoint& point) {
    QModelIndex index = ui.mapsTreeView->indexAt(point);

    if (index.isValid()) {
    }
    
    m_descriptorsContextMenu->exec(ui.mapsTreeView->mapToGlobal(point));
}

void MainForm::CreateContextMenus() {
    m_descriptorsContextMenu = new QMenu(ui.mapsTreeView);

    connect(m_descriptorsContextMenu->addAction("Expand all"), &QAction::triggered, [&]() {
        ui.mapsTreeView->expandAll();
    });

    connect(m_descriptorsContextMenu->addAction("Collapse all"), &QAction::triggered, [&]() {
        ui.mapsTreeView->collapseAll();
    });
}
