#include <mi4/PriorityQueue.hpp>
#include <iostream>
int main ( int argc,  char** argv ) {
	mi4::PriorityQueue<int> pq;
	pq.push(0, 3);
	pq.push(1, 0);
	pq.push(2, 2);
	pq.push(3, 1);
	pq.push(4, 5);

	while( !pq.empty() ) {
		std::cerr<<"cost"<<pq.getTopCost()<<" index"<<pq.getTopIndex()<<std::endl;
		pq.pop();
	}
	return 0;
}

