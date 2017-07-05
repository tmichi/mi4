/**
 * @file ParallelFor.hpp
 * @author Takashi Michikawa <michikawa@acm.org>
 */
#ifndef MI4_PARALLEL_FOR_HPP
#define MI4_PARALLEL_FOR_HPP 1
#include <iterator>
#include <vector>
#include <algorithm>
#include <thread>
#include <iostream>

namespace mi4
{



        template <class Iterator, class Function>
        void parallel_for_each_by_grain_size ( const Iterator begin, const Iterator end, const Function fn, const int grainSize = 1000 )
        {
                class parallel_for
                {
                public:
                        parallel_for ( const Iterator begin, const Iterator end, const Function fn, const int grainSize )
                        {
                                std::vector<std::thread> threads;
                                Iterator start = begin;
                                Iterator iter  = begin;

                                while ( start != end ) {
                                        for ( int i = 0 ; ( i < grainSize && iter != end ); ++i ) {
                                                ++iter;
                                        }

                                        threads.push_back ( std::thread ( &parallel_for::child_thread, this, start, iter,  fn ) );
                                        start = iter;
                                }

                                for ( auto& t : threads ) {
                                        t.join();
                                }

                                return;
                        }
                private:
                        void child_thread ( const Iterator begin, const Iterator end, Function fn )
                        {
                                std::for_each ( begin, end, fn );
                                return;
                        }
                };

                parallel_for ( begin, end, fn, grainSize );
                return;
        }

        template <class Iterator, class Function>
        void parallel_for_each ( const Iterator begin, const Iterator end, const Function fn, const int nthread = 4 )
        {
                int grainSize = 0;
                auto iter = begin;

                while ( iter != end ) {
                        ++grainSize;
                        ++iter;
                }

                grainSize = ( grainSize - 1 ) / nthread + 1;
                parallel_for_each_by_grain_size ( begin, end, fn, grainSize ) ;
                return;
        }
};
#endif
