#include "barrier.h"

barrier::barrier(std::size_t _num_threads) :num_threads(_num_threads)
{
	count.store(0);
}

//при вызове enter() поток блокируется до тех пор, пока к барьеру не подойдут все остальные потоки,
//т.е. тоже не вызовут enter()
//поток, последним вызвавший enter(), разблокирует все ожидающие потоки
void barrier::enter()
{
	std::unique_lock<std::mutex> lock(barr_mutex);
	count.fetch_add(1);
	if (count.load() == num_threads)
	{
		era++;
		count.store(0);
		ring.notify_all();
	}
	else
	{
		std::size_t previous_era = era;
		while (count.load() < num_threads && previous_era == era)
		{
			ring.wait(lock);
		}
	}
}