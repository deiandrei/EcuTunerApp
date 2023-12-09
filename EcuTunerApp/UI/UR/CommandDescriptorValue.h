#ifndef	COMMANDDESCRIPTORVALUE_H
#define COMMANDDESCRIPTORVALUE_H

#include "URManager.h"
#include "../../Ecu/File.h"
#include "../../Descriptor/Descriptor.h"

class CommandDescriptorValue : public IURCommand {
	public:
		CommandDescriptorValue(EcuTuner::EcuFile* ecuFile, EcuTuner::Descriptor* descriptor, int row, int column, double newValue, double oldValue);

		void Undo() override;
		//void Redo() override;

	protected:
		EcuTuner::EcuFile* m_ecuFile;
		EcuTuner::Descriptor* m_descriptor;
		int m_row, m_column;
		double m_newValue;
		double m_oldValue;

};

#endif
