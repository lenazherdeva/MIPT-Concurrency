#include <iostream>
#include <thread>
#include <atomic>
#include <array>

//��������� ������� ���������(���������������� ��� 2� �������
class Peterson_Mutex
{
public:
	Peterson_Mutex();
	//������ �������� ������� � ������� 0 ��� 1
	void lock(int t);
	//������������ �������� ������� 
	void unlock(int t);
	//����������� �����������
	Peterson_Mutex(const Peterson_Mutex &) = delete;
private:
	std::array<std::atomic<bool>, 2> want;
	std::atomic<int> victim;
};


