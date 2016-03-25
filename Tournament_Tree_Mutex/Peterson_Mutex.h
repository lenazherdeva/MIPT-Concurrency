#include <iostream>
#include <thread>
#include <atomic>
#include <array>

//Реализует мьютекс Петерсона(взаимоисключение для 2х потоков
class Peterson_Mutex
{
public:
	Peterson_Mutex();
	//Захват мьютекса потоком с номером 0 или 1
	void lock(int t);
	//Освобождение мьютекса потоком 
	void unlock(int t);
	//Конструктор копирования
	Peterson_Mutex(const Peterson_Mutex &) = delete;
private:
	std::array<std::atomic<bool>, 2> want;
	std::atomic<int> victim;
};


