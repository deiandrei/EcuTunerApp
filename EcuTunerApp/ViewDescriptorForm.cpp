#include "ViewDescriptorForm.h"

#include <QShortcut>
#include <QClipboard>

ViewDescriptorForm::ViewDescriptorForm(EcuTuner::EcuFile* file, EcuTuner::Descriptor* descriptor, QWidget *parent) : QWidget(parent) {
	ui.setupUi(this);

	m_ecuFile = file;
	m_descriptor = descriptor;

	m_model = new DescriptorTableModel(file, descriptor);
	ui.mainTable->setModel(m_model);
	ui.mainTable->setEditTriggers(QAbstractItemView::NoEditTriggers | QAbstractItemView::DoubleClicked);

	this->setWindowTitle(m_descriptor->Alias + " | " + m_descriptor->Name + " | " + QString::number(m_descriptor->AxisData->Rows) + "x" + QString::number(m_descriptor->AxisData->Columns));

	connect(ui.minusButton, &QPushButton::pressed, [&]() { AddMultiplySelectedIndexes(-ui.addFactorSpinBox->text().toDouble(), 1.0); });
	connect(new QShortcut(QKeySequence("_"), this), &QShortcut::activated, [&]() { IncrementSelectedIndexes(-1.0); });

	connect(ui.plusButton, &QPushButton::pressed, [&]() { AddMultiplySelectedIndexes(ui.addFactorSpinBox->text().toDouble(), 1.0); });
	connect(new QShortcut(QKeySequence("+"), this), &QShortcut::activated, [&]() { IncrementSelectedIndexes(1.0); });

	connect(ui.mulButton, &QPushButton::pressed, [&]() { AddMultiplySelectedIndexes(0.0, ui.multiplyFactorSpinBox->text().toDouble()); });
	connect(ui.divButton, &QPushButton::pressed, [&]() { AddMultiplySelectedIndexes(0.0, 1.0 / ui.multiplyFactorSpinBox->text().toDouble()); });

	connect(new QShortcut(QKeySequence::Copy, this), &QShortcut::activated, this, &ViewDescriptorForm::CopySelectedIndex);
	connect(new QShortcut(QKeySequence::Paste, this), &QShortcut::activated, this, &ViewDescriptorForm::PasteSelectedIndex);

	ui.addFactorSpinBox->setValue(descriptor->AxisData->ValueFactor);
}

ViewDescriptorForm::~ViewDescriptorForm() {

}

void ViewDescriptorForm::IncrementSelectedIndexes(double factor) {
	QItemSelectionModel* selectionModel = ui.mainTable->selectionModel();

	if (selectionModel->hasSelection()) {
		for (const auto& selectedIndex : selectionModel->selectedIndexes()) {
			m_model->incrementIndex(selectedIndex, factor);
		}

		m_model->dataChanged(selectionModel->selectedIndexes().first(), selectionModel->selectedIndexes().last());
		ui.mainTable->setFocus();
	}
}

void ViewDescriptorForm::AddMultiplySelectedIndexes(double number, double multiplyFactor) {
	QItemSelectionModel* selectionModel = ui.mainTable->selectionModel();

	if (selectionModel->hasSelection()) {
		for (const auto& selectedIndex : selectionModel->selectedIndexes()) {
			m_model->addMultiplyIndex(selectedIndex, number, multiplyFactor);
		}

		m_model->dataChanged(selectionModel->selectedIndexes().first(), selectionModel->selectedIndexes().last());
		ui.mainTable->setFocus();
	}
}

void ViewDescriptorForm::CopySelectedIndex() {
	QItemSelectionModel* selectionModel = ui.mainTable->selectionModel();

	if (selectionModel->hasSelection()) {
		QModelIndex index = selectionModel->selectedIndexes().first();

		QVariant data = m_model->data(index);
		QApplication::clipboard()->setText(data.toString());
	}
}

void ViewDescriptorForm::PasteSelectedIndex() {
	if (QApplication::clipboard()->text().isEmpty()) return;

	QItemSelectionModel* selectionModel = ui.mainTable->selectionModel();

	if (selectionModel->hasSelection()) {
		for (const auto& selectedIndex : selectionModel->selectedIndexes()) {
			m_model->setData(selectedIndex, QApplication::clipboard()->text());
		}

		m_model->dataChanged(selectionModel->selectedIndexes().first(), selectionModel->selectedIndexes().last());
		ui.mainTable->setFocus();
	}
}
