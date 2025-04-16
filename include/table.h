#pragma once
#include<iostream>
#include<vector>
#include<list>
#include<algorithm>

using std::size_t;
using std::pair;

template <typename T, typename CellType=pair<size_t,T>>
class TableInterface
{
public:
	virtual ~TableInterface() = default;
	virtual bool insert(size_t key, const T& data)=0;
	virtual bool erase(size_t key) = 0;
	virtual bool is_empty() = 0;
	virtual size_t size() = 0;
	virtual void clear() = 0;
	virtual CellType find(size_t key) = 0;
};

template <typename T, typename CellType = pair<size_t, T>>
class TableByArray : public TableInterface<T, CellType>
{
protected:
	using vector = std::vector<CellType>;
	vector data;
	size_t sz;
public:
	TableByArray():sz(0){};

	virtual ~TableByArray() = default;

	bool is_empty() override
	{
		return data.empty();
	}

	size_t size() override
	{
		return sz;
	}

	void clear() override
	{
		data.clear();
		sz = 0;
	}
};

template <typename T, typename CellType = pair<size_t, T>>
class UnorderedTable : public TableByArray<T, CellType>
{
protected:
	using TableByArray<T, CellType>::data;
	using TableByArray<T, CellType>::sz;
public:
	UnorderedTable(size_t _sz=0, const typename TableByArray<T, CellType>::vector& _data={})
	{
		sz = _sz;
		data = _data;
	}

	UnorderedTable(const UnorderedTable& t) : TableByArray<T, CellType>(t) {}

	CellType find(size_t key) override
	{
		for (size_t i = 0; i < data.size(); i++)
		{
			if (data[i].first == key)
				return data[i];
		}
		throw std::logic_error("Key not found");
	}

	bool insert(size_t key, const T& val) override
	{
		for (size_t i = 0; i <this->sz; ++i) 
		{
			if (data[i].first == key) return false;
		}
		data.emplace_back(key, val);
		sz = data.size();
		return true;
	}

	bool erase(size_t key) override
	{
			for (auto it = data.begin(); it != data.end(); ++it)
			{
				if (it->first == key)
				{
					data.erase(it);
					sz--;
					return true;
				}
			}
			return false;
	}
};

template <typename T, typename CellType=pair<size_t, T>>
class OrderedTable : public TableByArray<T, CellType>
{
protected:
	using TableByArray<T, CellType>::data;
	using TableByArray<T, CellType>::sz;

	size_t find_pos(size_t key) const
	{
		size_t left = 0;
		size_t right = sz;

		while (left < right) {
			size_t mid = left + (right - left) / 2;
			if (data[mid].first < key) {
				left = mid + 1;
			}
			else {
				right = mid;
			}
		}
		return left;
	}
public:

	OrderedTable(size_t _sz = 0, const typename TableByArray<T, CellType>::vector& _data = {})
	{
		data = _data;
		sz = _sz;
		std::sort(data.begin(), data.end(), [](const auto& a, const auto& b) { return a.first < b.first; });
	}

	OrderedTable(const OrderedTable& t) : TableByArray<T, CellType>(), sz(t.sz), data(t.data) {}

	bool insert(size_t key, const T& val) override
	{
		size_t pos = find_pos(key);
		if (pos < sz && data[pos].first == key)
		{
			return false;
		}
		data.insert(data.begin() + pos, std::make_pair(key, val));
		sz++;
		return true;
	}

	CellType find(size_t key) override 
	{
		size_t pos = find_pos(key);
		if (pos < sz && data[pos].first == key) {
			return data[pos];
		}
		throw std::logic_error("Key not found");
	}

	bool erase(size_t key) override {
		size_t pos = find_pos(key);
		if (pos < sz && data[pos].first == key) {
			data.erase(data.begin() + pos);
			sz--;
			return true;
		}
		return false;
	}
};

template <typename T, typename CellType = pair<size_t, T>>
class HashTable :public TableInterface<T, CellType>
{
protected:
	static const size_t DefaultCapacity = 16;
	static constexpr double IsLoad=0.75;

	std::vector<std::list<CellType>> table;
	size_t sz;
	size_t capacity;
	const size_t a = 2654435761;  
	const size_t b = 0xdeadbeef;

	size_t hash(size_t key) const noexcept
	{
		key ^= (key >> 16);
		key *= a;
		key ^= (key >> 16);
		return key % capacity;
	}

	void rehash()
	{
		size_t new_capacity = capacity * 2;
		if (new_capacity < capacity) 
		{
			throw std::overflow_error("HashTable capacity overflow");
		}
		std::vector<std::list<CellType>> new_table(new_capacity);
		const size_t old_capacity = capacity;
		capacity = new_capacity;
		for (const auto& chain : table) {
			for (const auto& item : chain) {
				size_t new_index = hash(item.first);
				new_table[new_index].push_back(item);
			}
		}
		table = std::move(new_table);
	}
public:
	HashTable() : sz(0), capacity(DefaultCapacity)
	{
		table.resize(capacity);
	}

	HashTable(size_t _capacity) : sz(0), capacity(DefaultCapacity)
	{
		if (_capacity > 0) {
			capacity = _capacity;
		}
		table.resize(capacity);
	}

	bool insert(size_t key, const T& val) override
	{
		if (static_cast<double>(sz) / capacity >= IsLoad)
		{
			rehash();
		}
		size_t index = hash(key);
		auto& chain = table[index];
		for (const auto& item : chain)
		{
			if (item.first == key)
			{
				return false;
			}
		}
		chain.emplace_back(key, val);
		sz++;
		return true;
	}

	bool erase(size_t key) override
	{
		size_t index = hash(key);
		auto& chain = table[index];

		for (auto it = chain.begin(); it != chain.end(); ++it)
		{
			if (it->first == key)
			{
				chain.erase(it);
				sz--;
				return true;
			}
		}
		return false;
	}

	CellType find(size_t key) override
	{
		size_t index = hash(key);
		const auto& chain = table[index];

		for (const auto& item : chain)
		{
			if (item.first == key)
			{
				return item;
			}
		}

		throw std::logic_error("Key not found");
	}

	bool is_empty() override
	{
		return sz == 0;
	}

	size_t size() override
	{
		return sz;
	}

	void clear() override
	{
		for (auto& chain : table)
		{
			chain.clear();
		}
		sz = 0;
	}
};