#ifndef MI4_PRIORITY_QUEUE_HPP
#define MI4_PRIORITY_QUEUE_HPP 1

#include <tuple>
#include <deque>
#include <queue>

namespace mi4
{
        /**
         * @class PriorityQueue PriorityQueue.hpp "mi/PriorityQueue.hpp"
         * @brief Finding minimum value with index from a queue structure.
         */
        template <typename T>
        class PriorityQueue
        {
        private:
                typedef std::tuple< float, T > queue_t;

                class greater
                {
                public:
                        bool operator () (const queue_t& left, const queue_t& right) const
                        {
                                return std::get< 0 >(left) > std::get< 0 >(right);
                        }
                };

        private:
                typename std::priority_queue< queue_t, typename std::deque< queue_t >, PriorityQueue::greater > _pq;
        public:
                /**
                 * @brief push key and cost to the queue.
                 * @param [in] idx Index.
                 * @param [in] cost Cost.
                 */
                void push ( const T& idx, const float cost )
                {
                        this->_pq.push ( std::make_pair ( cost, idx ) );
                        return;
                }
                /**
                 * @brief Get top index.
                 * @return Index.
                 */
                T getTopIndex ( void ) const
                {
                        return std::get< 1 >(this->_pq.top());
                }
                /**
                 * @brief Get top (minimum) cost.
                 * @return Cost.
                 */
                float getTopCost ( void ) const
                {
                        return std::get< 0 >(this->_pq.top());
                }

                bool empty (void) const
                {
                        return this->_pq.empty();
                }
                void pop ( void )
                {
                        this->_pq.pop();
                }

                size_t size ( void ) const
                {
                        return this->_pq.size();
                }
        };
};
#endif //PRIORITY_QUEUE_HPP
