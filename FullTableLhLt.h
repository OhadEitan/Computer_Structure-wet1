//
// Created by idanm on 20/11/2022.
//

#ifndef WET01_FULLTABLELHLT_H
#define WET01_FULLTABLELHLT_H
#include "RowLhLt.h"

class FullTableLhLt {
public:
    std::vector<RowLhLt> rows;
    bool currentMachine;
    unsigned tagSize;
    unsigned fsmState;
};


#endif //WET01_FULLTABLELHLT_H
