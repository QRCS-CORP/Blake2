#ifndef _BLAKE2_MACPARAMS_H
#define _BLAKE2_MACPARAMS_H

#include "Common.h"

namespace Blake2 
{
	/// <summary>
	/// MacParams: A MAC Key and Vector Container class.
	/// </summary>
	class MacParams
	{
	private:
		bool _isDestroyed;
		std::vector<uint8_t> _info;
		std::vector<uint8_t> _key;
		std::vector<uint8_t> _salt;

	public:

		/// <summary>
		/// Get/Set: The MAC Key
		/// </summary>
		std::vector<uint8_t> &Key() { return _key; }

		/// <summary>
		/// Get/Set: MAC Salt value
		/// </summary>
		std::vector<uint8_t> &Salt() { return _salt; }

		/// <summary>
		/// Get/Set: MAC Personalization info
		/// </summary>
		std::vector<uint8_t> &Info() { return _info; }

		/// <summary>
		/// Initialize this class
		/// </summary>
		MacParams()
			:
			_key(0),
			_salt(0),
			_info(0),
			_isDestroyed(false)
		{
		}

		/// <summary>
		/// Initialize this class with a MAC Key
		/// </summary>
		///
		/// <param name="Key">MAC Key</param>
		explicit MacParams(const std::vector<uint8_t> &Key)
			:
			_key(Key),
			_salt(0),
			_info(0),
			_isDestroyed(false)
		{
		}

		/// <summary>
		/// Initialize this class with a MAC Key, and Salt
		/// </summary>
		///
		/// <param name="Key">MAC Key</param>
		/// <param name="Salt">MAC Salt</param>
		explicit MacParams(const std::vector<uint8_t> &Key, const std::vector<uint8_t> &Salt)
			:
			_key(Key),
			_salt(Salt),
			_isDestroyed(false)
		{
		}

		/// <summary>
		/// Initialize this class with a Cipher Key, Salt, and Info
		/// </summary>
		///
		/// <param name="Key">MAC Key</param>
		/// <param name="Salt">MAC Salt</param>
		/// <param name="Info">MAC Info</param>
		explicit MacParams(const std::vector<uint8_t> &Key, const std::vector<uint8_t> &Salt, const std::vector<uint8_t> &Info)
			:
			_key(Key),
			_salt(Salt),
			_info(Info),
			_isDestroyed(false)
		{
		}

		/// <summary>
		/// Finalize objects
		/// </summary>
		~MacParams()
		{
			Destroy();
		}

		/// <summary>
		/// Create a shallow copy of this MacParams class
		/// </summary>
		MacParams Clone()
		{
			return	MacParams(_key, _salt, _info);
		}

		/// <summary>
		/// Create a deep copy of this MacParams class
		/// </summary>
		MacParams* DeepCopy()
		{
			std::vector<uint8_t> key(_key.size());
			std::vector<uint8_t> salt(_salt.size());
			std::vector<uint8_t> info(_info.size());

			if (_key.capacity() > 0)
				memcpy(&key[0], &_key[0], key.size());
			if (_salt.capacity() > 0)
				memcpy(&salt[0], &_salt[0], salt.size());
			if (_info.capacity() > 0)
				memcpy(&info[0], &_info[0], info.size());

			return new MacParams(key, salt, info);
		}

		/// <summary>
		/// Release all resources associated with the object
		/// </summary>
		void Destroy()
		{
			if (!_isDestroyed)
			{
				if (_key.capacity() > 0)
					ClearVector(_key);
				if (_salt.capacity() > 0)
					ClearVector(_salt);
				if (_info.capacity() > 0)
					ClearVector(_info);

				_isDestroyed = true;
			}
		}

		/// <summary>
		/// Compare this MacParams instance with another
		/// </summary>
		/// 
		/// <param name="Obj">MacParams to compare</param>
		/// 
		/// <returns>Returns true if equal</returns>
		bool Equals(MacParams &Obj)
		{
			if (Obj.Key() != _key)
				return false;
			if (Obj.Salt() != _salt)
				return false;
			if (Obj.Info() != _info)
				return false;

			return true;
		}

		private:
			template <typename T>
			static void ClearVector(std::vector<T> &Obj)
			{
				if (Obj.capacity() == 0)
					return;

				memset(Obj.data(), 0, Obj.capacity() * sizeof(T));
				Obj.clear();
			}

	};
}
#endif
