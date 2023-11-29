#include "DescriptorConverter.h"

#include <QFile>
#include <QTextStream>
#include <QMessageBox>

namespace EcuTuner {

	QList<Descriptor*> DescriptorConverter::ConvertFromCsv(const QString& csvFile) {
		QList<Descriptor*> descriptors;
		QStringList header;
		QList<QStringList> csvData;

		QFile* csvFilePtr = new QFile(csvFile);
		if (!csvFilePtr->open(QIODevice::ReadOnly | QIODevice::Text)) {
			QMessageBox::critical(0, "Load CSV File Problem", "Couldn't open csv file to load settings for download", QMessageBox::Ok);
			return {};
		}

		QTextStream reader(csvFilePtr);
		header = reader.readLine().split(";");
		if (!header.isEmpty()) {
			while (!reader.atEnd())
			{
				QString line = reader.readLine();
				csvData.push_back(line.split(";"));
				int x = 0;
			}
		}

		csvFilePtr->close();

		if (!csvData.isEmpty()) {
			// Main data columns
			int nameId = header.indexOf("Name");
			int aliasId = header.indexOf("IdName");
			int rowsId = header.indexOf("Rows");
			int columnsId = header.indexOf("Columns");
			int dataAxisValueFactorId = header.indexOf("Fieldvalues.Factor");
			int dataAxisValueOffsetId = header.indexOf("Fieldvalues.Offset");
			int dataAxisOffsetId = header.indexOf("Fieldvalues.StartAddr");
			int dataAxisByteCountId = header.indexOf("DataOrg");
			int dataAxisTypeId = header.indexOf("Type");
			int dataAxisPrecisionId = header.indexOf("Precision");

			if (nameId == -1 || aliasId == -1 || rowsId == -1 || columnsId == -1 || dataAxisOffsetId == -1 || dataAxisValueFactorId == -1 || dataAxisValueOffsetId == -1 || dataAxisByteCountId == -1 || dataAxisTypeId == -1) return {};

			// Axis columns

			// Axis X
			int axisXValueFactorId = header.indexOf("AxisX.Factor");
			int axisXValueOffsetId = header.indexOf("AxisX.Offset");
			int axisXOffsetId = header.indexOf("AxisX.DataAddr");
			int axisXByteCountId = header.indexOf("AxisX.DataOrg");
			int axisXPrecisionId = header.indexOf("AxisX.Precision");

			bool loadAxisX = !(axisXOffsetId == -1 || axisXValueFactorId == -1 || axisXValueOffsetId == -1);

			// Axis Y
			int axisYValueFactorId = header.indexOf("AxisY.Factor");
			int axisYValueOffsetId = header.indexOf("AxisY.Offset");
			int axisYOffsetId = header.indexOf("AxisY.DataAddr");
			int axisYByteCountId = header.indexOf("AxisY.DataOrg");
			int axisYPrecisionId = header.indexOf("AxisY.Precision");

			bool loadAxisY = !(axisYOffsetId == -1 || axisYValueFactorId == -1 || axisYValueOffsetId == -1);

			// Load data
			for (QStringList descriptorData : csvData) {
				Descriptor* descriptor = new Descriptor;

				bool conversionStatus = false;
				int offset = descriptorData[dataAxisOffsetId].mid(1).toInt(&conversionStatus, 16);
				if (!conversionStatus || offset == 0) continue;

				descriptor->Name = descriptorData[nameId];
				descriptor->Alias = descriptorData[aliasId];

				// Load AxisData
				descriptor->AxisData = new DataAxis;
				descriptor->AxisData->BlockSize = ParseByteCount(descriptorData[dataAxisByteCountId]);
				descriptor->AxisData->InverseMap = descriptorData[dataAxisTypeId] == "eZweiInv";
				descriptor->AxisData->Offset = offset;
				descriptor->AxisData->Rows = descriptorData[rowsId].toInt();
				descriptor->AxisData->Columns = descriptorData[columnsId].toInt();
				descriptor->AxisData->ValueFactor = descriptorData[dataAxisValueFactorId].toDouble();
				descriptor->AxisData->ValueOffset = descriptorData[dataAxisValueOffsetId].toDouble();
				if (dataAxisPrecisionId != -1) descriptor->AxisData->Precision = std::max(descriptorData[dataAxisPrecisionId].toInt(), 0);

				// Load AxisX
				if (loadAxisX) {
					conversionStatus = false;
					offset = descriptorData[axisXOffsetId].mid(1).toInt(&conversionStatus, 16);
					if (conversionStatus && offset != 0) {
						descriptor->AxisX = new Axis;
						descriptor->AxisX->Offset = offset;
						descriptor->AxisX->BlockSize = ParseByteCount(descriptorData[axisXByteCountId]);
						descriptor->AxisX->ValueFactor = descriptorData[axisXValueFactorId].toDouble();
						descriptor->AxisX->ValueOffset = descriptorData[axisXValueOffsetId].toDouble();
						if (axisXPrecisionId != -1) descriptor->AxisX->Precision = std::max(descriptorData[axisXPrecisionId].toInt(), 0);
					}
				}

				// Load AxisY
				if (loadAxisY) {
					conversionStatus = false;
					offset = descriptorData[axisYOffsetId].mid(1).toInt(&conversionStatus, 16);
					if (conversionStatus && offset != 0) {
						descriptor->AxisY = new Axis;
						descriptor->AxisY->Offset = offset;
						descriptor->AxisY->BlockSize = ParseByteCount(descriptorData[axisYByteCountId]);
						descriptor->AxisY->ValueFactor = descriptorData[axisYValueFactorId].toDouble();
						descriptor->AxisY->ValueOffset = descriptorData[axisYValueOffsetId].toDouble();
						if (axisYPrecisionId != -1) descriptor->AxisY->Precision = std::max(descriptorData[axisYPrecisionId].toInt(), 0);
					}
				}
				
				descriptors.push_back(descriptor);
			}
		}

		return descriptors;
	}

	int DescriptorConverter::ParseByteCount(const QString& input) {
		if (input == "eByte") return 1;
		else if (input == "eLoHi") return 2;

		return 1;
	}

}
