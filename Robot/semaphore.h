#include<atomic>
#include<mutex>
#include<condition_variable>

class semaphore
{
public:
	semaphore() : count(0) {}
	semaphore(std::size_t _count) : count(_count) {}
	void signal();
	void wait();

private:
	std::atomic<int> count;
	std::mutex mtx;
	std::condition_variable cv;
};