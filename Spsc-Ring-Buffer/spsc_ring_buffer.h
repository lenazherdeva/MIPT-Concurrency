#ifndef SPSC_RING_BUFFER
#define SPSC_RING_BUFFER

#include <vector>
#include <atomic>

//�������������� ������� �� ������ ������ ������������� �����

template <typename T>
class spsc_ring_buffer
{
public:
	explicit spsc_ring_buffer(std::size_t _capacity): ring_buffer(_capacity + 1),tail(0), head(0), capacity(_capacity + 1) {}
	bool enqueue(T e);
	bool dequeue(T & e);
private:
	std::vector<T> ring_buffer;
	std::atomic<std::size_t> tail;
	std::atomic<std::size_t> head;
	const std::size_t capacity;
};

//� equeue � tail �������� ������ producer, ������ ������ tail ����� relaxed
//������ ����������, ����� � consumer � producer ������ ��������� � tail, ������� ����� release
template <typename T>
inline bool spsc_ring_buffer<T>::enqueue(T e)
{
	std::size_t current_tail = tail.load(std::memory_order_relaxed);
	std::size_t current_head = head.load(std::memory_order_acquire);

	//����� ��������� ��� ��������� (������������ � �������), ���������,
	//����� ����� ������� � ������� ������� � ������ ��������� �� ������� ���� ���� ��������� ����
	if ((current_tail + 1) % capacity == current_head)
	{
		return false;
	}
	ring_buffer[current_tail] = std::move(e);
	tail.store((current_tail + 1) % capacity, std::memory_order_release);
	return true;
}

//� dequeue � head �������� ������ consumer, ������ ������ head ����� relaxed
//������ ����������, ����� � consumer � producer ������ ��������� � head, ������� ����� release
template <typename T>
inline bool spsc_ring_buffer<T>::dequeue(T& e)
{
	std::size_t current_head = head.load(std::memory_order_relaxed);
	std::size_t current_tail = tail.load(std::memory_order_acquire);

	if (current_head == current_tail) {
		return false;
	}
	e = ring_buffer[current_head];
	head.store((current_head + 1) % capacity, std::memory_order_release);
	return true;
}

#endif 



