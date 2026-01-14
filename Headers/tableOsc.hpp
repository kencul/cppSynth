#ifndef TABLEOSC_HPP
#define TABLEOSC_HPP

#include <vector>
#include <utility>

#include "osc.hpp"


class TableOsc : public Osc{
public:
    // && : rvalue reference
    TableOsc(double amp = 1, int freq = 440, std::vector<double>& table, size_t outputBufferSize)
    : lookupTable(table), outputBuffer(outputBufferSize), outputBufferSize(outputBufferSize){
        Osc(2);
        lookupTableSize = table.size();
    }

protected:
    

private:
    std::vector<double> lookupTable;
    size_t lookupTableSize;
    std::vector<double> outputBuffer;
    size_t outputBufferSize;
};

#endif