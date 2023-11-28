#include "ViewDescriptorForm.h"

#include "UI/DescriptorTableModel.h"

ViewDescriptorForm::ViewDescriptorForm(EcuTuner::EcuFile* file, EcuTuner::Descriptor* descriptor, QWidget *parent) : QWidget(parent) {
	ui.setupUi(this);

	m_ecuFile = file;
	m_descriptor = descriptor;

	DescriptorTableModel* model = new DescriptorTableModel(file, descriptor);
	ui.testTable->setModel(model);


	this->setWindowTitle(m_descriptor->Alias + " | " + m_descriptor->Name + " | " + QString::number(m_descriptor->AxisData->Rows) + "x" + QString::number(m_descriptor->AxisData->Columns));
}

ViewDescriptorForm::~ViewDescriptorForm()
{
}
