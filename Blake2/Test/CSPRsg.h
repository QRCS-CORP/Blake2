#ifndef _BLAKE2TEST_RANDOM_H
#define _BLAKE2TEST_RANDOM_H

#include <vector>

#if defined(_WIN32)
#	include <Windows.h>
#	pragma comment(lib, "advapi32.lib")
#else
#	include <sys/types.h>
#	include <thread>
#endif

namespace BlakeTest
{
	/// <summary>
	/// A random generator
	/// </summary>
	class CSPRsg
	{
	private:
#if defined(_WIN32)
		HCRYPTPROV m_hProvider = 0;
#endif

	public:

		/// <summary>
		/// Initialize this class
		/// </summary>
		CSPRsg()
		{
			Reset();
		}

		/// <summary>
		/// Destructor
		/// </summary>
		~CSPRsg()
		{
			Destroy();
		}

		/// <summary>
		/// Release all resources associated with the object
		/// </summary>
		void Destroy()
		{
#if defined(_WIN32)
			if (!::CryptReleaseContext(m_hProvider, 0))
				return;
#endif
		}

		/// <summary>
		/// Fill the buffer with random bytes
		/// </summary>
		///
		/// <param name="Output">The array to fill</param>
		void GetBytes(std::vector<uint8_t> &Output)
		{
#if defined(_WIN32)
			DWORD dwLength = (DWORD)Output.size();
			BYTE* ptr = (BYTE*)&Output[0];

			try
			{
				if (!::CryptGenRandom(m_hProvider, dwLength, ptr))
				{
					::CryptReleaseContext(m_hProvider, 0);
					throw;
				}
			}
			catch (...)
			{
				throw;
			}
#else
			size_t size = Output.size();

			try
			{
				if (size <= 4)
				{
					if (size > 0)
					{
						uint32_t r = arc4random();
						memcpy(&Output[0], &r, size);
					}
				}
				else
				{
					size_t div = size / 4;
					size_t mod = size % 4;

					int i = 0;
					for (int d = 0; d < div; ++d)
					{
						uint32_t r = arc4random();
						uint8_t *rp = (uint8_t*)&r;
						Output[i++] = rp[0];
						Output[i++] = rp[1];
						Output[i++] = rp[2];
						Output[i++] = rp[3];
					}

					if (mod)
					{
						uint32_t r = arc4random();
						memcpy(&Output[i], &r, mod);
					}
				}
			}
			catch (...)
			{
				throw;
			}
#endif
		}

		/// <summary>
		/// Get a pseudo random seed uint8_t array
		/// </summary>
		/// 
		/// <param name="Size">The size of the expected seed returned</param>
		/// 
		/// <returns>A pseudo random seed</returns>
		std::vector<uint8_t> CSPRsg::GetBytes(size_t Size)
		{
			std::vector<uint8_t> data(Size);
			GetBytes(data);
			return data;
		}

		/// <summary>
		/// Returns the next pseudo random 32bit integer
		/// </summary>
		int Next()
		{
			int ret(0);
			int len = sizeof(ret);
			std::vector<uint8_t> rnd(len);

			GetBytes(rnd);
			memcpy(&ret, &rnd[0], len);

			return ret;
		}

		/// <summary>
		/// Reset the internal state
		/// </summary>
		void Reset()
		{
#if defined(_WIN32)
			if (!::CryptAcquireContextW(&m_hProvider, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
				throw;
#endif
		}
	};
}
#endif

