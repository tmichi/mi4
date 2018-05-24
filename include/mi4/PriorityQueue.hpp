#ifndef MI_PRIORITY_QUEUE_HPP
#define MI_PRIORITY_QUEUE_HPP 1
#include <cstddef>
#include <utility>
#include <deque>
#include <queue>

namespace mi4
{
        /**
         * @class PriorityQueue PriorityQueue.hpp "mi/PriorityQueue.hpp"
         * @brief Finding minimum value with index from a queue structure.
         *
         * This class is a wrapper of std::priority_queue in STL, and most of fucntion gives same name as those in std::pririty_queue.
         * But I added a few extention including :
         * 1) Defining cost with index structure ( queue_type ) in local classes for managing indexed cost.
         * 2) Changing order to ascend order. Original classes supports ascend order but implementation looks long.
         * I believe it will be a help for implementing Dijkstra's shortest path problem or distance transform algorithm.
         */
        template <typename T>
        class PriorityQueue
        {
        private:
                typedef std::pair<float, T> queue_type;
                class CompareDistance
                {
                public:
                        bool operator() ( const queue_type& left, const queue_type& right ) const
                        {
                                return left.first > right.first;
                        }
                };

                size_t _num;
                typename std::priority_queue<queue_type, typename std::deque<queue_type>, CompareDistance> _pq;
                // disable copy constructor and = operator.
                PriorityQueue ( const PriorityQueue& that ) = delete;
                void operator = ( const PriorityQueue& that ) = delete;
        public:
                /**
                 * @brief Constructor.
                 */
                PriorityQueue ( void ) : _num ( 0 )
                {
                        return;
                }
                /**
                 * @brief Destructor.
                 */
                ~PriorityQueue ( void )  = default;
                /**
                 * @brief push key and cost to the queue.
                 * @param [in] idx Index.
                 * @param [in] cost Cost.
                 */
                void push ( const T& idx, const float cost )
                {
                        this->_pq.push ( std::make_pair ( cost, idx ) );
                        this->_num += 1;
                        return;
                }
                /**
                 * @brief Get top index.
                 * @return Index.
                 */
                T getTopIndex ( void ) const
                {
                        return this->_pq.top().second;
                }
                /**
                 * @brief Get top (minimum) cost.
                 * @return Cost.
                 */
                float getTopCost ( void ) const
                {
                        return this->_pq.top().first;
                }
                /**
                 * @brief Check the queue is empty.
                 * @retval true The queue is empty.
                 * @retval false The queue is not empty.
                 */
                bool empty ( void )
                {
                        return this->_pq.empty();
                }
                /**
                 * @brief Pop the queue.
                 */
                void pop ( void )
                {
                        this->_pq.pop();
                        this->_num -= 1;
                }
                /**
                 * @brief Get size of queue.
                 * @return Size of queue.
                 */
                size_t size ( void ) const
                {
                        return this->_num;
                }

                /**
                 * @brief Get all indices.
                 * @param [out] idx Indices.
                 */
                void getAllIndices ( std::deque<T>& idx )
                {
                        idx.clear();
                        std::deque<float> cost;

                        while ( !this->empty() ) {
                                idx.push_back (  this->getTopIndex() );
                                cost.push_back ( this->getTopCost() );
                                this->pop();
                        }

                        for ( size_t i = 0 ; i < idx.size() ; ++i ) {
                                this->push ( idx.at ( i ), cost.at ( i ) );
                        }

                        cost.clear();
                        return;
                }
        };
};
#endif //PRIORITY_QUEUE_HPP
