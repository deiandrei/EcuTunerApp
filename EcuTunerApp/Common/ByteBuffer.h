#ifndef BYTEBUFFER_H
#define BYTEBUFFER_H

#include "../defines.h"

class ByteBuffer {
	public:
		ByteBuffer(byte1* buffer, uint4 size, bool copyBytes = false);
		~ByteBuffer();
		
		template<typename T>
		T Read() {
			if (sizeof(T) + mPosition > mSize) {
				mPosition = mSize;
				return 0;
			}

			T temp;
			memcpy(&temp, mByteContainer + mPosition, sizeof(T));
			mPosition += sizeof(T);
			return temp;
		}

		template<typename T>
		void Read(T* dest) {
			if (sizeof(T) + mPosition > mSize) {
				mPosition = mSize;
				return;
			}

			memcpy(dest, mByteContainer + mPosition, sizeof(T));
			mPosition += sizeof(T);
		}

		template<typename T>
		void ReadArray(T* destStartPtr, uint4 count) {
			if (count == 0) return;
			if ((sizeof(T) * count + mPosition) > mSize) {
				mPosition = mSize;
				return;
			}

			memcpy(destStartPtr, mByteContainer + mPosition, sizeof(T) * count);
			mPosition += sizeof(T) * count;
		}

		bool EndReached() { return mPosition >= mSize; }

		uint4& Position() { return mPosition; }
		uint4 Size() { return mSize; }
		uint4 BytesRemaining() { return mSize - mPosition; }

		byte1* Data() { return mByteContainer; }

		static bool GetBitFromByte(int bitId, const byte1& byte);

	private:
		byte1* mByteContainer;
		uint4 mPosition, mSize;

};

#endif
