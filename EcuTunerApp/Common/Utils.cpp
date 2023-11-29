#include "Utils.h"

namespace EcuTuner {

	QByteArray Utils::ByteBufferToQByteArray(ByteBuffer* bb) {
		return QByteArray((char*)bb->Data(), bb->Size());
	}

	ByteBuffer* Utils::ReadFileIntoByteBuffer(std::ifstream& fileStream) {
        if (fileStream.is_open()) {
            fileStream.seekg(0, std::ios::end);
            size_t size = fileStream.tellg();
            fileStream.seekg(0);

            byte1* tempDataPtr = new byte1[size];
            fileStream.read((char*)tempDataPtr, size);

            return (new ByteBuffer(tempDataPtr, size));
        }

        return nullptr;
	}

    QString Utils::FormatPrecision(double value, unsigned int precission) {
        QString tempStr = QString::number(value, 'f', precission);

        int dotPos = tempStr.indexOf(".");

        if (dotPos == -1) {
            return tempStr + (precission > 0 ? "." : "") + QString().fill('0', precission);
        } else {
            return tempStr + QString().fill('0', precission - (tempStr.length() - dotPos - 1));
            
        }

        return tempStr;
    }

}
