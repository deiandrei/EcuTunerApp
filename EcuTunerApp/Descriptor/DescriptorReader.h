#ifndef DESCRIPTORREADER_H
#define	DESCRIPTORREADER_H

#include <QList>
#include <QtXml>

#include "../Common/ByteBuffer.h"
#include "Descriptor.h"

namespace EcuTuner {

	class DescriptorReader {
		public:
			static QList<Descriptor*> ReadDescriptorsFromFile(const QString& fileName);
			static void WriteDescriptorsToFile(QList<Descriptor*>& descriptors, const QString& fileName);

			static double ReadDataAxisValue(ByteBuffer* bb, DataAxis* dataAxis, int row, int column);
			static double ReadAxisValue(ByteBuffer* bb, Axis* axis, int index);

			static void SetDataAxisValue(ByteBuffer* bb, DataAxis* dataAxis, int row, int column, double value);

		private:
			static void ParseAxisElement(QDomElement* element, EcuTuner::Axis* axis);

			static void AddElementWithName(QDomDocument* doc, QDomElement* parent, QString tagName, QString value);
			static void AddAxisElements(QDomDocument* doc, QDomElement* parent, Axis* axis);

	};

}

#endif
