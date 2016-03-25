#pragma once

#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>

//Блокирующая очередь ограниченного размера
template <typename T>
class thread_safe_queue
{
public:
	thread_safe_queue() : queue_capacity(0) {}
	thread_safe_queue(std::size_t capacity) : queue_capacity(capacity) {}
	thread_safe_queue(thread_safe_queue<T> &) = delete; // запрещённый конструктор копирования
	void enqueue(const T & item); // добавляет item в очередь,но если она уже заполнена(она ограничена), поток ждет
	void pop(T & item); //удаляет элемент из очереди , сохраняя его в item. Если очередь пуста, то поток ждет

private:
	std::queue<T> queue;
	std::mutex queue_mtx; // мьютекс, который захватывается потоком, при pop и enqueue
	std::condition_variable not_empty; //условные переменные
	std::condition_variable not_full;
	std::size_t queue_capacity;
};


template <typename T>
inline void thread_safe_queue<T>:: enqueue(const T & item)
{
	std::unique_lock<std::mutex> lock(queue_mtx);
	if (queue.size() == queue_capacity)
	{
		not_full.wait(lock, [this]() { return queue.size() != queue_capacity; });
		//not_full.wait(lock)
	}
	queue.push(item);
	not_empty.notify_one();
}

template <typename T>
inline void thread_safe_queue<T>:: pop(T & item)
{
	std::unique_lock<std::mutex> lock(queue_mtx);
	if (queue.empty())
	{
		not_empty.wait(lock, [this]() { return !queue.empty(); });
	}
	item = queue.front();
	queue.pop();
	not_full.notify_one();
}

