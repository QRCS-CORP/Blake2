#ifndef _CEX_STREAMMODES_H
#define _CEX_STREAMMODES_H

#include "CexDomain.h"

NAMESPACE_ENUMERATION

/// <summary>
/// Streaming file and memory container types
/// </summary>
enum class StreamModes : byte
{
	/// <summary>
	/// No stream mode is specified
	/// </summary>
	None = 0,
	/// <summary>
	/// A FileStream class, provides streaming file access
	/// </summary>
	FileStream = 1,
	/// <summary>
	/// A MemoryStream class, provides streaming memory storage
	/// </summary>
	MemoryStream = 2,
	/// <summary>
	/// A SecureStream class, provides streaming encrytped memory storage
	/// </summary>
	SecureStream = 4
};

NAMESPACE_ENUMERATIONEND
#endif



