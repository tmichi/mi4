/**
 * @file ParallelFor.hpp
 * @author Takashi Michikawa <michikawa@acm.org>
 */
#ifndef MI4_PARALLEL_FOR_HPP
#define MI4_PARALLEL_FOR_HPP 1

#include <vector>
#include <algorithm>
#include <thread>
#include <iostream>

namespace mi4 {
        template < class Iterator, class Function >
        void parallel_for_each_by_grain_size (const Iterator begin, const Iterator end, const Function fn, const size_t grainSize = 1000)
        {
                std::vector< std::thread > threads;
                auto start = begin;
                auto iter = begin;

                while ( start != end ) {
                        for ( size_t i = 0; (i < grainSize && iter != end); ++i ) ++iter;
                        threads.push_back(std::thread([] (const Iterator begin, const Iterator end, Function fn) { std::for_each(begin, end, fn); }, start, iter, fn));
                        start = iter;
                }

                for ( auto& t : threads ) {
                        t.join();
                }
        }

        template < class Iterator, class Function >
        void parallel_for_each (const Iterator begin, const Iterator end, const Function fn, const size_t num_threads = std::thread::hardware_concurrency())
        {
                mi4::parallel_for_each_by_grain_size(begin, end, fn, static_cast<size_t>((std::distance(begin, end) - 1) / num_threads) + 1);
        }
};
#endif

