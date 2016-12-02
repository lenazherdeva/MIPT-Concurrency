#include <mutex>
#include <vector>
#include <forward_list>
#include <deque>
#include <mutex>

template <typename T, class H = std::hash<T>>
class striped_hash_set
{
public:
	striped_hash_set(int _num_stripes = 1, int _growth_factor = 2, double _load_factor = 100.)
		: num_stripes(_num_stripes), growth_factor(_growth_factor), load_factor(_load_factor), num_elements(0)
	{
		locks.resize(_num_stripes);
		table.resize(_num_stripes * _growth_factor);
	}
	void add(const T& e);
	bool contains(const T& e);
	void remove(const T& e);	
private:
	int num_stripes;
	int growth_factor; // мультипликативный коэффициент роста хэш-таблицы
	double load_factor; // отношение числа элементов к числу корзин
	std::atomic<int> num_elements; // текущее число элементов в таблице
	std::deque<std::mutex> locks;
	std::vector<std::forward_list<T>> table;

	int get_bucket_index(size_t hash_value)
	{
		return hash_value % ((int)table.size());
	}

	int get_stripe_index(size_t hash_value)
	{
		return hash_value % num_stripes;
	}
	void expand(std::size_t last_size);
};

// добавляет элемент е
// таблица расширяется, если текущее значение загруженности таблицы превысило load_factor
template <typename T, class H>
inline void striped_hash_set<T, H>::add(const T& e)
{
	std::size_t hash_value = H()(e);
	// блокируем подходящий мьютекс
	std::unique_lock<std::mutex> lock(locks[get_stripe_index(hash_value)]);
	table[get_bucket_index(hash_value)].push_front(e);
	num_elements.fetch_add(1);
	// расширение
	if (((float)num_elements.load()) / ((float)table.size()) > load_factor)
	{
		std::size_t last_size = table.size();
		lock.unlock();
		expand(last_size);
	}
}

template <typename T, class H>
inline void striped_hash_set<T, H>::expand(std::size_t last_size)
{
	std::vector<std::unique_lock<std::mutex>> lk;
	// блокируем первый мьютекс и проверяем, не расширил ли кто-то таблицу за нас
	lk.emplace_back(locks[0]);
	if (table.size() == last_size)
	{
		for (std::size_t i = 1; i < locks.size(); i++)
		{
			lk.emplace_back(locks[i]);
		}
		std::size_t new_size = last_size * growth_factor;
		std::vector<std::forward_list<T>> new_table(new_size);
		// заполняем новую таблицу
		for (std::size_t i = 0; i < last_size; i++)
		{
			for (auto it = table[i].begin(); it != table[i].end(); it++)
			{
				size_t hash_value = H()(*it);
				new_table[hash_value % new_size].push_front(*it);
			}
		}
		table = new_table;
	}
}

// расширение таблицы:поток захватывает все мьютексы хэш-таблицы
template <typename T, class H>
bool striped_hash_set<T,H>:: contains(const T& e)
{
	std::size_t hash_value = H()(e);
	std::unique_lock<std::mutex> lock(locks[get_stripe_index(hash_value)]);
	for (T elem : table[get_bucket_index(hash_value)])
	{
		if (elem == e)
			return true;
	}
	return false;
}

// удаляем е
// уменьшаем количество элементов в таблице
template <typename T, class H>
void striped_hash_set<T,H>::remove(const T& e)
{
	size_t hash_value = H()(e);
	std::unique_lock<std::mutex> lock(locks[get_stripe_index(hash_value)]);
	table[get_bucket_index(hash_value)].remove(e);
	num_elements.fetch_sub(1);
}