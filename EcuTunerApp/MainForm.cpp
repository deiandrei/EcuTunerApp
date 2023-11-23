#include "MainForm.h"

#include <fstream>
#include <qfiledialog.h>
#include <qbytearray.h>
#include <qtextstream.h>

#include "Common/Utils.h"
#include "Ecu/File.h"
#include "Descriptor/DescriptorReader.h"

#include "UI/DescriptorModel.h"
#include "UI/DescriptorModelItem.h"
#include "ViewDescriptorForm.h"

MainForm::MainForm(QWidget *parent) : QMainWindow(parent) {
    ui.setupUi(this);
    m_file = nullptr;

    connect(ui.mapsTableView, &QTableView::doubleClicked, this, &MainForm::OpenDescriptorForm);

    connect(ui.actionLoad_ECU_File, &QAction::triggered, [&]() {
        QString file = QFileDialog::getOpenFileName(nullptr, "Select ECU bin");
        ui.ecuFileLineEdit->setText(file);
        SetECUFile(file);
    });

    connect(ui.actionLoad_DescriptorPack, &QAction::triggered, [&]() {
        QString descriptorFile = QFileDialog::getOpenFileName(nullptr, "Select Descriptor file");

        QList<EcuTuner::Descriptor*> descriptors;
        if (!descriptorFile.isEmpty()) descriptors = EcuTuner::DescriptorReader::ReadDescriptorsFromFile(descriptorFile);

        DescriptorModel* descriptorModel = new DescriptorModel(descriptors);
        ui.mapsTableView->setModel(descriptorModel);

        ui.actionLoad_DescriptorPack->setEnabled(false); // disable it for now until I implement how to reload packs
    });

    connect(ui.actionSave_As, &QAction::triggered, [&]() {
        QString fileSaveName = QFileDialog::getSaveFileName(nullptr, "Save ECU bin");
        if (!fileSaveName.isEmpty()) {
            std::ofstream writer(fileSaveName.toStdString(), std::ios::binary);

            writer.write((char*)m_file->GetBuffer()->Data(), m_file->GetBuffer()->Size());
        }
    });
}

MainForm::~MainForm()
{}


void MainForm::OpenDescriptorForm(const QModelIndex& index) {
    EcuTuner::Descriptor* descriptor = ((DescriptorModel*)ui.mapsTableView->model())->getItemAtIndex(index)->GetDescriptor();
    if (!descriptor) return;

    // ShowNormal the current window if the window is maximized
    auto* currentWindow = ui.mainArea->currentSubWindow();
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
        ui.actionSave_As->setEnabled(true);
    }
    else {
        m_file = nullptr;
        ui.actionLoad_ECU_File->setEnabled(true);
        ui.actionLoad_DescriptorPack->setEnabled(false);
        ui.actionSave_As->setEnabled(false);
    }
}
