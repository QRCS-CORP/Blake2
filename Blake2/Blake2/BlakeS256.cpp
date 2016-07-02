#include "BlakeS256.h"
#include "BlakeSCompress.h"
#include "IntUtils.h"
#include "ParallelUtils.h"

namespace Blake2
{
	// *** Public Methods *** //

	void BlakeS256::BlockUpdate(const std::vector<uint8_t> &Input, size_t InOffset, size_t Length)
	{
		if (Length == 0)
			return;

		if (m_isParallel)
		{
			size_t ttlLen = Length + m_msgLength;
			const size_t minPrl = m_msgBuffer.size() + (m_minParallel - BLOCK_SIZE);

			// input larger than min parallel; process buffer and loop-in remainder
			if (ttlLen > minPrl)
			{
				// fill buffer
				size_t rmd = m_msgBuffer.size() - m_msgLength;
				if (rmd != 0)
					memcpy(&m_msgBuffer[m_msgLength], &Input[InOffset], rmd);

				m_msgLength = 0;
				Length -= rmd;
				InOffset += rmd;
				ttlLen -= m_msgBuffer.size();

				// empty the message buffer
				ParallelUtils::ParallelFor(0, m_treeParams.ThreadDepth(), [this, &Input, InOffset](size_t i)
				{
					ProcessLeaf(m_msgBuffer, i * BLOCK_SIZE, m_State[i], BLOCK_SIZE * 2);
				});

				// loop in the remainder (no buffering)
				if (Length > minPrl)
				{
					// calculate working set size
					size_t prcLen = Length - m_minParallel;
					if (prcLen % m_minParallel != 0)
						prcLen -= (prcLen % m_minParallel);

					const size_t BLKLEN = prcLen / m_treeParams.ThreadDepth();

					// process large blocks
					ParallelUtils::ParallelFor(0, m_treeParams.ThreadDepth(), [this, &Input, InOffset, BLKLEN](size_t i)
					{
						ProcessLeaf(Input, InOffset + (i * BLKLEN), m_State[i], BLKLEN);
					});

					Length -= prcLen;
					InOffset += prcLen;
					ttlLen -= prcLen;
				}
			}

			// remainder exceeds buffer size; process first 4 blocks and shift buffer left
			if (ttlLen > m_msgBuffer.size())
			{
				// fill buffer
				size_t rmd = m_msgBuffer.size() - m_msgLength;
				if (rmd != 0)
					memcpy(&m_msgBuffer[m_msgLength], &Input[InOffset], rmd);

				Length -= rmd;
				InOffset += rmd;
				m_msgLength = m_msgBuffer.size();

				// process first half of buffer
				ParallelUtils::ParallelFor(0, m_treeParams.ThreadDepth(), [this, &Input, InOffset](size_t i)
				{
					ProcessBlock(m_msgBuffer, i * BLOCK_SIZE, m_State[i], BLOCK_SIZE);
				});

				// left rotate the buffer
				m_msgLength -= m_minParallel;
				rmd = m_msgBuffer.size() / 2;
				memcpy(&m_msgBuffer[0], &m_msgBuffer[rmd], rmd);
			}
		}
		else
		{
			if (m_msgLength + Length > BLOCK_SIZE)
			{
				size_t rmd = BLOCK_SIZE - m_msgLength;
				if (rmd != 0)
					memcpy(&m_msgBuffer[m_msgLength], &Input[InOffset], rmd);

				ProcessBlock(m_msgBuffer, 0, m_State[0], BLOCK_SIZE);
				m_msgLength = 0;
				InOffset += rmd;
				Length -= rmd;
			}

			if (Length > BLOCK_SIZE)
			{
				size_t blkLen = Length - Length % BLOCK_SIZE;
				if (Length % BLOCK_SIZE == 0)
					blkLen -= BLOCK_SIZE;
				ProcessLeaf(Input, InOffset, m_State[0], blkLen);
				InOffset += blkLen;
				Length -= blkLen;
			}
		}

		// store unaligned bytes
		if (Length != 0)
		{
			memcpy(&m_msgBuffer[m_msgLength], &Input[InOffset], Length);
			m_msgLength += Length;
		}
	}

	void BlakeS256::ComputeHash(const std::vector<uint8_t> &Input, std::vector<uint8_t> &Output)
	{
		BlockUpdate(Input, 0, Input.size());
		DoFinal(Output, 0);
		Reset();
	}

	void BlakeS256::Destroy()
	{
		if (!m_isDestroyed)
		{
			m_isDestroyed = true;
			IntUtils::ClearVector(m_msgBuffer);
			IntUtils::ClearVector(m_treeConfig);

			for (size_t i = 0; i < m_State.size(); ++i)
				m_State[i].Reset();

			if (m_treeDestroy)
				m_treeParams.Reset();

			m_isParallel = false;
			m_leafSize = 0;
			m_minParallel = 0;
			m_msgLength = 0;
			m_parallelBlockSize = 0;
		}
	}

	size_t BlakeS256::DoFinal(std::vector<uint8_t> &Output, const size_t OutOffset)
	{
		if (m_isParallel)
		{
			std::vector<uint8_t> hashCodes(m_treeParams.ThreadDepth() * DIGEST_SIZE);

			// padding
			if (m_msgLength < m_msgBuffer.size())
				memset(&m_msgBuffer[m_msgLength], 0, m_msgBuffer.size() - m_msgLength);

			size_t prtBlk = 0;
			// process unaligned blocks
			if (m_msgLength > m_minParallel)
			{
				size_t blkCount = (m_msgLength - m_minParallel) / BLOCK_SIZE;
				if (m_msgLength % BLOCK_SIZE != 0)
					++blkCount;

				for (size_t i = 0; i < blkCount; ++i)
				{
					// process partial block set
					ProcessBlock(m_msgBuffer, (i * BLOCK_SIZE), m_State[i], BLOCK_SIZE);
					// swap blocks
					memcpy(&m_msgBuffer[i * BLOCK_SIZE], &m_msgBuffer[m_minParallel + (i * BLOCK_SIZE)], BLOCK_SIZE);
					m_msgLength -= BLOCK_SIZE;
				}
				// store partial block index
				prtBlk = blkCount - 1;
			}

			// process last 4 blocks
			for (size_t i = 0; i < m_treeParams.ThreadDepth(); ++i)
			{
				// apply f0 bit reversal constant to final blocks
				m_State[i].F[0] = UL_MAX;

				// f1 constant on last block
				if (i == m_treeParams.ThreadDepth() - 1)
					m_State[i].F[1] = UL_MAX;

				if (i != prtBlk)
				{
					ProcessBlock(m_msgBuffer, i * BLOCK_SIZE, m_State[i], BLOCK_SIZE);
				}
				else
				{
					size_t fnlSze = m_msgLength % BLOCK_SIZE == 0 ? BLOCK_SIZE : m_msgLength % BLOCK_SIZE;
					ProcessBlock(m_msgBuffer, i * BLOCK_SIZE, m_State[i], fnlSze);
				}

				IntUtils::Le256ToBlock(m_State[i].H, hashCodes, i * DIGEST_SIZE);
			}

			// set up the root node
			m_msgLength = 0;
			m_treeParams.NodeDepth() = 1;
			m_treeParams.NodeOffset() = 0;
			m_treeParams.MaxDepth() = 2;
			Initialize(m_treeParams, m_State[0]);

			// load blocks
			for (size_t i = 0; i < m_treeParams.ThreadDepth(); ++i)
				BlockUpdate(hashCodes, i * DIGEST_SIZE, DIGEST_SIZE);

			// compress all but last block
			ProcessLeaf(m_msgBuffer, 0, m_State[0], m_msgLength - BLOCK_SIZE);
			// apply f0 and f1 flags
			m_State[0].F[0] = UL_MAX;
			m_State[0].F[1] = UL_MAX;
			// last compression
			ProcessBlock(m_msgBuffer, m_msgLength - BLOCK_SIZE, m_State[0], BLOCK_SIZE);
			// output the code
			IntUtils::Le256ToBlock(m_State[0].H, Output, 0);
		}
		else
		{
			size_t padLen = m_msgBuffer.size() - m_msgLength;
			if (padLen > 0)
				memset(&m_msgBuffer[m_msgLength], 0, padLen);

			m_State[0].F[0] = UL_MAX;
			ProcessBlock(m_msgBuffer, 0, m_State[0], m_msgLength);
			IntUtils::Le256ToBlock(m_State[0].H, Output, 0);
		}

		Reset();

		return DIGEST_SIZE;
	}

	size_t BlakeS256::Generate(MacParams &MacKey, std::vector<uint8_t> &Output)
	{
#if defined(_DEBUG)
		assert(Output.size() != 0);
		assert(MacKey.Key().size() >= DIGEST_SIZE);
		assert((MacKey.Key().size() + MacKey.Salt().size() + MacKey.Info().size()) <= BLOCK_SIZE);
#endif
#if defined(CPP_EXCEPTIONS)
		if ((MacKey.Key().size() + MacKey.Salt().size() + MacKey.Info().size()) > BLOCK_SIZE)
			throw CryptoDigestException("BlakeS256:Generate", "Buffer size must be at least 1 byte!");
		if (MacKey.Key().size() < DIGEST_SIZE)
			throw CryptoDigestException("BlakeS256:Generate", "The key must be at least 32 bytes long!");
		if ((MacKey.Key().size() + MacKey.Salt().size() + MacKey.Info().size()) > BLOCK_SIZE)
			throw CryptoDigestException("BlakeS256:Generate", "The maximum combined key (key + salt + info) input size is 64 bytes!");
#endif

		size_t bufSize = DIGEST_SIZE;
		std::vector<uint8_t> inpCtr(BLOCK_SIZE);

		// add the key to state
		LoadMacKey(MacKey);
		// process the key
		ProcessBlock(m_msgBuffer, 0, m_State[0], BLOCK_SIZE);
		// copy hash to upper half of input
		memcpy(&inpCtr[DIGEST_SIZE], &m_State[0].H[0], DIGEST_SIZE);
		// add padding to empty bytes; hamming const 'ipad'
		memset(&inpCtr[sizeof(uint32_t)], 0x36, DIGEST_SIZE - sizeof(uint32_t));
		// increment the input counter
		Increment(inpCtr);
		// process the block
		ProcessBlock(inpCtr, 0, m_State[0], BLOCK_SIZE);

		if (bufSize < Output.size())
		{
			memcpy(&Output[0], &m_State[0].H[0], bufSize);
			int32_t rmd = (int32_t)(Output.size() - bufSize);

			while (rmd > 0)
			{
				memcpy(&inpCtr[DIGEST_SIZE], &m_State[0].H[0], DIGEST_SIZE);
				Increment(inpCtr);
				ProcessBlock(inpCtr, 0, m_State[0], BLOCK_SIZE);

				if (rmd > (int32_t)DIGEST_SIZE)
				{
					memcpy(&Output[bufSize], &m_State[0].H[0], DIGEST_SIZE);
					bufSize += DIGEST_SIZE;
					rmd -= (int32_t)DIGEST_SIZE;
				}
				else
				{
					rmd = (int32_t)(Output.size() - bufSize);
					memcpy(&Output[bufSize], &m_State[0].H[0], rmd);
					rmd = 0;
				}
			}
		}
		else
		{
			memcpy(&Output[0], &m_State[0].H[0], Output.size());
		}

		return Output.size();
	}

	void BlakeS256::LoadMacKey(MacParams &MacKey)
	{
#if defined(_DEBUG)
		assert(MacKey.Key().size() >= 16 || MacKey.Key().size() <= 32);
#endif
#if defined(CPP_EXCEPTIONS)
		if (MacKey.Key().size() < 16 || MacKey.Key().size() > 32)
			throw CryptoDigestException("BlakeS256", "Mac Key has invalid length!");
#endif

		if (MacKey.Salt().size() != 0)
		{
#if defined(_DEBUG)
			assert(MacKey.Salt().size() == 8);
#endif
#if defined(CPP_EXCEPTIONS)
			if (MacKey.Salt().size() != 8)
				throw CryptoDigestException("BlakeS256", "Salt has invalid length!");
#endif

			m_treeConfig[4] = IntUtils::BytesToLe32(MacKey.Salt(), 0);
			m_treeConfig[5] = IntUtils::BytesToLe32(MacKey.Salt(), 4);
		}

		if (MacKey.Info().size() != 0)
		{
#if defined(_DEBUG)
			assert(MacKey.Info().size() == 8);
#endif
#if defined(CPP_EXCEPTIONS)
			if (MacKey.Info().size() != 8)
				throw CryptoDigestException("BlakeS256", "Info has invalid length!");
#endif

			m_treeConfig[6] = IntUtils::BytesToLe32(MacKey.Info(), 0);
			m_treeConfig[7] = IntUtils::BytesToLe32(MacKey.Info(), 4);
		}

		std::vector<uint8_t> mkey(BLOCK_SIZE, 0);
		memcpy(&mkey[0], &MacKey.Key()[0], MacKey.Key().size());
		m_treeParams.KeyLength() = (uint8_t)MacKey.Key().size();

		if (m_isParallel)
		{
			// initialize the leaf nodes and add the key 
			for (size_t i = 0; i < m_treeParams.ThreadDepth(); ++i)
			{
				memcpy(&m_msgBuffer[i * BLOCK_SIZE], &mkey[0], mkey.size());
				m_treeParams.NodeOffset() = i;
				Initialize(m_treeParams, m_State[i]);
			}
			m_msgLength = m_minParallel;
			m_treeParams.NodeOffset() = 0;
		}
		else
		{
			memcpy(&m_msgBuffer[0], &mkey[0], mkey.size());
			m_msgLength = BLOCK_SIZE;
			Initialize(m_treeParams, m_State[0]);
		}
	}

	void BlakeS256::Reset()
	{
		m_msgLength = 0;
		memset(&m_msgBuffer[0], 0, m_msgBuffer.size());

		if (m_isParallel)
		{
			for (size_t i = 0; i < m_treeParams.ThreadDepth(); ++i)
			{
				m_treeParams.NodeOffset() = i;
				Initialize(m_treeParams, m_State[i]);
			}
			m_treeParams.NodeOffset() = 0;
		}
		else
		{
			Initialize(m_treeParams, m_State[0]);
		}
	}

	void BlakeS256::Update(uint8_t Input)
	{
		std::vector<uint8_t> inp(1, Input);
		BlockUpdate(inp, 0, 1);
	}

	// *** Private Methods *** //

	void BlakeS256::Increase(Blake2sState &State, uint32_t Length)
	{
		State.T[0] += Length;
		if (State.T[0] < Length)
			++State.T[1];
	}

	void BlakeS256::Increment(std::vector<uint8_t> &Counter)
	{
		// increment the message counter
		IntUtils::Le32ToBytes(IntUtils::BytesToLe32(Counter, 0) + 1, Counter, 0);
	}

	void BlakeS256::Initialize(Blake2Tree &TreeParams, Blake2sState &State)
	{
		memset(&State.T[0], 0, COUNTER_SIZE * sizeof(uint32_t));
		memset(&State.F[0], 0, FLAG_SIZE * sizeof(uint32_t));
		memcpy(&State.H[0], &m_cIV[0], CHAIN_SIZE * sizeof(uint32_t));

		m_treeConfig[0] = TreeParams.DigestLength();
		m_treeConfig[0] |= TreeParams.KeyLength() << 8;
		m_treeConfig[0] |= TreeParams.FanOut() << 16;
		m_treeConfig[0] |= TreeParams.MaxDepth() << 24;
		m_treeConfig[1] = TreeParams.LeafLength();
		m_treeConfig[2] = (uint32_t)TreeParams.NodeOffset();
		m_treeConfig[3] |= TreeParams.NodeDepth() << 16;
		m_treeConfig[3] |= TreeParams.InnerLength() << 24;

		State.H[0] ^= m_treeConfig[0];
		State.H[1] ^= m_treeConfig[1];
		State.H[2] ^= m_treeConfig[2];
		State.H[3] ^= m_treeConfig[3];
		State.H[4] ^= m_treeConfig[4];
		State.H[5] ^= m_treeConfig[5];
		State.H[6] ^= m_treeConfig[6];
		State.H[7] ^= m_treeConfig[7];
	}

	void BlakeS256::ProcessBlock(const std::vector<uint8_t> &Input, size_t InOffset, Blake2sState &State, size_t Length)
	{
		Increase(State, (uint32_t)Length);
		BlakeSCompress::Compress32(Input, InOffset, State, m_cIV);
	}

	void BlakeS256::ProcessLeaf(const std::vector<uint8_t> &Input, size_t InOffset, Blake2sState &State, size_t Length)
	{
		do
		{
			ProcessBlock(Input, InOffset, State, BLOCK_SIZE);
			InOffset += BLOCK_SIZE;
			Length -= BLOCK_SIZE;
		} 
		while (Length != 0);
	}

}