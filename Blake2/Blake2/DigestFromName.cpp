#include "DigestFromName.h"
#include "Blake256.h"
#include "Blake512.h"

NAMESPACE_HELPER

IDigest* DigestFromName::GetInstance(Digests DigestType, bool Parallel)
{
	try
	{
		switch (DigestType)
		{
		case Digests::Blake256:
			return new Digest::Blake256(Parallel);
		case Digests::Blake512:
			return new Digest::Blake512(Parallel);
		default:
			throw;
		}
	}
	catch (const std::exception)
	{
		throw;
	}
}

size_t DigestFromName::GetBlockSize(Digests DigestType)
{
	try
	{
		switch (DigestType)
		{
		case Digests::Blake256:
			return 64;
		case Digests::Blake512:
			return 128;
		case Digests::None:
			return 0;
		default:
			throw;
		}
	}
	catch (const std::exception)
	{
		throw;
	}
}

size_t DigestFromName::GetDigestSize(Digests DigestType)
{
	try
	{
		switch (DigestType)
		{
		case Digests::Blake256:
			return 32;
		case Digests::Blake512:
			return 64;
		case Digests::None:
			return 0;
		default:
			throw;
		}
	}
	catch (const std::exception)
	{
		throw;
	}
}

size_t DigestFromName::GetPaddingSize(Digests DigestType)
{
	try
	{
		switch (DigestType)
		{
		case Digests::Blake256:
		case Digests::Blake512:
			return 0;
		default:
			throw;
		}
	}
	catch (const std::exception)
	{
		throw;
	}
}

NAMESPACE_HELPEREND