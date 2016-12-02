#ifndef lock_free_queue_h
#define lock_free_queue_h

#include <memory>
#include <atomic>

//lock - free очередь имени Michael и Scott
template <typename T>
class lock_free_queue
{
public:
	void enqueue(T item);
	bool dequeue(T& item);
	lock_free_queue() : head(new node), tail(head.load()), to_be_removed(head.load()) {}
	~lock_free_queue();
private:
	struct node
	{
		std::shared_ptr<T> data;
		std::atomic<node*> next;
		node() : next(nullptr) {}
		node(T& item) : data(new T(item)), next(nullptr) {}
	};
	std::atomic<node*> head;
	std::atomic<node*> tail;
	std::atomic<node*> to_be_removed; // узлы, подлежащие удалению
	std::atomic<std::size_t> threads_in_pop; //считаем потоки, которые в данный момент пытаются извлечь элемент из стека
};

template<typename T>
lock_free_queue<T>::~lock_free_queue()
{
	for (node* it = to_be_removed.load(); it != nullptr;){
		node* next = it->next.load();
		delete it;
		it = next;
	}
}

template<typename T>
void lock_free_queue<T>::enqueue(T item)
{
	node* new_node = new node(item);
	node* curr_tail;
	threads_in_pop.fetch_add(1);
	while (true)
	{
		node* curr_tail = tail.load();
		node* curr_tail_next = curr_tail->next;

		if (!curr_tail_next)
		{
			if (tail.load()->next.compare_exchange_weak(curr_tail_next, new_node)) // 1
				break;
		}
		else
			tail.compare_exchange_weak(curr_tail, curr_tail_next); // helping
	}
	tail.compare_exchange_weak(curr_tail, new_node); // 2
	threads_in_pop.fetch_sub(1);
}

// удаление из очереди - это просто сдвиг указателя head вперед
template<typename T>
bool lock_free_queue<T>::dequeue(T& item)
{
	threads_in_pop.fetch_add(1);
	node* curr_head_next;
	while (true)
	{
		node* curr_head = head.load();
		node* curr_tail = tail.load();
		curr_head_next = curr_head->next;
		if (curr_head == curr_tail)
		{
			if (!curr_head_next)
				return false;
			else
				tail.compare_exchange_weak(curr_head, curr_head_next); // helping
		}
		else
		{
			if (head.compare_exchange_weak(curr_head, curr_head_next))
			{
				item = *curr_head_next->data;
				return true;
			}
		}
	}
	// пытаемся удалить узлы, которые подлежат удалению, если потоков, вызывающих dequeue нет вообще
	// иначе эти узлы будут ожидать удаления до момента уничтожения стека
	if (threads_in_pop.load() == 1)
	{
		node* nodes_to_delete = to_be_removed.exchange(curr_head_next); //заявляем права на список подлежащих удалению узлов
		while (nodes_to_delete != to_be_removed.load()) //обходим список и удаляем узлы
		{
			node* tmp = nodes_to_delete->next.load();
			delete nodes_to_delete;
			nodes_to_delete = tmp;
		}
	}
	threads_in_pop.fetch_sub(1);
	return true;
}

#endif 