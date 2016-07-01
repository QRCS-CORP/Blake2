#ifndef _BLAKE2_INTUTILS_H
#define _BLAKE2_INTUTILS_H

#include "Config.h"

namespace Blake2
{
	class IntUtils
	{
	public:

		static inline uint32_t BytesToLe32(const std::vector<uint8_t> &Input, const size_t InOffset)
		{
#if defined(IS_LITTLE_ENDIAN)
			uint32_t value = 0;
			memcpy(&value, &Input[InOffset], sizeof(uint32_t));
			return value;
#else
			return
				(static_cast<uint>(Input[InOffset]) |
				(static_cast<uint>(Input[InOffset + 1] << 8)) |
				(static_cast<uint>(Input[InOffset + 2] << 16)) |
				(static_cast<uint>(Input[InOffset + 3] << 24)));
#endif
		}

		static inline uint64_t BytesToLe64(const std::vector<uint8_t> &Input, const size_t InOffset)
		{
#if defined(IS_LITTLE_ENDIAN)
			uint64_t value = 0;
			memcpy(&value, &Input[InOffset], sizeof(uint64_t));
			return value;
#else
			return
				((uint64_t)Input[InOffset]) |
				((uint64_t)Input[InOffset + 1] << 8) |
				((uint64_t)Input[InOffset + 2] << 16) |
				((uint64_t)Input[InOffset + 3] << 24) |
				((uint64_t)Input[InOffset + 4] << 32) |
				((uint64_t)Input[InOffset + 5] << 40) |
				((uint64_t)Input[InOffset + 6] << 48) |
				((uint64_t)Input[InOffset + 7] << 56);
#endif
		}

		template <typename T>
		static inline void ClearVector(std::vector<T> &Obj)
		{
			if (Obj.capacity() == 0)
				return;

			memset(Obj.data(), 0, Obj.capacity() * sizeof(T));
			Obj.clear();
		}

		static inline void Le32ToBytes(const uint32_t Value, std::vector<uint8_t> &Output, const size_t OutOffset)
		{
#if defined(IS_LITTLE_ENDIAN)
			memcpy(&Output[OutOffset], &Value, sizeof(uint32_t));
#else
			Output[OutOffset] = static_cast<uint8_t>(Value);
			Output[OutOffset + 1] = static_cast<uint8_t>(Value >> 8);
			Output[OutOffset + 2] = static_cast<uint8_t>(Value >> 16);
			Output[OutOffset + 3] = static_cast<uint8_t>(Value >> 24);
#endif
		}

		static inline void Le64ToBytes(const uint64_t Value, std::vector<uint8_t> &Output, const size_t OutOffset)
		{
#if defined(IS_LITTLE_ENDIAN)
			memcpy(&Output[OutOffset], &Value, sizeof(uint64_t));
#else
			Output[OutOffset] = static_cast<uint8_t>(Value);
			Output[OutOffset + 1] = static_cast<uint8_t>(Value >> 8);
			Output[OutOffset + 2] = static_cast<uint8_t>(Value >> 16);
			Output[OutOffset + 3] = static_cast<uint8_t>(Value >> 24);
			Output[OutOffset + 4] = static_cast<uint8_t>(Value >> 32);
			Output[OutOffset + 5] = static_cast<uint8_t>(Value >> 40);
			Output[OutOffset + 6] = static_cast<uint8_t>(Value >> 48);
			Output[OutOffset + 7] = static_cast<uint8_t>(Value >> 56);
#endif
		}

		static inline void Le256ToBlock(std::vector<uint32_t> &Input, std::vector<uint8_t> &Output, size_t OutOffset)
		{
	#if defined(IS_LITTLE_ENDIAN)
			memcpy(&Output[OutOffset], &Input[0], Input.size() * sizeof(uint32_t));
	#else
			Le32ToBytes(Input[0], Output, OutOffset);
			Le32ToBytes(Input[1], Output, OutOffset + 4);
			Le32ToBytes(Input[2], Output, OutOffset + 8);
			Le32ToBytes(Input[3], Output, OutOffset + 12);
			Le32ToBytes(Input[4], Output, OutOffset + 16);
			Le32ToBytes(Input[5], Output, OutOffset + 20);
			Le32ToBytes(Input[6], Output, OutOffset + 24);
			Le32ToBytes(Input[7], Output, OutOffset + 28);
	#endif
		}

		static inline void Le512ToBlock(std::vector<uint64_t> &Input, std::vector<uint8_t> &Output, size_t OutOffset)
		{
#if defined(IS_LITTLE_ENDIAN)
			memcpy(&Output[OutOffset], &Input[0], Input.size() * sizeof(uint64_t));
#else
			Le64ToBytes(Input[0], Output, OutOffset);
			Le64ToBytes(Input[1], Output, OutOffset + 8);
			Le64ToBytes(Input[2], Output, OutOffset + 16);
			Le64ToBytes(Input[3], Output, OutOffset + 24);
			Le64ToBytes(Input[4], Output, OutOffset + 32);
			Le64ToBytes(Input[5], Output, OutOffset + 40);
			Le64ToBytes(Input[6], Output, OutOffset + 48);
			Le64ToBytes(Input[7], Output, OutOffset + 56);
#endif
		}

	};
}
#endif