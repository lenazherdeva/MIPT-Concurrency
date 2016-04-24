#pragma once
#include <functional>
#include <future>
#include <thread>
#include <mutex>
#include <vector>
#include "thread_safe_queue.h"
#include <utility>

template <typename R>
class thread_pool {
public:
	thread_pool();
	thread_pool(std::size_t num_threads_);	
	std::future <R> submit(std::function<R()> func); // функция для передачи задачи на выполнение в пул потоков
	void shutdown(); // функция для корректного остановления потоков-воркеров при разрушении пула
	void worker_function(); 
private:
	std::size_t num_threads;
	thread_safe_queue<std::pair<std::promise<R>, std::function<R()> >> queue; //потокобезопасная очередь
	//thread_safe_queue<std::packaged_task<R()> > queue; //потокобезопасная очередь
	std::vector<std::thread> workers;
};

//в конструкторе по умолчанию идет подбор числа воркеров: создает столько, сколько ядер, если что-то идет не так, то 4
template <typename R>
inline thread_pool<R>::thread_pool() : queue(std::numeric_limits<std::size_t>::max())
{
	int temp = std::thread::hardware_concurrency();
	num_threads = temp != 0 ? temp : 4;
	for (std::size_t i = 0; i < num_threads; ++i)
	{
		workers.emplace_back(std::thread(std::bind(&thread_pool::worker_function, this)));
	}
}

//для передачи задач должна использоваться блокирующая очередь неограниченного размера,поэтому задаем максимальный размер
template<typename R>
inline thread_pool<R>::thread_pool(std::size_t num_threads_) : queue(std::numeric_limits<std::size_t>::max())
{
	num_threads = num_threads_;
	for (std::size_t i = 0; i < num_threads; ++i)
	{
		workers.emplace_back(std::thread(std::bind(&thread_pool::worker_function, this)));
	}
}

//функция для воркеров (забирают задачу из очереди, в promise будет записан результат выполнения функции)
template<typename R>
inline void thread_pool<R>::worker_function()
{

	std::pair<std::promise<R>, std::function<R()>> job;
	try {
		while (true) {
			queue.pop(job);
			job.first.set_value(job.second());
		}
	}
	catch (std::exception &) {
		std::cout << "Error";
	}
}

//функция для передачи задачи на выполнение в пул потоков
//поток-клиент при добавлении задачи получит std::future
template <typename R>
std::future<R> thread_pool<R>::submit(std::function<R()> func) {
	std::promise<R> mypromise;
	std::future<R> myfuture = mypromise.get_future();
	queue.enqueue(std::make_pair(std::move(mypromise), func));
	return myfuture;
}

//функция для корректного остановления потоков-воркеров при разрушении пула
//вызов завершается только тогда, когда все потоки-воркеры завершили свою работу
template <typename R>
inline void thread_pool<R>::shutdown() {
	queue.shutdown();
	for (std::size_t i = 0; i < num_threads; ++i) {
		if (workers[i].joinable())
		{
			workers[i].join();
		}
	}
}






