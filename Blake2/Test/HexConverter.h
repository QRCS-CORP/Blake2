#ifndef _CEXTEST_HEXCONVERTER_H
#define _CEXTEST_HEXCONVERTER_H

#include "../Blake2/CexDomain.h"

namespace Test
{
	/// <summary>
	/// A Hexadecimal conversion helper class
	/// </summary>
	class HexConverter
	{
	public:
		static void Decode(const std::string &Input, std::vector<byte> &Output);
		static void Decode(const std::vector<std::string> &Input, std::vector<std::vector<byte>> &Output);
		static void Decode(const char *Input[], size_t Length, std::vector<std::vector<byte>> &Output);
		static void Encode(const std::vector<byte> &Input, size_t Offset, size_t Length, std::vector<byte> &Output);
		static bool Ignore(char Value);
		static void ToString(const std::vector<byte> &Input, std::string &Output);

	private:
		static const byte ENCODING_TABLE[16];
		static std::vector<byte> GetEncodingTable();
		static std::vector<byte> GetDecodingTable();
	};
}

#endif

