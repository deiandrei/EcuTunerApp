#ifndef DESCRIPTOR_H
#define DESCRIPTOR_H

#include <QString>

namespace EcuTuner {

	struct Axis {
		int Offset;
		
		double ValueFactor;
		double ValueOffset;

		Axis() {
			Offset = 0;
			
			ValueFactor = 1.0;
			ValueOffset = 0.0;
		}

		double Parse(double input) {
			return (input * ValueFactor) + ValueOffset;
		}

		double UnParse(double input) {
			return (input - ValueOffset) / ValueFactor;
		}
	};

	struct DataAxis : Axis {
		int Rows;
		int Columns;

		DataAxis() {
			Rows = Columns = 0;
		}
	};

	class Descriptor {
		public:
			Descriptor();

			DataAxis* AxisData;
			Axis* AxisX;
			Axis* AxisY;

			QString Name;
			QString Alias;

			bool InverseMap;

	};

}

#endif
