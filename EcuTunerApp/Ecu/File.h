#ifndef	ECUFILE_H
#define ECUFILE_H

#include "../Common/ByteBuffer.h"

namespace EcuTuner {

	class EcuFile {
		public:
			EcuFile(ByteBuffer* buffer);

			ByteBuffer* GetBuffer() { return m_buffer; }

		protected:
			ByteBuffer* m_buffer;

	};

}

#endif
