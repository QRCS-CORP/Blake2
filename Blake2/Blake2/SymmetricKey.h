#ifndef _CEX_SYMMETRICKEY_H
#define _CEX_SYMMETRICKEY_H

#include "ISymmetricKey.h"

NAMESPACE_KEYSYMMETRIC

/// <summary>
/// A symmetric key container class.
/// <para>Contains keying material used for initialization of symmetric ciphers, Macs, Rngs, and Drbgs.</para>
/// </summary>
class SymmetricKey : public ISymmetricKey
{
private:

	std::vector<byte> m_info;
	bool m_isDestroyed;
	std::vector<byte> m_key;
	SymmetricKeySize m_keySizes;
	std::vector<byte> m_nonce;

public:

	//~~~Public Properties~~~//

	/// <summary>
	/// Get: Return a copy of the personalization string; can used as an optional source of entropy
	/// </summary>
	virtual const std::vector<byte> Info() { return m_info; }

	/// <summary>
	/// Get: Return a copy of the primary key
	/// </summary>
	virtual const std::vector<byte> Key() { return m_key; }

	/// <summary>
	/// Get: The SymmetricKeySize containing the byte sizes of the key, nonce, and info state members
	/// </summary>
	virtual const SymmetricKeySize KeySizes() { return m_keySizes; }

	/// <summary>
	/// Get: Return a copy of the nonce
	/// </summary>
	virtual const std::vector<byte> Nonce() { return m_nonce; }

	//~~~Constructors~~~//

	/// <summary>
	/// Instantiate an empty container
	/// </summary>
	SymmetricKey();

	/// <summary>
	/// Instantiate this class with an encryption key
	/// </summary>
	///
	/// <param name="Key">The primary encryption key</param>
	explicit SymmetricKey(const std::vector<byte> &Key);

	/// <summary>
	/// Instantiate this class with an encryption key, and nonce parameters
	/// </summary>
	///
	/// <param name="Key">The primary encryption key</param>
	/// <param name="Nonce">The nonce or counter array</param>
	explicit SymmetricKey(const std::vector<byte> &Key, const std::vector<byte> &Nonce);

	/// <summary>
	/// Instantiate this class with an encryption key, nonce, and info parameters
	/// </summary>
	///
	/// <param name="Key">The primary encryption key</param>
	/// <param name="Nonce">The nonce or counter array</param>
	/// <param name="Info">The personalization string or additional keying material</param>
	explicit SymmetricKey(const std::vector<byte> &Key, const std::vector<byte> &Nonce, const std::vector<byte> &Info);

	/// <summary>
	/// Finalize objects
	/// </summary>
	virtual ~SymmetricKey();

	//~~~Public Functions~~~//

	/// <summary>
	/// Create a shallow copy of this SymmetricKey class
	/// </summary>
	SymmetricKey* Clone();

	/// <summary>
	/// Deserialize a SymmetricKey class.
	/// <para>The caller is resposible for destroying the return key.</para>
	/// </summary>
	/// 
	/// <param name="KeyStream">Stream containing the SymmetricKey data</param>
	/// 
	/// <returns>A populated SymmetricKey class</returns>
	static SymmetricKey* DeSerialize(const MemoryStream &KeyStream);

	/// <summary>
	/// Release all resources associated with the object
	/// </summary>
	virtual void Destroy();

	/// <summary>
	/// Compare this SymmetricKey instance with another
	/// </summary>
	/// 
	/// <param name="Obj">SymmetricKey to compare</param>
	/// 
	/// <returns>Returns true if equal</returns>
	virtual bool Equals(ISymmetricKey &Obj);

	/// <summary>
	/// Serialize a SymmetricKey class.
	/// <para>The caller is resposible for destroying the return stream.</para>
	/// </summary>
	/// 
	/// <param name="KeyObj">A SymmetricKey class</param>
	/// 
	/// <returns>A stream containing the SymmetricKey data</returns>
	static MemoryStream* Serialize(SymmetricKey &KeyObj);
};

NAMESPACE_KEYSYMMETRICEND
#endif
