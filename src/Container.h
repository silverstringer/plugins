#pragma once

#include <vector>
#include <mutex>
#include <memory>
#include <cassert>
#include <algorithm>

template<typename T>
class Container {
public:
    explicit Container();
     ~Container();

public:
    void addItem(T *item);

    bool contains(T *item);

    T *getItem(T *item);

    std::vector<T *> getAllItem(T *item);

    void removeItem(T *item);

    size_t counterItem();

protected:

private:
    std::vector<T *> m_container;
    std::recursive_mutex m_mutex;
};