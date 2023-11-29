#include "DescriptorReader.h"

#include <QFile>
#include <QMessageBox>

namespace EcuTuner {

	QList<Descriptor*> DescriptorReader::ReadDescriptorsFromFile(const QString& fileName) {
		QFile* xmlFile = new QFile(fileName);
		if (!xmlFile->open(QIODevice::ReadOnly | QIODevice::Text)) {
			QMessageBox::critical(0, "Load XML File Problem", "Couldn't open xmlfile.xml to load settings for download", QMessageBox::Ok);
			return {};
		}

		QList<Descriptor*> descriptors;
		QDomDocument* xmlDoc = new QDomDocument();
		xmlDoc->setContent(xmlFile);
		xmlFile->close();

		QDomElement rootNode = xmlDoc->documentElement();
		if (rootNode.tagName() == "DescriptorPack") {
			for (QDomElement element = rootNode.firstChildElement(); !element.isNull(); element = element.nextSiblingElement()) {
				
				if (element.tagName() == "Descriptor") {
					QDomElement nameElement = element.firstChildElement("Name");
					QDomElement aliasElement = element.firstChildElement("Alias");

					if (nameElement.isNull() || aliasElement.isNull() || nameElement.text().isEmpty() || aliasElement.text().isEmpty()) continue;

					Descriptor* descriptor = new Descriptor;
					descriptor->Name = nameElement.text();
					descriptor->Alias = aliasElement.text();
					descriptor->AxisData = new DataAxis;

					QDomElement dataAxisElement = element.firstChildElement("AxisData");
					if (!dataAxisElement.isNull()) {
						ParseAxisElement(&dataAxisElement, descriptor->AxisData);

						if(!dataAxisElement.firstChildElement("Rows").isNull()) descriptor->AxisData->Rows = dataAxisElement.firstChildElement("Rows").text().toInt();
						if(!dataAxisElement.firstChildElement("Columns").isNull()) descriptor->AxisData->Columns = dataAxisElement.firstChildElement("Columns").text().toInt();
						if(!dataAxisElement.firstChildElement("InverseMap").isNull()) descriptor->AxisData->InverseMap = dataAxisElement.firstChildElement("InverseMap").text().toInt() ? 1 : 0;
					}

					QDomElement xAxisElement = element.firstChildElement("AxisX");
					if (!xAxisElement.isNull()) {
						descriptor->AxisX = new Axis;
						ParseAxisElement(&xAxisElement, descriptor->AxisX);
					}

					QDomElement yAxisElement = element.firstChildElement("AxisY");
					if (!yAxisElement.isNull()) {
						descriptor->AxisY = new Axis;
						ParseAxisElement(&yAxisElement, descriptor->AxisY);
					}

					descriptors.push_back(descriptor);
				}
			}
		}

		return descriptors;
	}

	void DescriptorReader::ParseAxisElement(QDomElement* element, EcuTuner::Axis* axis) {
		if (!element->firstChildElement("Offset").isNull()) axis->Offset = element->firstChildElement("Offset").text().toInt();
		if (!element->firstChildElement("BlockSize").isNull()) axis->BlockSize = element->firstChildElement("BlockSize").text().toInt();
		if (!element->firstChildElement("ValueFactor").isNull()) axis->ValueFactor = element->firstChildElement("ValueFactor").text().toDouble();
		if (!element->firstChildElement("ValueOffset").isNull()) axis->ValueOffset = element->firstChildElement("ValueOffset").text().toDouble();
	}

	double DescriptorReader::ReadDataAxisValue(ByteBuffer* bb, DataAxis* dataAxis, int row, int column) {
		int dataPos = dataAxis->GetTableOffset(row, column);

		double readData = 0.0f;
		if (dataAxis->BlockSize == 1) {
			readData = bb->Read<byte1>(dataPos);
		}
		else if (dataAxis->BlockSize == 2) {
			readData = bb->Read<uint2>(dataPos);
		}
		else if (dataAxis->BlockSize == 4) {
			readData = bb->Read<uint4>(dataPos);
		}

		return dataAxis->Parse(readData);
	}

	double DescriptorReader::ReadAxisValue(ByteBuffer* bb, Axis* axis, int index) {
		int dataPos = axis->GetOffset(index);

		double readData = 0.0f;
		if (axis->BlockSize == 1) {
			readData = bb->Read<byte1>(dataPos);
		}
		else if (axis->BlockSize == 2) {
			readData = bb->Read<uint2>(dataPos);
		}
		else if (axis->BlockSize == 4) {
			readData = bb->Read<uint4>(dataPos);
		}

		return axis->Parse(readData);
	}

	void DescriptorReader::SetDataAxisValue(ByteBuffer* bb, DataAxis* dataAxis, int row, int column, double value) {
		int dataPos = dataAxis->GetTableOffset(row, column);

		if (dataAxis->BlockSize == 1) {
			byte1 valueToSet = round(dataAxis->UnParse(value));
			memcpy(bb->Data() + dataPos, &valueToSet, dataAxis->BlockSize);
		}
		else if (dataAxis->BlockSize == 2) {
			uint2 valueToSet = dataAxis->UnParse(value);
			memcpy(bb->Data() + dataPos, &valueToSet, dataAxis->BlockSize);
		}
		else if (dataAxis->BlockSize == 4) {
			uint4 valueToSet = dataAxis->UnParse(value);
			memcpy(bb->Data() + dataPos, &valueToSet, dataAxis->BlockSize);
		}
	}

}
