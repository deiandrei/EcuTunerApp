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

		int GetOffset(int index) {
			return index;
		}
	};

	struct DataAxis : Axis {
		int Rows;
		int Columns;

		bool InverseMap;

		DataAxis() {
			Rows = Columns = 0;
			InverseMap = false;
		}

		int GetTableOffset(int row, int column) {
			return (InverseMap ? (Rows * column + row) : (Columns * row + column));
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

			

	};

}

#endif
