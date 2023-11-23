#pragma once

#include <QMdiSubWindow>
#include "ui_ViewDescriptorForm.h"

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
		Ui::ViewDescriptorFormClass ui;

		EcuTuner::EcuFile* m_ecuFile;
		EcuTuner::Descriptor* m_descriptor;

};
