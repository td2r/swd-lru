//
// Created by ivanln on 14.10.22.
//

#pragma once

#define DEBUG

#include <list>
#include <unordered_map>
#include <stdexcept>
#include <cassert>

template<typename Key, typename Value>
struct lru_cache_t {
    static std::size_t const DEFAULT_CAPACITY = 256;

    explicit lru_cache_t(std::size_t capacity = DEFAULT_CAPACITY);
    ~lru_cache_t() = default;

    Value* get(Key const&);
    void put(Key const&, Value const&);

private:
    using list_t = std::list<std::pair<Key, Value>>;
    using hashmap_t = std::unordered_map<Key, typename list_t::iterator>;

private:
    std::size_t const cap;
    list_t list;
    hashmap_t hashmap;

private:
    void assert_invariant() const;
};

template<typename Key, typename Value>
lru_cache_t<Key, Value>::lru_cache_t(std::size_t capacity)
: cap(capacity)
, list()
, hashmap()
{
    if (capacity == 0) {
        throw std::invalid_argument("capacity must be positive number");
    }
    assert_invariant();
}

template<typename Key, typename Value>
Value* lru_cache_t<Key, Value>::get(const Key& key) {
    assert_invariant();
    auto const it = hashmap.find(key);
    if (it == hashmap.end())
        return nullptr;
    list.splice(list.end(), list, it->second);
    assert_invariant();
    return &it->second->second;
}

template<typename Key, typename Value>
void lru_cache_t<Key, Value>::put(const Key& key, const Value& value) {
    assert_invariant();
    auto const it = hashmap.find(key);
    if (it != hashmap.end()) {
        list.erase(it->second);
        it->second = list.insert(list.end(), std::make_pair(key, value));
    } else {
        if (hashmap.size() == cap) {
            hashmap.erase(list.begin()->first);
            list.erase(list.begin());
        }
        hashmap.insert(std::make_pair(key, list.insert(list.end(), std::make_pair(key, value))));
    }
    assert_invariant();
}

template<typename Key, typename Value>
void lru_cache_t<Key, Value>::assert_invariant() const {
#ifdef DEBUG
    assert(list.size() == hashmap.size());
    assert(hashmap.size() <= cap);
    for (auto it = list.begin(); it != list.end(); ++it) {
        assert(hashmap.at(it->first) == it);
    }
#endif
}
