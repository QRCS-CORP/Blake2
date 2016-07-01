// The MIT License (MIT)
// 
// Copyright (c) 2016 vtdev.com
// This file is part of the CEX Cryptographic library.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef _BLAKE2_BLAKECONFIG_H
#define _BLAKE2_BLAKECONFIG_H

#include "Common.h"
#include "BlakeTree.h"
#include "CryptoDigestException.h"
#include "IntUtils.h"

/// <summary>
/// 
/// </summary> 
struct BlakeConfig
{
private:
	std::vector<byte>  m_key;
	uint m_outputSizeInBytes;
	std::vector<byte> m_personalization;
	std::vector<byte>  m_salt;

public:

	/// <summary>
	/// 
	/// </summary>
	const std::vector<byte> &Key() { return m_key; }
	void Key(std::vector<byte> &Value) { m_key = Value; }

	/// <summary>
	/// 
	/// </summary>
	const uint &OutputSizeInBytes() { return m_outputSizeInBytes; }

	/// <summary>
	/// 
	/// </summary>
	const std::vector<byte> &Personalization() { return m_personalization; }
	void Personalization(std::vector<byte> &Value) { m_personalization = Value; }

	/// <summary>
	/// 
	/// </summary>
	const std::vector<byte> &Salt() { return m_salt; }
	void Salt(std::vector<byte> &Value) { m_salt = Value; }

	/// <summary>
	/// Initialize this class
	/// </summary>
	BlakeConfig()
		:
		m_key(0),
		m_outputSizeInBytes(64),
		m_personalization(0),
		m_salt(0)
	{
	}

	/// <summary>
	/// Clone the structure
	/// </summary>
	BlakeConfig Clone()
	{
		BlakeConfig result;
		result.m_key = Key();
		result.m_outputSizeInBytes = OutputSizeInBytes();
		result.m_personalization = Personalization();
		result.m_salt = Salt();
		return result;
	}

	static std::vector<ulong> ConfigB(BlakeConfig config, BlakeTree treeConfig)
	{
		bool isSequential = treeConfig.IntermediateHashSize() == 0;
		std::vector<ulong> rawConfig(8, 0);
		std::vector<ulong> result(8, 0);

		//digest length
		if (config.OutputSizeInBytes() <= 0 || config.OutputSizeInBytes() > 64)
			throw CEX::Exception::CryptoDigestException("");
		rawConfig[0] |= (ulong)(uint)config.OutputSizeInBytes();

		//Key length
		if (config.Key().size() != 0)
		{
			if (config.Key().size() > 64)
				throw CEX::Exception::CryptoDigestException("");
			rawConfig[0] |= (ulong)((uint)config.Key().size() << 8);
		}
		// FanOut
		rawConfig[0] |= (uint)treeConfig.FanOut() << 16;
		// Depth
		rawConfig[0] |= (uint)treeConfig.MaxHeight() << 24;
		// Leaf length
		rawConfig[0] |= ((ulong)(uint)treeConfig.LeafSize()) << 32;
		// Inner length
		if (!isSequential && (treeConfig.IntermediateHashSize() <= 0 || treeConfig.IntermediateHashSize() > 64))
			throw CEX::Exception::CryptoDigestException("Key is too long");

		rawConfig[2] |= (uint)treeConfig.IntermediateHashSize() << 8;
		// Salt
		if (config.Salt().size() != 0)
		{
			if (config.Salt().size() != 16)
				throw CEX::Exception::CryptoDigestException("Salt has invalid length");
			rawConfig[4] = BytesToUInt64(config.Salt(), 0);
			rawConfig[5] = BytesToUInt64(config.Salt(), 8);
		}
		// Personalization
		if (config.Personalization().size() != 0)
		{
			if (config.Personalization().size() != 16)
				throw CEX::Exception::CryptoDigestException("Personalization too long");
			rawConfig[6] = BytesToUInt64(config.Personalization(), 0);
			rawConfig[7] = BytesToUInt64(config.Personalization(), 8);
		}

		return rawConfig;
	}

	static void ConfigBSetNode(std::vector<ulong> &rawConfig, byte depth, ulong nodeOffset)
	{
		rawConfig[1] = nodeOffset;
		rawConfig[2] = (rawConfig[2] & ~0xFFul) | depth;
	}

	static ulong BytesToUInt64(std::vector<byte> buf, int offset)
	{
		return
			((ulong)buf[offset + 7] << 7 * 8 |
			((ulong)buf[offset + 6] << 6 * 8) |
			((ulong)buf[offset + 5] << 5 * 8) |
			((ulong)buf[offset + 4] << 4 * 8) |
			((ulong)buf[offset + 3] << 3 * 8) |
			((ulong)buf[offset + 2] << 2 * 8) |
			((ulong)buf[offset + 1] << 1 * 8) |
			((ulong)buf[offset]));
	}
};

#endif