#include "CommandDescriptorValue.h"

#include "../../Descriptor/DescriptorReader.h"

CommandDescriptorValue::CommandDescriptorValue(EcuTuner::EcuFile* ecuFile, EcuTuner::Descriptor* descriptor, int row, int column, double newValue, double oldValue) :
	m_ecuFile(ecuFile),
	m_descriptor(descriptor),
	m_row(row),
	m_column(column),
	m_newValue(newValue),
	m_oldValue(oldValue)
{
}

void CommandDescriptorValue::Undo() {
	EcuTuner::DataAxis* dataAxis = m_descriptor->AxisData;
	ByteBuffer* bb = m_ecuFile->GetBuffer();

	if (dataAxis->InverseMap) EcuTuner::DescriptorReader::SetDataAxisValue(bb, dataAxis, m_column, m_row, m_oldValue);
	else EcuTuner::DescriptorReader::SetDataAxisValue(bb, dataAxis, m_row, m_column, m_oldValue);
}
