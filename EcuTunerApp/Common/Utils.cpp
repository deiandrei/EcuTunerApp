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
}
