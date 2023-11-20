#include "ByteBuffer.h"

ByteBuffer::ByteBuffer(byte1* buffer, uint4 size, bool copyBytes) {
	mPosition = 0;
	mSize = size;

	if (copyBytes) {
		mByteContainer = new byte1[size];
		memcpy(mByteContainer, buffer, size);
	}
	else {
		mByteContainer = buffer;
	}
}

ByteBuffer::~ByteBuffer() {
	delete[] mByteContainer;
}

bool ByteBuffer::GetBitFromByte(int bitId, const byte1& byte) {
	return ((byte >> bitId) & 1);
}
