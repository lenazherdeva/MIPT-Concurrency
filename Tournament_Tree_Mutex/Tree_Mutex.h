#ifndef TREE_MUTEX
#define TREE_MUTEX
#include <iostream>
#include <thread>
#include <atomic>
#include <vector>
#include <array>
#include<cmath>	
#include "Peterson_Mutex.h"

//Мьютекс для взаимного исключения для n потоков на основе турнирного дерева и алгоритма Петерсона
class Tree_Mutex
{
public:
	Tree_Mutex() = default;
	Tree_Mutex(std::size_t num_threads);
	//Захват мьютекса потоком с номером от 0 до num_treads
	void lock(std::size_t thread_index);
	///Освобождение мьютекса потоком 
	void unlock(std::size_t thread_index);

private:
	std::size_t height; // высота дерева
	std::size_t powered_num_threads; // количество потоков, приведеннык к степени 2	
	std::vector<Peterson_Mutex> vec_peterson_mutex; //мьютексы
	std::atomic<size_t> locked_ind; // индексы потоков, которые захватили мьютекс

};

#endif

