#ifndef _BLAKE2_PARALLELUTILS_H
#define _BLAKE2_PARALLELUTILS_H

#include "Config.h"
#include <functional>

#if defined(_OPENMP)
#	include <omp.h>
#elif defined(_WIN32)
#	include <Windows.h>
#	include <ppl.h>
#else
#	include <future>
#endif

namespace Blake2
{
	class ParallelUtils
	{
	public:

		static inline void ParallelFor(size_t From, size_t To, const std::function<void(size_t)> &F)
		{
#if defined(_OPENMP)
#pragma omp parallel num_threads(To)
			{
				size_t i = omp_get_thread_num();
				F(i);
			}
#elif defined(_WIN32)
			concurrency::parallel_for(From, To, [&](size_t i)
			{
				F(i);
			});
#else
			std::vector<std::future<void>> futures;

			for (size_t i = From; i < To; ++i)
			{
				auto fut = std::async([i, F]()
				{
					F(i);
				});
				futures.push_back(std::move(fut));
			}

			for (size_t i = 0; i < futures.size(); ++i)
				futures[i].wait();

			futures.clear();
#endif
		}
	};
}
#endif