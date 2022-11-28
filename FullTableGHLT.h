//
// Created by eitan on 28/11/2022.
//

#ifndef BP_CPP_FULLTABLEGHLT_H
#define BP_CPP_FULLTABLEGHLT_H
#include "RowGhLt.h"
#include "vector"

class FullTableGHLT {
public:
    std::vector<RowGhLt> rows;
    std::vector<bool> globalHistory;
    // std::vector<unsigned> fsmTable;
    int Shared;
    bool currentMachine;
    unsigned tagSize;
    unsigned fsmState;

    //FullTableGhGt(unsigned btbSize, unsigned historySize, unsigned fsmState);

};
//extern  FullTableGHLT tableGhLT;

#endif //BP_CPP_FULLTABLEGHLT_H
