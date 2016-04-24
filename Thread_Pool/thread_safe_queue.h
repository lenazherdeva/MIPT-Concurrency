#ifndef THREAD_SAFE_QUEUE
#define THREAD_SAFE_QUEUE


#include <mutex>
#include <deque>
#include <queue>
#include <condition_variable>
#include<exception>
#include<utility>

//Блокирующая очередь ограниченного размера
template <typename T, class Container = std::deque<T>>
class thread_safe_queue
{
public:

	explicit thread_safe_queue(std::size_t capacity) : queue_capacity(capacity), isShutdown(false) {}
	thread_safe_queue(thread_safe_queue<T> &) = delete; // запрещённый конструктор копирования
	void enqueue( T && item); // добавляет item в очередь,но если она уже заполнена(она ограничена), поток ждет
	void pop(T & item); //удаляет элемент из очереди , сохраняя его в item. Если очередь пуста, то поток ждет
	void shutdown();
private:
	Container queue;
	//std::queue<T> queue;
	std::mutex queue_mtx; // мьютекс, который захватывается потоком, при pop и enqueue
	std::condition_variable not_empty; //условные переменные
	std::condition_variable not_full;
	std::size_t queue_capacity;
	bool isShutdown;
};

template <typename T, class Container>
inline void thread_safe_queue<T, Container>::enqueue(T && item)
{
	std::unique_lock<std::mutex> lock(queue_mtx);
	if (queue.size() == queue_capacity && !isShutdown)
	{
		not_full.wait(lock, [this]() { return queue.size() != queue_capacity; });

	}
	if (isShutdown)
	{
		throw std::exception();
	}
	queue.push_back(std::move(item));
	not_empty.notify_one();
}

template <typename T, class Container>
inline void thread_safe_queue<T, Container>:: pop(T & item)
{
	std::unique_lock<std::mutex> lock(queue_mtx);
	if (queue.empty() && !isShutdown)
	{
		not_empty.wait(lock, [this]() { return !queue.empty(); });
	}
	if (isShutdown)
	{
		throw std::exception();
	}
	item = std::move(queue.front());
	queue.pop_front();
	not_full.notify_one();
}

template <typename T, class Container>
inline void thread_safe_queue<T, Container>::shutdown() 
{
	std::unique_lock<std::mutex> lock(queue_mtx);
	isShutdown = true;
	not_empty.notify_all();		
}

#endif