#include <iostream>
#include <vector>
#include <iterator>
#include <list>
#include <stdexcept>

template<class KeyType, class ValueType, class Hash = std::hash<KeyType> >
class HashMap {
public:

using element = typename std::pair<const KeyType, ValueType>;
using iterator = typename std::list<element>::iterator;
using const_iterator = typename std::list<element>::const_iterator;
using bucket = std::pair<iterator, iterator>;

HashMap(Hash hasher_ = Hash()) : hasher(hasher_) {
    HashTable.resize(array_size, std::make_pair(end(), end()));
    HT_size = 0;
}

template<class Iter>
HashMap(Iter first, Iter last, Hash hasher_ = Hash(), size_t ar_sz = 8) : hasher(hasher_) {
    array_size = ar_sz;
    HashTable.resize(array_size, std::make_pair(end(), end()));
    HT_size = 0;
    while (first != last) {
        insert(*first);
        ++first;
    }
}

HashMap(std::initializer_list<element> l, Hash hasher_ = Hash()) : HashMap(l.begin(), l.end(), hasher_) {}

HashMap(HashMap& other) {
    if (this == &other)
        return;
    array_size = other.array_size;
    HashTable.resize(array_size, std::make_pair(end(), end()));
    hasher = other.hasher;
    for (const auto& el : other.elem_list) {
        insert(el);
    }
}

HashMap& operator = (HashMap& other) {
    if (this == &other) {
        return *this;
    }
    clear();
    hasher = other.hasher;
    for (const element& el : other) {
        insert(el);
    }
    return *this;
}

Hash hash_function() const {
    return hasher;
}

void insert(const element& new_el) {
    auto it = find(new_el.first);
    if (it != end())
        return;
    size_t pos = hasher(new_el.first) % array_size;
    if (HashTable[pos].first == end()) {
        HashTable[pos].first = HashTable[pos].second = elem_list.insert(elem_list.end(), new_el);
        ++HT_size;
        rehash();
        return;
    }
    HashTable[pos].first = elem_list.insert(HashTable[pos].first, new_el);
    ++HT_size;
    rehash();
}

void erase(const KeyType& key) {
    auto it = find(key);
    size_t pos = hasher(key) % array_size;
    if (it == end())
    	return;
    bool one_elem = (HashTable[pos].first == HashTable[pos].second);
    if (it == HashTable[pos].first) {
        HashTable[pos].first = elem_list.erase(it);
    } else if (it == HashTable[pos].second) {
        HashTable[pos].second = --elem_list.erase(it);
    } else {
        elem_list.erase(it);
    }
    if (one_elem) {
        HashTable[pos] = std::make_pair(end(), end());
    }
    --HT_size;
}

iterator find(const KeyType& key) {
    size_t pos = hasher(key) % array_size;
    if (HashTable[pos].first == end())
        return end();
    for (iterator it = HashTable[pos].first; it != next(HashTable[pos].second); ++it) {
        if ((*it).first == key)
            return it;
    }
    return end();
}
const_iterator find(const KeyType& key) const {
    size_t pos = hasher(key) % array_size;
    if (HashTable[pos].first == end())
        return end();
    for (const_iterator it = HashTable[pos].first; it != next(HashTable[pos].second); ++it) {
        if ((*it).first == key)
            return it;
    }
    return end();
}

iterator begin() {
    return elem_list.begin();
}
iterator end() {
    return elem_list.end();
}

const_iterator begin() const {
    return elem_list.begin();
}
const_iterator end() const {
    return elem_list.end();
}

ValueType& operator [] (const KeyType& key) {
    insert(std::make_pair(key, ValueType()));
    return (*find(key)).second;
}
const ValueType& at(const KeyType& key) const {
    auto it = find(key);
    if (it != end())
        return (*it).second;
    throw std::out_of_range("no such key in HashMap");
}

void clear() {
    elem_list.clear();
    HashTable.clear();
    HashTable.resize(array_size, std::make_pair(end(), end()));
    HT_size = 0;
}

size_t size() const {
    return HT_size;
}

bool empty() const {
    return HT_size == 0;
}

void rehash() {
    if (HT_size <= array_size)
        return;
    std::vector<element> tmp;
    for (auto it = begin(); it != end(); ++it) {
        tmp.push_back(*it);
    }
    array_size *= 2;
    clear();
    for (auto& el : tmp) {
        insert(el);
    }
}

private:

size_t array_size = 8;
size_t HT_size = 0;
std::vector<bucket> HashTable;
std::list<element> elem_list;
Hash hasher;
};
