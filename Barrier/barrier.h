# pragma once
#include <thread>
#include <condition_variable>
#include <atomic>

//примитив синхронизации для n потоков, который представляет собой виртуальную границу в коде, 
//пересечь которую можно только в случае, если к ней подошли все n потоков.
class barrier
{
public:
	barrier(std::size_t _num_threads);
	void enter(); //метод,который поток вызывает,когда хочет пройти через барьер

private:
	std::size_t num_threads;
	std::condition_variable ring; //условная переменная,на которой блокируются потоки,пока не пришло достаточное количество
	std::size_t era = 0; // счетчик, который инкрементируется в тот момент, когда все потоки подошли к барьеру
	std::mutex barr_mutex;
	std::atomic<std::size_t> count; //количество потоков,которые подошли к барьеру

};