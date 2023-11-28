#ifndef DESCRIPTORCONVERTER_H
#define	DESCRIPTORCONVERTER_H

#include <QString>
#include "Descriptor.h"

namespace EcuTuner {

	class DescriptorConverter {
		public:
			static QList<Descriptor*> ConvertFromCsv(const QString& csvFile);

		private:
			static int ParseByteCount(const QString& input);

	};

}

#endif
