#include "Sota.h"

namespace plugin {

    namespace sota{


    SimpleSota::SimpleSota(int number) : m_counter(number) {

        generateRandomNumber();
        makeRandomNumbers();

    }

    SimpleSota::~SimpleSota() {};


    void SimpleSota::setValue() {
        for (int i = 0; i < m_counter; i++) {
            m_vector.emplace_back(rand() % 10);
            m_map.insert(std::make_pair(i, rand() % 10));
        }
    }

    void SimpleSota::eraseValue(int &index) {
//    assert(index <= static_cast<uint64_t >(m_vector.size()));
//    assert(index < static_cast<uint64_t >(m_map.size()));
        if (index > m_vector.size()) {
            index = 0;
            std::vector<int>::iterator iter = m_vector.begin();
            m_vector.erase(iter + index);
        }

        m_map.erase(index);
    }


    void SimpleSota::removeRangeValue() {

        for (auto value:m_random_numbers) {
            try {
                eraseValue(value);
            }
            catch (std::exception &ex) {
                std::cout << ex.what();
            }
        }

    }


    void SimpleSota::makeRandomNumbers() {
        std::random_device rd;
        std::mt19937 data(rd());
        std::uniform_int_distribution<> distr(0, m_counter);

        while (m_random_numbers.size() < m_counterRemove)
            m_random_numbers.insert(distr(data));


    }

    void SimpleSota::generateRandomNumber() {
        std::random_device rd;
        std::mt19937 data(rd());
        std::uniform_int_distribution<> distr(1, 15);
        m_counterRemove = distr(data);
    }


    void SimpleSota::printStorage() const {
        std::cout << "Map:"<< "\t";
        for (auto &value:m_map)
            std::cout << value.second << "\t";

        std::cout << "\nVector:"<< "\t";
        for (auto &value:m_vector)
            std::cout << value << "\t";

    }


    void SimpleSota::synchronization() {
        for (auto &value:m_vector) {


            auto result = std::find_if(m_map.begin(), m_map.end(), [&value](const auto &sm) { return sm.second == value; });

            if (result != m_map.end()) {
                int foundkey = result->first;
            }
            else
            {
                m_vector.erase(std::remove(begin(m_vector), end(m_vector), value), end(m_vector));
            }
        }


        for (auto &value:m_map) {
            auto result = std::find_if(m_vector.begin(), m_vector.end(), [&](auto &sm) { return sm == value.second; });

            if (result != m_vector.end()) {

            }
            else
            {
                std::map<int, int>::iterator it = m_map.find(value.first);
                if(it != m_map.end())
                {
                    m_map.erase(it);
                }
            }
        }


        std::sort(m_vector.begin(),m_vector.end());

    }

    std::string SimpleSota::toString()
    {
        std::string tmp;
        for (auto &value:m_vector)
            tmp +=" "+std::to_string(value);
        return tmp;

    }

    ostream& operator<<(ostream& os,const SimpleSota &p)
    {
        for (auto &value:p.m_vector)
            os<<value;
        return os;
    }
    }
}