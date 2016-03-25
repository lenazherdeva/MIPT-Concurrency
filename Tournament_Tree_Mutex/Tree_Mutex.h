#ifndef TREE_MUTEX
#define TREE_MUTEX
#include <iostream>
#include <thread>
#include <atomic>
#include <vector>
#include <array>
#include<cmath>	
#include "Peterson_Mutex.h"

//������� ��� ��������� ���������� ��� n ������� �� ������ ���������� ������ � ��������� ���������
class Tree_Mutex
{
public:
	Tree_Mutex() = default;
	Tree_Mutex(std::size_t num_threads);
	//������ �������� ������� � ������� �� 0 �� num_treads
	void lock(std::size_t thread_index);
	///������������ �������� ������� 
	void unlock(std::size_t thread_index);

private:
	std::size_t height; // ������ ������
	std::size_t powered_num_threads; // ���������� �������, ����������� � ������� 2	
	std::vector<Peterson_Mutex> vec_peterson_mutex; //��������
	std::atomic<size_t> locked_ind; // ������� �������, ������� ��������� �������

};

#endif

