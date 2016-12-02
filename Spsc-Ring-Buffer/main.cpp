#include "spsc_ring_buffer.h"
#include <thread>
#include <future>
#include <iostream>

#define CAPACITY 1024
#define TESTS 10000000

//�����-��������� ��������������� ��������� � ������� N  ����� �� 1 �� N, ������ ������� �� �����

long producer_work_loop(spsc_ring_buffer<int>& buffer)
{
	long sum = 0;
	for (int i = 0; i < TESTS; i++)
	{
		while (!buffer.enqueue(i))
		{
			std::this_thread::yield();
		}
		sum += i;
	}
	return sum;
}

long consumer_work_loop(spsc_ring_buffer<int>& buffer)
{
	long sum = 0;
	for (int i = 0; i < TESTS; i++)
	{
		int elem;
		while (!buffer.dequeue(elem)) 
		{
			std::this_thread::yield(); //�������� ���������� ������� ������, �������� � ������������� �� ������� ����������
		}
		sum += elem;
	}
	return sum;
}


int main()
{
	spsc_ring_buffer<int> buf(CAPACITY);
	//��������� ������� consumer � producer ����������
	std::future<long> produced_sum = async(std::launch::async, producer_work_loop, std::ref(buf));
	std::future<long> consumed_sum = async(std::launch::async, consumer_work_loop, std::ref(buf));
	if (produced_sum.get() - consumed_sum.get() == 0)
	{
		std::cout << "OK" << std::endl;
	}
	else
	{
		std::cout << "WRONG ANSWER" << std::endl;
	}
	return 0;
}