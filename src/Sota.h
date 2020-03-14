#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>
#include <array>
#include <cassert>
#include <random>
#include <set>
#include "macros.h"


namespace plugin {

    namespace sota {

    using std::vector;
    using std::map;
    using std::set;
    using std::ostream;
    using std::istream;

    class iSota {
    public:
        virtual void setValue() = 0;
        virtual void printStorage() const = 0;
        virtual ~iSota(){}
    protected:
        virtual void eraseValue(int &index) = 0;

    };


    class SimpleSota: public iSota{

    DECLARE_NO_COPY_CLASS(SimpleSota);
    public:
        SimpleSota(int number);

        ~SimpleSota();

        void setValue();
        void removeRangeValue();
        void printStorage () const;
        void synchronization();

        std::string toString();
        friend ostream& operator<<(ostream& os,const SimpleSota &p);

    protected:
        void generateRandomNumber();
        void makeRandomNumbers();
        void eraseValue(int &index);
    private:
        int m_counter;
        int m_counterRemove;
        vector<int> m_vector;
        map<int, int> m_map;
        set<int> m_random_numbers;
    };
}
}