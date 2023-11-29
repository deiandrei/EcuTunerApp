#pragma once

#include <fstream>
#include <qbytearray.h>
#include <QString>

#include "ByteBuffer.h"

namespace EcuTuner {

	class Utils {
		public:
			static QByteArray ByteBufferToQByteArray(ByteBuffer* bb);
			static ByteBuffer* ReadFileIntoByteBuffer(std::ifstream& fileStream);

			static QString FormatPrecision(double value, unsigned int precission);
	};

}
