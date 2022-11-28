//
// Created by idanm on 20/11/2022.
//

#ifndef WET01_ROWLH_H
#define WET01_ROWLH_H
#include "Row.h"
#include "vector"

class RowLh : public Row {
public:
    std::vector<bool> history;
};


#endif //WET01_ROWLH_H
