#include<iostream>
#include "thread_safe_queue.h"

thread_safe_queue<int> safeQueue(7);
int numberOfConsumers = 5;
std::mutex printing_mutex; //mutex для безопасной печати
int tasks = 12;

//функция, проверяющая является ли число простым
bool isPrime(int n)
{
	for (int i = 2; i <= sqrt(n); i++)
	{
		if (n % i == 0)
		{
			return false;
		}
	}
	return true;
}

// функция для поставщика
void producer_function()
{
	int curTask = 0;
	while (curTask < tasks)
	{
		safeQueue.enqueue(rand()%100);
		curTask++;
	}
	//shutdown consumers
	for (int i = 0; i < numberOfConsumers; i++)
		safeQueue.enqueue(-1);
}

//функция для потребителя(который обрабатывает данные от поставщика)
void consumer_function(int  consumerId)
{
	int number;
	while (true)
	{
		safeQueue.pop(number);
		if (number == -1)
		{
			//безопасная печать
			std::lock_guard<std::mutex> lock(printing_mutex);
			std::cout << "consumer is shutdowning" << std::endl;
			break;
		}
		else
		{
			std::lock_guard<std::mutex> lock(printing_mutex);
			if (isPrime(number) == true)
			{
				std::cout << number << " is prime" << std::endl;
			}
			else
			{
				std::cout << number << " is not prime" << std::endl;
			}
		}
	}
}

int main()
{
	std::vector<std::thread> consumers;
	std::thread producer(producer_function);
	for (int i = 0; i < numberOfConsumers; ++i) 
	{
		consumers.emplace_back(consumer_function, i);
	}
	if (producer.joinable())
	{
		producer.join();
	}
	for (int i = 0; i < numberOfConsumers; i++)
	{
		if (consumers[i].joinable())
		{
			consumers[i].join();
		}
	}
	return 0;
}

	









