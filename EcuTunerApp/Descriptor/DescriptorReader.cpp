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
					QDomElement folderElement = element.firstChildElement("Folder");

					if (nameElement.isNull() || aliasElement.isNull() || nameElement.text().isEmpty() || aliasElement.text().isEmpty()) continue;

					Descriptor* descriptor = new Descriptor;
					descriptor->Name = nameElement.text();
					descriptor->Alias = aliasElement.text();
					if (!folderElement.isNull()) descriptor->Folder = folderElement.text();
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

		delete xmlFile;

		return descriptors;
	}

	void DescriptorReader::WriteDescriptorsToFile(QList<Descriptor*>& descriptors, const QString& fileName) {
		QFile* file = new QFile(fileName);
		if (!file->open(QIODevice::WriteOnly | QIODevice::Text))
		{
			QMessageBox::critical(0, "Save XML File Problem", "Couldn't save file", QMessageBox::Ok);
		}

		QDomDocument* doc = new QDomDocument();
		QDomElement rootNode = doc->createElement("DescriptorPack");
		doc->appendChild(rootNode);
		
		for (auto descriptor : descriptors) {
			QDomElement descriptorElement = doc->createElement("Descriptor");
			rootNode.appendChild(descriptorElement);

			AddElementWithName(doc, &descriptorElement, "Name", descriptor->Name);
			AddElementWithName(doc, &descriptorElement, "Alias", descriptor->Alias);
			AddElementWithName(doc, &descriptorElement, "Folder", descriptor->Folder);

			QDomElement dataAxisElement = doc->createElement("AxisData");
			descriptorElement.appendChild(dataAxisElement);
			AddAxisElements(doc, &dataAxisElement, descriptor->AxisData);
			AddElementWithName(doc, &dataAxisElement, "Rows", QString::number(descriptor->AxisData->Rows));
			AddElementWithName(doc, &dataAxisElement, "Columns", QString::number(descriptor->AxisData->Columns));
			AddElementWithName(doc, &dataAxisElement, "InverseMap", QString::number((int)descriptor->AxisData->InverseMap));

			if (descriptor->AxisX) {
				QDomElement axisXElement = doc->createElement("AxisX");
				descriptorElement.appendChild(axisXElement);
				AddAxisElements(doc, &axisXElement, descriptor->AxisX);
			}
			if (descriptor->AxisY) {
				QDomElement axisYElement = doc->createElement("AxisY");
				descriptorElement.appendChild(axisYElement);
				AddAxisElements(doc, &axisYElement, descriptor->AxisY);
			}
		}

		QTextStream stream(file);
		stream.setCodec("UTF-8");
		stream << doc->toString();
		file->close();

		delete file;
	}

	void DescriptorReader::ParseAxisElement(QDomElement* element, EcuTuner::Axis* axis) {
		if (!element->firstChildElement("Offset").isNull()) axis->Offset = element->firstChildElement("Offset").text().toInt();
		if (!element->firstChildElement("BlockSize").isNull()) axis->BlockSize = element->firstChildElement("BlockSize").text().toInt();
		if (!element->firstChildElement("ValueFactor").isNull()) axis->ValueFactor = element->firstChildElement("ValueFactor").text().toDouble();
		if (!element->firstChildElement("ValueOffset").isNull()) axis->ValueOffset = element->firstChildElement("ValueOffset").text().toDouble();
		if (!element->firstChildElement("Precision").isNull()) axis->Precision = element->firstChildElement("Precision").text().toDouble();
	}

	void DescriptorReader::AddAxisElements(QDomDocument* doc, QDomElement* parent, Axis* axis) {
		AddElementWithName(doc, parent, "Offset", QString::number(axis->Offset));
		AddElementWithName(doc, parent, "BlockSize", QString::number(axis->BlockSize));
		AddElementWithName(doc, parent, "ValueFactor", QString::number(axis->ValueFactor));
		AddElementWithName(doc, parent, "ValueOffset", QString::number(axis->ValueOffset));
		AddElementWithName(doc, parent, "Precision", QString::number(axis->Precision));
	}

	void DescriptorReader::AddElementWithName(QDomDocument* doc, QDomElement* parent, QString tagName, QString value) {
		QDomElement folderElement = doc->createElement(tagName);
		folderElement.appendChild(doc->createTextNode(value));
		parent->appendChild(folderElement);
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
