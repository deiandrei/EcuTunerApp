#include "ViewDescriptorForm.h"

#include "UI/DescriptorTableModel.h"

ViewDescriptorForm::ViewDescriptorForm(EcuTuner::EcuFile* file, EcuTuner::Descriptor* descriptor, QWidget *parent) : QWidget(parent) {
	ui.setupUi(this);

	m_ecuFile = file;
	m_descriptor = descriptor;

	DescriptorTableModel* model = new DescriptorTableModel(file, descriptor);
	ui.testTable->setModel(model);

	//UpdateUI();
}

ViewDescriptorForm::~ViewDescriptorForm()
{
}

void ViewDescriptorForm::UpdateUI() {
	ByteBuffer* bb = m_ecuFile->GetBuffer();
	EcuTuner::DataAxis axis = *m_descriptor->AxisData;
	
	//ui.testTable->setRowCount(axis.Rows);
	//ui.testTable->setColumnCount(axis.Columns);

	//// create horizontal header
	//if (m_descriptor->AxisX) {
	//	bb->Position() = m_descriptor->AxisX->Offset;
	//	QStringList header;
	//	for (int i = 0; i < axis.Columns; ++i) {
	//		uint2 data = bb->Read<uint2>();

	//		header << QString::number(data * m_descriptor->AxisX->ValueFactor);
	//	}
	//	ui.testTable->setHorizontalHeaderLabels(header);
	//}

	//// create vertical header
	//if (m_descriptor->AxisY) {
	//	bb->Position() = m_descriptor->AxisY->Offset;
	//	QStringList header;
	//	for (int i = 0; i < axis.Rows; ++i) {
	//		uint2 data = bb->Read<uint2>();

	//		header << QString::number(data * m_descriptor->AxisY->ValueFactor);
	//	}
	//	ui.testTable->setVerticalHeaderLabels(header);
	//}

	//// load data
	//bb->Position() = axis.Offset;
	//if (m_descriptor->InverseMap) {
	//	for (int i = 0; i < axis.Columns; ++i) {
	//		for (int j = 0; j < axis.Rows; ++j) {
	//			uint2 data = bb->Read<uint2>();

	//			ui.testTable->setItem(j, i, new QTableWidgetItem(QString::number(data * axis.ValueFactor)));
	//			if(ui.testTable->item(j, i)->text().toDouble() > 100.0) ui.testTable->item(j, i)->setBackground(Qt::red);
	//			else if(ui.testTable->item(j, i)->text().toDouble() > 50.0) ui.testTable->item(j, i)->setBackground(Qt::yellow);
	//			else ui.testTable->item(j, i)->setBackground(Qt::green);
	//		}
	//	}
	//}
	//else {
	//	for (int j = 0; j < axis.Rows; ++j) {
	//		for (int i = 0; i < axis.Columns; ++i) {
	//			uint2 data = bb->Read<uint2>();

	//			ui.testTable->setItem(j, i, new QTableWidgetItem(QString::number(data * axis.ValueFactor)));
	//			if (ui.testTable->item(j, i)->text().toDouble() > 100.0) ui.testTable->item(j, i)->setBackground(Qt::red);
	//			else if (ui.testTable->item(j, i)->text().toDouble() > 50.0) ui.testTable->item(j, i)->setBackground(Qt::yellow);
	//			else ui.testTable->item(j, i)->setBackground(Qt::green);
	//		}
	//	}
	//}
	//
}
