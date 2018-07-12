#include <mi4/PriorityQueue.hpp>
#include <iostream>
#include <Eigen/Dense>
int main ( int argc,  char** argv )
{
        mi4::PriorityQueue< Eigen::Vector2d > pq;
        pq.push(Eigen::Vector2d(10, 10), 3);
        pq.push(Eigen::Vector2d(30, 10), 2);
        pq.push(Eigen::Vector2d(10, 10), 0);
        pq.push(Eigen::Vector2d(40, 10), 1);
        pq.push(Eigen::Vector2d(10, 30), 4);

        while ( !pq.empty() ) {
                std::cerr << "cost" << pq.getTopCost() << std::endl;
                pq.pop();
        }

        return 0;
}

