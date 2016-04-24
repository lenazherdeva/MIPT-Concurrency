#include "thread_pool.h"
#include <ctime>
#include <iostream>


//функция проверки числа на простоту
bool isPrime(unsigned n)
{
	for (unsigned i = 2; i <= sqrt(n); i++)
	{
		if (n % i == 0)
		{
			return false;
		}
	}
	return true;
}

int main()
{
	std::srand(std::time(0));
	std::vector<int> values;
	std::vector<std::future<bool> > results;
	thread_pool<bool> pool;

	for (int i = 0; i < 10; i++) {
		values.push_back(std::rand()%100);
		results.push_back(pool.submit(std::bind(isPrime, values[i])));
		std::cout <<  values[i] << (results[i].get() ? " is prime" : " is not prime") << std::endl;
	}
	pool.shutdown();
	return 0;
}