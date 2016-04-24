#include"semaphore.h"

//увеличить значение счетчика, и если он был отрицательным, то разбудить один из
//заблокированных потоков.

void semaphore::signal() 
{
	std::unique_lock<std::mutex> lock(mtx);
	if (count.fetch_add(1) < 1) {
		cv.notify_one();
	}
}

//уменьшить значение счетчика, и если он стал отрицательным, то заблокировать поток.
void semaphore::wait()
{
	std::unique_lock<std::mutex> lock(mtx);
	count.fetch_sub(1);
	while (count.load() < 0)
	{
		cv.wait(lock);
	}
}