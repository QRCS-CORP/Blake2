#ifndef _BLAKE2TEST_ITEST_H
#define _BLAKE2TEST_ITEST_H

#include "TestCommon.h"
#include "TestEventHandler.h"

namespace TestBlake2
{
	/// <summary>
	/// Test Interface
	/// </summary>
	class ITest
	{
	public:
		// *** Properties *** //

		/// <summary>
		/// Get: The test description
		/// </summary>
		virtual const std::string Description() = 0;

		/// <summary>
		/// Progress return event callback
		/// </summary>
		virtual TestEventHandler &Progress() = 0;

		// *** Constructor *** //

		/// <summary>
		/// CTor: Initialize this class
		/// </summary>
		ITest() {}

		/// <summary>
		/// Destructor
		/// </summary>
		virtual ~ITest() {}

		// *** Public Methods *** //

		/// <summary>
		/// Start the test
		/// </summary>
		virtual std::string Run() = 0;
	};
}

#endif

