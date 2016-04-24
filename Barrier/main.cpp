#include "barrier.h"
#include <iostream>
#include <vector>

int main() 
{
	int number_of_threads = 10;
	barrier barr(number_of_threads);
	std::vector<std::thread> threads;
	for (int i = 0; i < number_of_threads; ++i) {
		threads.emplace_back(std::thread([&barr, i]() {
			std::cout << i << " entering\n"; 
			barr.enter();
			std::cout << i << " pass\n";
			barr.enter();
			std::cout << i << " cyclic\n";
		}));
	}
	for (int i = 0; i < number_of_threads; ++i) {
		if (threads[i].joinable())
		{
			threads[i].join();
		}
	}
	std::cout << "success!";
	return 0;
}