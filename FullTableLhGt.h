//
// Created by idanm on 20/11/2022.
//

#ifndef WET01_FULLTABLELHGT_H
#define WET01_FULLTABLELHGT_H
#include "RowLh.h"

class FullTableLhGt {
public:
    std::vector<RowLh> rows;
    std::vector<unsigned> fsmTable;
    int Shared;
    bool currentMachine;
    unsigned tagSize;
    unsigned fsmState;

};


#endif //WET01_FULLTABLELHGT_H
