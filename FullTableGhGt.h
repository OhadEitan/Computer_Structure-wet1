//
// Created by idanm on 20/11/2022.
//

#ifndef WET01_FULLTABLEGHGT_H
#define WET01_FULLTABLEGHGT_H

#include "Row.h"
#include "vector"

class FullTableGhGt {
public:
    std::vector<Row> rows;
    std::vector<bool> globalHistory;
    std::vector<unsigned> fsmTable;
    int Shared;
    bool currentMachine;
    unsigned tagSize;
    unsigned fsmState;

    //FullTableGhGt(unsigned btbSize, unsigned historySize, unsigned fsmState);

};
extern  FullTableGhGt tableGhGt;


#endif //WET01_FULLTABLEGHGT_H
