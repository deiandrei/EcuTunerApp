#include "MainForm.h"

#include <fstream>
#include <qfiledialog.h>
#include <qbytearray.h>
#include <qtextstream.h>

#include "Common/Utils.h"
#include "Ecu/File.h"

#include "UI/DescriptorModel.h"
#include "UI/DescriptorModelItem.h"
#include "ViewDescriptorForm.h"

MainForm::MainForm(QWidget *parent) : QMainWindow(parent) {
    ui.setupUi(this);
    m_file = nullptr;

    connect(ui.browseEcuFileButton, &QPushButton::clicked, this, &MainForm::BrowseButton_Clicked);
    
    QList<EcuTuner::Descriptor*> descriptors;

    {
        EcuTuner::DataAxis* dataAxis = new EcuTuner::DataAxis;
        dataAxis->Offset = 86220;
        dataAxis->Rows = 16;
        dataAxis->Columns = 16;
        dataAxis->ValueFactor = 0.0234375;
        dataAxis->InverseMap = true;

        EcuTuner::Axis* xAxis = new EcuTuner::Axis;
        xAxis->Offset = 86156;
        xAxis->ValueFactor = 0.25;

        EcuTuner::Axis* yAxis = new EcuTuner::Axis;
        yAxis->Offset = 86188;
        yAxis->ValueFactor = 0.001525878906;

        EcuTuner::Descriptor* descriptor = new EcuTuner::Descriptor;
        descriptor->Name = "Optimal torque";
        descriptor->Alias = "KFMIOP";
        descriptor->AxisData = dataAxis;
        descriptor->AxisX = xAxis;
        descriptor->AxisY = yAxis;

        descriptors.push_back(descriptor);
    }

    {
        EcuTuner::DataAxis* dataAxis = new EcuTuner::DataAxis;
        dataAxis->Offset = 127094;
        dataAxis->Rows = 1;
        dataAxis->Columns = 16;
        dataAxis->ValueFactor = 0.0234375;

        EcuTuner::Axis* xAxis = new EcuTuner::Axis;
        xAxis->Offset = 127062;
        xAxis->ValueFactor = 0.25;

        EcuTuner::Descriptor* descriptor = new EcuTuner::Descriptor;
        descriptor->Name = "Maximum load";
        descriptor->Alias = "LDRXN";
        descriptor->AxisData = dataAxis;
        descriptor->AxisX = xAxis;

        descriptors.push_back(descriptor);
    }

    DescriptorModel* descriptorModel = new DescriptorModel(descriptors);
    ui.mapsTableView->setModel(descriptorModel);

    QObject::connect(ui.mapsTableView, &QTableView::doubleClicked, [&](const QModelIndex& index) {
        ViewDescriptorForm* vdForm = new ViewDescriptorForm(m_file, descriptorModel->getItemAtIndex(index)->GetDescriptor(), ui.mainArea);
        ui.mainArea->addSubWindow(vdForm);
        vdForm->showNormal();
    });
}

MainForm::~MainForm()
{}

void MainForm::BrowseButton_Clicked() {
    QString file = QFileDialog::getOpenFileName(nullptr, "Select ECU bin");
    
    ui.ecuFileLineEdit->setText(file);
    SetECUFile(file);
}

void MainForm::SetECUFile(const QString& file) {
    if (file.isEmpty()) return;

    if (m_file) delete m_file;

    std::ifstream reader(file.toStdString(), std::ios::binary);

    ByteBuffer* bb = EcuTuner::Utils::ReadFileIntoByteBuffer(reader);
    if (bb != nullptr) {
        m_file = new EcuTuner::EcuFile(bb);

        QByteArray ba = EcuTuner::Utils::ByteBufferToQByteArray(bb);
        //ui.binaryTextEdit->setText(ba.toHex());
    }
    else {
        m_file = nullptr;
    }
}
