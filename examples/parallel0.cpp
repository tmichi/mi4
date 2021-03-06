#include <mi4/ParallelFor.hpp>
#include <iostream>
#include <mutex>
static std::mutex mutex;
class fn
{
private:
        const std::vector<int>& p;
        int& sum;

public:
        fn ( const std::vector<int>& p0, int& sum0 ) : p ( p0 ), sum ( sum0 )
        {
                return;
        }
        void operator () ( const int& i )
        {
                std::lock_guard<std::mutex>lock ( mutex ); //critical section.
                sum += i; //p[i];
        }
};

int main ( int argc, char** argv )
{
        std::vector<int> p;

        for ( int i = 0; i < 10000; i++ ) {
                p.push_back ( i * 2 );
        }

        int sum = 0;
        mi4::parallel_for_each ( p.begin(), p.end(), fn ( p, sum ), std::thread::hardware_concurrency() );
        std::cerr << sum << std::endl;
        sum = 0;

        for ( auto i : p ) {
                sum += i;
        }

        std::cerr << sum << std::endl;
        return 0;
}
