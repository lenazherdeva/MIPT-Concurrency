#include "Tree_Mutex.h"
#include <cmath>

Peterson_Mutex::Peterson_Mutex()
{
	want[0].store(false);
	want[1].store(false);
	victim.store(0);
}

//������ �������� � �������
//� ������ ������������� ��������� ������� ����� ����������� ������� ���� �� ���������� ������� ������
void Peterson_Mutex::lock(int t)
{
	want[t].store(true);
	victim.store(t);
	while (want[1 - t].load() && victim.load() == t)
	{
		std::this_thread::yield();
	}
}

// ������������ �������� � �������
void Peterson_Mutex::unlock(int t)
{
	want[t].store(false);
}

//������������� ������: ���������� ������� ��������� �� ������� 2
//������ ������ = ������� �������� �� ���������� �������
Tree_Mutex::Tree_Mutex(size_t num_threads) : powered_num_threads(2 >= pow(2, ceil(log2(num_threads))) ? 2 : pow(2, ceil(log2(num_threads)))),
vec_peterson_mutex(powered_num_threads-1)
{
	//powered_num_threads = 2 >= pow(2, ceil(log2(num_threads))) ? 2 : pow(2, ceil(log2(num_threads)));
	height = 1 > ceil(log2(powered_num_threads - 1)) ? 1 : ceil(log2(powered_num_threads - 1));
	//peterson_mutex = std::vector<Peterson_Mutex>(powered_num_threads - 1);
}

// ���������������� ������ ������� �������� � �������� ������
//node_index-������ ������� � ������,��� ����� ����������� �������
void Tree_Mutex::lock(std::size_t thread_index)
{
	if (thread_index >= powered_num_threads)
	{
		throw (std::invalid_argument("Thread_index is out of range"));
	}
	if (locked_ind.load() == thread_index)
	{
		throw (std::invalid_argument("Thread_index has already locked"));
	}
	std::size_t node_index = powered_num_threads - 1 + thread_index;
	for (std::size_t level = 0; level < height; level++)
	{
		std::size_t index = node_index % 2;
		node_index = floor((node_index - 1) / 2);
		vec_peterson_mutex[node_index].lock(index);
	}
	locked_ind.store(thread_index);
}

// ������������ ���������, ������� � ����� � ���� �� ������
// tab_order-������, � ������� �������� ����(�� ������), ���������� �������
void Tree_Mutex::unlock(std::size_t thread_index)
{
	if (thread_index >= powered_num_threads)
	{
		throw (std::invalid_argument("thread_index out of range"));
	}
	if (locked_ind.load() == thread_index)
	{
		std::vector<std::pair<int, int> > tab_order;
		size_t node_index = powered_num_threads - 1 + thread_index;
		for (size_t level = 0; level < height; ++level)
		{
			int index = node_index % 2;
			node_index = floor((node_index - 1) / 2);
			tab_order.push_back({ node_index, index });
		}
		for (int i = (int)tab_order.size() - 1; i >= 0; --i)
		{
			vec_peterson_mutex[tab_order[i].first].unlock(tab_order[i].second);
			
		}
		locked_ind.store(powered_num_threads);
	}
}







