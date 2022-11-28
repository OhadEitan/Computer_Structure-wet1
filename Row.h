//
// Created by idanm on 20/11/2022.
//

#ifndef WET01_ROW_H
#define WET01_ROW_H
#include <stdint.h>


class Row {
public:
    uint32_t tag;
    uint32_t target;
    bool valid;

    Row();
};


#endif //WET01_ROW_H
