#pragma once

#include <QMdiSubWindow>
#include "ui_ViewDescriptorForm.h"

#include "UI/DescriptorTableModel.h"
#include "Descriptor/Descriptor.h"
#include "Ecu/File.h"

class ViewDescriptorForm : public QWidget
{
	Q_OBJECT

	public:
		ViewDescriptorForm(EcuTuner::EcuFile* file, EcuTuner::Descriptor* descriptor, QWidget* parent = nullptr);
		~ViewDescriptorForm();

		EcuTuner::Descriptor* GetDescriptor() { return m_descriptor; }

	private:
		void IncrementSelectedIndexes(double factor);

	protected slots:
		void CopySelectedIndex();
		void PasteSelectedIndex();

	private:
		Ui::ViewDescriptorFormClass ui;
		DescriptorTableModel* m_model;

		EcuTuner::EcuFile* m_ecuFile;
		EcuTuner::Descriptor* m_descriptor;

};
