#ifndef RAWQUOTES_H
#define RAWQUOTES_H

#include "../headers.h"
#include "quote.h"

namespace qsh {

class RawQuotes
{
public:
    RawQuotes();
    void clear();
    int& operator[] (int key);
    void remove(int key);
    std::vector<Quote> quotes(size_t limit);

private:
    std::map<int, int, std::greater<int>> m_quotes;
};

}

#endif // RAWQUOTES_H
