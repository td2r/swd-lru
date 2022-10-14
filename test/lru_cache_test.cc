#include "../lru_cache/lru_cache.h"

#include <gtest/gtest.h>

#include <stdexcept>

#define ASSERT_EQ_BY_PTR(ptr, val) \
    ASSERT_NE(ptr, nullptr);       \
    ASSERT_EQ(*ptr, val)

#define ASSERT_KEY_VALUE(cache, key, val) \
    ASSERT_EQ_BY_PTR(cache.get(key), val)

#define ASSERT_NO_KEY(cache, key) \
    ASSERT_EQ(cache.get(key), nullptr)

TEST(Base, ZeroCapacity) {
    auto lmb = []() {
        lru_cache_t<int, int> lru_cache(0);
    };
    ASSERT_THROW(lmb(), std::invalid_argument);
}

TEST(SinglePut, IntInt) {
    lru_cache_t<int, int> lru_cache;
    lru_cache.put(1, 2);
    ASSERT_KEY_VALUE(lru_cache, 1, 2);
}

TEST(SinglePut, IntString) {
    lru_cache_t<int, std::string> lru_cache;
    lru_cache.put(1, "hello");
    ASSERT_KEY_VALUE(lru_cache, 1, "hello");
}

TEST(SinglePut, StringInt) {
    lru_cache_t<std::string, int> lru_cache;
    lru_cache.put("world", 2);
    ASSERT_KEY_VALUE(lru_cache, "world", 2);
}

TEST(SinglePut, StringString) {
    lru_cache_t<std::string, std::string> lru_cache;
    lru_cache.put("hello", "world");
    ASSERT_KEY_VALUE(lru_cache, "hello", "world");
}

TEST(SinglePut, NonExistentKey) {
    lru_cache_t<int, int> lru_cache;
    lru_cache.put(1, 2);
    ASSERT_NO_KEY(lru_cache, 2);
}

TEST(MultiplePut, ValueReplacement) {
    lru_cache_t<std::string, int> lru_cache;
    lru_cache.put("A", 1);
    lru_cache.put("A", 2);
    ASSERT_KEY_VALUE(lru_cache, "A", 2);
}

TEST(MultiplePut, DiffKeysSameValues) {
    lru_cache_t<std::string, int> lru_cache;
    lru_cache.put("A", 42);
    lru_cache.put("B", 42);
    ASSERT_KEY_VALUE(lru_cache, "A", 42);
    ASSERT_KEY_VALUE(lru_cache, "B", 42);

    lru_cache.put("B", 43);
    ASSERT_KEY_VALUE(lru_cache, "A", 42);
    ASSERT_KEY_VALUE(lru_cache, "B", 43);
}

TEST(LRU_Discard, SimpleReplacement) {
    lru_cache_t<std::string, int> cache(3);
    cache.put("A", 0);
    cache.put("B", 1);
    cache.put("C", 2);
    cache.put("D", 3);
    ASSERT_NO_KEY(cache, "A");
    ASSERT_KEY_VALUE(cache, "B", 1);
    ASSERT_KEY_VALUE(cache, "C", 2);
    ASSERT_KEY_VALUE(cache, "D", 3);
}

TEST(LRU_Discard, GetReplacement) {
    lru_cache_t<std::string, int> cache(2);
    cache.put("A", 1);
    cache.put("B", 2);
    cache.get("A");
    cache.put("C", 3);
    ASSERT_KEY_VALUE(cache, "A", 1);
    ASSERT_NO_KEY(cache, "B");
    ASSERT_KEY_VALUE(cache, "C", 3);
}

TEST(LRU_Discard, PutReplacement) {
    lru_cache_t<std::string, int> cache(2);
    cache.put("A", 1);
    cache.put("B", 2);
    cache.put("A", 11);
    cache.put("C", 3);
    ASSERT_KEY_VALUE(cache, "A", 11);
    ASSERT_NO_KEY(cache, "B");
    ASSERT_KEY_VALUE(cache, "C", 3);
}

TEST(LRU_Discard, PutSameValueReplacement) {
    lru_cache_t<std::string, int> cache(2);
    cache.put("A", 1);
    cache.put("B", 2);
    cache.put("A", 1);
    cache.put("C", 3);
    ASSERT_KEY_VALUE(cache, "A", 1);
    ASSERT_NO_KEY(cache, "B");
    ASSERT_KEY_VALUE(cache, "C", 3);
}

TEST(LRU_Discard, DoubleReplacement) {
    lru_cache_t<std::string, int> cache(2);
    cache.put("A", 1);
    cache.put("B", 2);
    cache.get("A");
    cache.put("C", 3);
    ASSERT_KEY_VALUE(cache, "A", 1);
    ASSERT_NO_KEY(cache, "B");
    ASSERT_KEY_VALUE(cache, "C", 3);
    cache.get("A");
    cache.put("D", 4);
    ASSERT_KEY_VALUE(cache, "A", 1);
    ASSERT_NO_KEY(cache, "C");
    ASSERT_KEY_VALUE(cache, "D", 4);
}

TEST(LRU_Discard, Reorder) {
    lru_cache_t<std::string, int> cache(6);
    cache.put("A", 1);
    cache.put("B", 2);
    cache.put("C", 3);
    cache.put("D", 4);
    cache.put("E", 5);
    cache.put("F", 6);

    cache.get("C");
    cache.get("F");
    cache.get("B");
    cache.get("E");
    cache.get("D");
    cache.get("A");

    cache.put("T1", 42);
    cache.put("T2", 42);
    cache.put("T3", 42);

    ASSERT_NO_KEY(cache, "C");
    ASSERT_NO_KEY(cache, "F");
    ASSERT_NO_KEY(cache, "B");
    ASSERT_KEY_VALUE(cache, "E", 5);
    ASSERT_KEY_VALUE(cache, "D", 4);
    ASSERT_KEY_VALUE(cache, "A", 1);
}


