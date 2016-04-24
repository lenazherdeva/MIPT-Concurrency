#include "semaphore.h"
#include <thread>
#include <iostream>

void step_left(int step)
{
	std::cout <<step<<" left\n";
}

void step_right(int step)
{
	std::cout <<step<<" right\n";
}

int main()
{
	semaphore sem1;
	semaphore sem2;

	//поток,отвечающий за шаги влево
	//после шага сигналим о том, что поток,который отвечает за шаги вправо,должен проснуться, а поток,отвечающий за лево засыпает
	//в конце вызываем sem2.signal(), так как после шага влево, идет шаг вправо
	std::thread th_sem1([&sem1, &sem2]()
	{
		for (int i = 0; i < 7; ++i)
		{
			step_left(i);
			sem2.signal();
			sem1.wait();
		}
		sem2.signal();

	});

	//поток,отвечающий за шаги вправо
	//ждем,когда будет очередь шагать вправо
	//после шага сигналим о том, что поток,который отвечает за шаги влево,должен проснуться, а поток,отвечающий за право засыпает
	std::thread th_sem2([&sem1, &sem2]()
	{
		sem2.wait();
		for (int i = 0; i < 7; ++i) {
			step_right(i);
			sem1.signal();
			sem2.wait();
		}
	});

	if (th_sem1.joinable()) {
		th_sem1.join();
	}
	if (th_sem2.joinable()) {
		th_sem2.join();
	}
	std::cout << "success!";
	return 0;
}