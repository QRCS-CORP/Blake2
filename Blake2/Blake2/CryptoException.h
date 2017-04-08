#ifndef _CEX_CRYPTOEXCEPTION_H
#define _CEX_CRYPTOEXCEPTION_H

#include "CexDomain.h"

NAMESPACE_EXCEPTION

/// <summary>
/// Generalized cryptographic error container
/// </summary>
struct CryptoException : std::exception
{
private:
	std::string m_details;
	std::string m_message;
	std::string m_origin;

public:

	/// <summary>
	/// Get/Set: The inner exception string
	/// </summary>
	std::string &Details() { return m_details; }

	/// <summary>
	/// Get/Set: The message associated with the error
	/// </summary>
	std::string &Message() { return m_message; }

	/// <summary>
	/// Get/Set: The origin of the exception in the format Class
	/// </summary>
	std::string &Origin() { return m_origin; }


	/// <summary>
	/// Instantiate this class with a message
	/// </summary>
	///
	/// <param name="Message">A custom message or error data</param>
	explicit CryptoException(const std::string &Message)
		:
		m_details(""),
		m_message(Message),
		m_origin("")
	{
	}

	/// <summary>
	/// Instantiate this class with an origin and message
	/// </summary>
	///
	/// <param name="Origin">The origin of the exception</param>
	/// <param name="Message">A custom message or error data</param>
	explicit CryptoException(const std::string &Origin, const std::string &Message)
		:
		m_details(""),
		m_message(Message),
		m_origin(Origin)
	{
	}

	/// <summary>
	/// Instantiate this class with an origin, message and inner exception
	/// </summary>
	///
	/// <param name="Origin">The origin of the exception</param>
	/// <param name="Message">A custom message or error data</param>
	/// <param name="Detail">The inner exception string</param>
	explicit CryptoException(const std::string &Origin, const std::string &Message, const std::string &Detail)
		:
		m_details(Detail),
		m_message(Message),
		m_origin(Origin)
	{
	}
};

NAMESPACE_EXCEPTIONEND
#endif