#ifndef _BLAKE2TEST_HEXCONVERTER_H
#define _BLAKE2TEST_HEXCONVERTER_H

#include <string.h>
#include <vector>

namespace BlakeTest
{
	/// <summary>
	/// A Hexadecimal conversion helper class
	/// </summary>
	class HexConverter
	{
	private:
		static constexpr uint8_t m_encBytes[16] =
		{
			(uint8_t)'0', (uint8_t)'1', (uint8_t)'2', (uint8_t)'3', (uint8_t)'4', (uint8_t)'5', (uint8_t)'6', (uint8_t)'7',
			(uint8_t)'8', (uint8_t)'9', (uint8_t)'a', (uint8_t)'b', (uint8_t)'c', (uint8_t)'d', (uint8_t)'e', (uint8_t)'f'
		};

	public:
		static void Decode(const std::string &Input, std::vector<uint8_t> &Output)
		{
			size_t end = Input.size();
			Output.resize(end / 2, 0);

			while (end > 0)
			{
				if (!Ignore(Input[end - 1]))
					break;

				end--;
			}

			size_t i = 0;
			size_t j = 0;
			size_t length = 0;
			std::vector<uint8_t> decTable = GetDecodingTable();

			while (i < end)
			{
				while (i < end && Ignore(Input[i]))
					i++;

				uint8_t b1 = decTable[Input[i++]];

				while (i < end && Ignore(Input[i]))
					i++;

				uint8_t b2 = decTable[Input[i++]];
				Output[j++] = (uint8_t)((b1 << 4) | b2);
				length++;
			}
		}

		static void Decode(const std::vector<std::string> &Input, std::vector<std::vector<uint8_t>> &Output)
		{
			Output.clear();

			for (size_t i = 0; i < Input.size(); ++i)
			{
				const std::string str = Input[i];
				std::vector<uint8_t> temp;
				Decode(str, temp);
				Output.push_back(temp);
			}
		}

		static void Decode(const char *InputArray[], size_t Length, std::vector<std::vector<uint8_t>> &OutputArray)
		{
			OutputArray.reserve(Length);

			for (size_t i = 0; i < Length; ++i)
			{
				std::string encoded = InputArray[i];
				std::vector<uint8_t> decoded;
				Decode(encoded, decoded);
				OutputArray.push_back(decoded);
			}
		}

		static void Encode(const std::vector<uint8_t> &Input, size_t Offset, size_t Length, std::vector<uint8_t> &Output)
		{
			Output.resize(Length * 2, 0);
			size_t counter = 0;
			std::vector<uint8_t> encTable = GetEncodingTable();

			for (size_t i = Offset; i < (Offset + Length); i++)
			{
				int v = Input[i];
				Output[counter++] = encTable[v >> 4];
				Output[counter++] = encTable[v & 0xf];
			}
		}

		static bool Ignore(char Value)
		{
			return (Value == '\n' || Value == '\r' || Value == '\t' || Value == ' ');
		}

		static void ToString(const std::vector<uint8_t> &Input, std::string &Output)
		{
			std::vector<uint8_t> encoded;
			Encode(Input, 0, Input.size(), encoded);
			Output.assign((char*)&encoded[0], encoded.size());
		}

	private:
		static std::vector<uint8_t> GetEncodingTable()
		{
			std::vector<uint8_t> encTable;
			encTable.reserve(sizeof(m_encBytes));
			for (size_t i = 0; i < sizeof(m_encBytes); ++i)
				encTable.push_back(m_encBytes[i]);

			return encTable;
		}

		static std::vector<uint8_t> GetDecodingTable()
		{
			std::vector<uint8_t> encTable = GetEncodingTable();
			std::vector<uint8_t> decTable(128, 0);

			for (size_t i = 0; i < encTable.size(); i++)
				decTable[encTable[i]] = (uint8_t)i;

			decTable['A'] = decTable['a'];
			decTable['B'] = decTable['b'];
			decTable['C'] = decTable['c'];
			decTable['D'] = decTable['d'];
			decTable['E'] = decTable['e'];
			decTable['F'] = decTable['f'];

			return decTable;
		}
	};
}

#endif

