
#include "Container.h"

template <typename T>
Container<T>::Container() {

}

template <typename T>
Container<T>::~Container() {

}
template <typename T>
void Container<T>::addItem(T *item) {
    std::lock_guard<std::recursive_mutex> lock(m_mutex);

    m_container.push_back(item);

}
template <typename T>
bool Container<T>::contains(T *item) {

    if (std::find(m_container.begin(), m_container.end(), item) != m_container.end())
        return true;
    return false;
}

template <typename T>
std::vector<T *> Container<T>::getAllItem(T *item) {

    return m_container;
}

template <typename T>
T *Container<T>::getItem(T *item) {
    if (std::find(m_container.begin(), m_container.end(), item) != m_container.end())
        return item;
}
template <typename T>
void Container<T>::removeItem(T *item) {

    std::lock_guard<std::recursive_mutex> lock(m_mutex);

    auto position = std::find(m_container.begin(), m_container.end(), item);
    if (position != m_container.end())
        m_container.erase(position);

}
template <typename T>
size_t Container<T>::counterItem() {

//    assert(m_container.size() > 0 && "Size is empty");

    return m_container.size();
}