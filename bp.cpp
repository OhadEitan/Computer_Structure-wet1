/* 046267 Computer Architecture - Winter 20/21 - HW #1                  */
/* This file should hold your implementation of the predictor simulator */

#include "bp_api.h"
#include "iostream"
#include "FullTableGHLT.h"
#include <cmath>
using namespace std;

vector<bool> decToBin(uint32_t number);
int binToDec (vector<bool> pcBits);
int decimalXor(int x, int y);
int indexInBtb(uint32_t pc, int btbSize);
int tagFromPc(uint32_t pc, int btbSize, unsigned tagSize);
int sharedLsb(uint32_t pc, int historySize);
int sharedMid(uint32_t pc, int historySize);
void shift(vector<bool> &vec, bool taken);


FullTableGhGt tableGhGt;
FullTableLhGt tableLhGt;
FullTableLhLt tableLhLt;
FullTableGHLT tableGhLT;

SIM_stats stats;

int BP_init(unsigned btbSize, unsigned historySize, unsigned tagSize, unsigned fsmState,
			bool isGlobalHist, bool isGlobalTable, int Shared){
    
    int fsmLength = pow(2, historySize);
    //cout << tagFromPc(296, 4,16) << endl;
    //cout << indexInBtb(296, 4) << endl;
    tableGhGt.currentMachine = false;
    tableLhGt.currentMachine = false;
    tableLhLt.currentMachine = false;
    tableGhLT.currentMachine = false;
    tableGhGt.fsmState = fsmState;
    tableLhGt.fsmState = fsmState;
    tableLhLt.fsmState = fsmState;
    tableGhLT.fsmState = fsmState;


    //tableGhGt

    if(isGlobalHist && isGlobalTable) {
        tableGhGt.currentMachine = true;
        tableGhGt.Shared = Shared;
        tableGhGt.tagSize = tagSize;
        tableGhGt.rows = vector<Row>(btbSize);
        vector<bool> globalHistoryInst(historySize, false);
        tableGhGt.globalHistory = globalHistoryInst;
        tableGhGt.fsmTable = vector<unsigned>(fsmLength,fsmState);

    }
    //tableLhGt

    if(!isGlobalHist && isGlobalTable) {
        tableLhGt.currentMachine = true;
        tableLhGt.Shared = Shared;
        tableLhGt.tagSize = tagSize;
        tableLhGt.rows = vector<RowLh>(btbSize);
        for (int i = 0; i < tableLhGt.rows.size(); i++) {
            tableLhGt.rows[i].history = vector<bool>(historySize, false);
        }
        tableLhGt.fsmTable = vector<unsigned>(fsmLength, fsmState);

    }
    //   tableLhLt

    if(!isGlobalTable && !isGlobalHist) {
        tableLhLt.currentMachine = true;
        tableLhLt.tagSize = tagSize;
        tableLhLt.rows = vector<RowLhLt>(btbSize);

        for (int i = 0; i < tableLhLt.rows.size(); i++) {
            tableLhLt.rows[i].fsmTable = vector<unsigned>(fsmLength, fsmState);
            tableLhLt.rows[i].history = vector<bool>(historySize, false);
        }
    }
    if(!isGlobalTable && isGlobalHist) {
        tableGhLT.currentMachine = true;
        tableGhLT.Shared = Shared;
        tableGhLT.tagSize = tagSize;
        tableGhLT.rows = vector<RowGhLt>(btbSize);
        vector<bool> globalHistoryInst(historySize, false);
        tableGhLT.globalHistory = globalHistoryInst;

        for (int i = 0; i < tableGhLT.rows.size(); i++) {
            tableGhLT.rows[i].fsmTable = vector<unsigned>(fsmLength, fsmState);
        }
    }
    return 0;
}

bool BP_predict(uint32_t pc, uint32_t *dst) {
    int btbSize = 0;
    int indexInBtbDec = 0;
    unsigned resPredict = 0;
    int indexShared = 0;

    if (tableGhGt.currentMachine) {
        btbSize = tableGhGt.rows.size();

        indexInBtbDec = indexInBtb(pc, btbSize);

        if (tableGhGt.rows[indexInBtbDec].valid &&
            tableGhGt.rows[indexInBtbDec].tag == tagFromPc(pc,btbSize,tableGhGt.tagSize)) {
            //not_using_share
            if(tableGhGt.Shared == 0) {
                 resPredict = tableGhGt.fsmTable[binToDec(tableGhGt.globalHistory)];
            }
            //using_share_lsb
            else if(tableGhGt.Shared == 1){
                 indexShared = decimalXor(binToDec(tableGhGt.globalHistory), sharedLsb(pc,tableGhGt.globalHistory.size()));
                 resPredict = tableGhGt.fsmTable[indexShared];
            }
            else { //using_share_mid
                indexShared = decimalXor(binToDec(tableGhGt.globalHistory), sharedMid(pc,tableGhGt.globalHistory.size()));
                resPredict = tableGhGt.fsmTable[indexShared];
            }
            if (resPredict == 2 || resPredict == 3) {
                *dst = tableGhGt.rows[indexInBtbDec].target;
                return true;
            }
            else {
                *dst = pc + 4;
                return false;
            }
        }
         else{
                *dst = pc + 4;
                return false;
         }
    }

    if (tableLhGt.currentMachine) {
        btbSize = tableLhGt.rows.size();

        indexInBtbDec = indexInBtb(pc, btbSize);

        if (tableLhGt.rows[indexInBtbDec].valid &&
            tableLhGt.rows[indexInBtbDec].tag == tagFromPc(pc,btbSize,tableLhGt.tagSize)) {

            //not_using_share
            if(tableGhGt.Shared == 0) {
                resPredict = tableLhGt.fsmTable[binToDec(tableLhGt.rows[indexInBtbDec].history)];
            }
            //using_share_lsb
            else if(tableGhGt.Shared == 1){
                indexShared = decimalXor(binToDec(tableLhGt.rows[indexInBtbDec].history),
                                         sharedLsb(pc,tableLhGt.rows[indexInBtbDec].history.size()));
                resPredict = tableGhGt.fsmTable[indexShared];
            }
            else { //using_share_mid
                indexShared = decimalXor(binToDec(tableLhGt.rows[indexInBtbDec].history),
                                         sharedMid(pc,tableLhGt.rows[indexInBtbDec].history.size()));
                resPredict = tableGhGt.fsmTable[indexShared];
            }

            if (resPredict == 2 || resPredict == 3) {
                *dst = tableLhGt.rows[indexInBtbDec].target;
                return true;
            }
            else {
                *dst = pc + 4;
                return false;
            }
        }
        else {
            *dst = pc + 4;
            return false;
        }
    }

    if (tableLhLt.currentMachine) {
        btbSize = tableLhLt.rows.size();

        indexInBtbDec = indexInBtb(pc, btbSize);

        if (tableLhLt.rows[indexInBtbDec].valid &&
            tableLhLt.rows[indexInBtbDec].tag == tagFromPc(pc,btbSize,tableLhLt.tagSize)) {
            resPredict = tableLhLt.rows[indexInBtbDec].fsmTable[binToDec(tableLhLt.rows[indexInBtbDec].history)];
            if (resPredict == 2 || resPredict == 3) {
                *dst = tableLhLt.rows[indexInBtbDec].target;
                return true;
            }
            else {
                *dst = pc + 4;
                return false;
            }
        }
        else {
            *dst = pc + 4;
            return false;
        }
    }
    if (tableGhLT.currentMachine) {
        btbSize = tableGhLT.rows.size();

        indexInBtbDec = indexInBtb(pc, btbSize);

        if (tableGhLT.rows[indexInBtbDec].valid &&
                tableGhLT.rows[indexInBtbDec].tag == tagFromPc(pc,btbSize,tableGhLT.tagSize))
        {
                resPredict = tableGhLT.rows[indexInBtbDec].fsmTable[binToDec(tableGhLT.globalHistory)];

            if (resPredict == 2 || resPredict == 3) {
                *dst = tableGhLT.rows[indexInBtbDec].target;
                return true;
            }
            else {
                *dst = pc + 4;
                return false;
            }
        }
        else{
            *dst = pc + 4;
            return false;
        }
    }
}

void BP_update(uint32_t pc, uint32_t targetPc, bool taken, uint32_t pred_dst){
    int btbSize = 0;
    stats.br_num++;

    if (tableGhGt.currentMachine) {
        btbSize = tableGhGt.rows.size();
        bool isTagRight = (tableGhGt.rows[indexInBtb(pc,btbSize)].tag == tagFromPc(pc,btbSize,tableGhGt.tagSize));
        if(isTagRight) {
            if (targetPc == pred_dst && taken) {  // the predict is right
                if (tableGhGt.fsmTable[binToDec(tableGhGt.globalHistory)] != 3) {
                    tableGhGt.fsmTable[binToDec(tableGhGt.globalHistory)] += 1;
                    //shift(tableGhGt.globalHistory, taken);
                }
            }  if (targetPc != pred_dst && taken) { // the predict is wrong
                stats.flush_num += 1;
                if (tableGhGt.fsmTable[binToDec(tableGhGt.globalHistory)] != 3)
                    tableGhGt.fsmTable[binToDec(tableGhGt.globalHistory)] += 1;
            } else if (targetPc != pred_dst && (!taken)) { // the predict is right
                if (tableGhGt.fsmTable[binToDec(tableGhGt.globalHistory)] != 0) {
                    tableGhGt.fsmTable[binToDec(tableGhGt.globalHistory)] -= 1;
                }
            }
            else if (targetPc == pred_dst && (!taken)) { // the predict is wrong
                stats.flush_num += 1;
                if (tableGhGt.fsmTable[binToDec(tableGhGt.globalHistory)] != 0) {
                    tableGhGt.fsmTable[binToDec(tableGhGt.globalHistory)] -= 1;
                }
            }
        }
        else {
            tableGhGt.rows[indexInBtb(pc,btbSize)].tag = tagFromPc(pc,btbSize,tableGhGt.tagSize);
            tableGhGt.rows[indexInBtb(pc,btbSize)].target = targetPc;
            tableGhGt.rows[indexInBtb(pc,btbSize)].valid = true;
            if (taken){
                stats.flush_num += 1;
            }
        }
        shift(tableGhGt.globalHistory,taken);
    }

        if (tableLhGt.currentMachine) {
            btbSize = tableLhGt.rows.size();
            vector<bool> newHistory = vector<bool> (tableLhGt.rows[0].history.size(),false);
            newHistory[0] = taken;

            if(tableLhGt.rows[indexInBtb(pc,btbSize)].tag == tagFromPc(pc,btbSize,tableLhGt.tagSize)) {
                if (targetPc == pred_dst && taken) {  // the predict is right
                    if (tableLhGt.fsmTable[binToDec(tableLhGt.rows[indexInBtb(pc,btbSize)].history)] != 3)
                        tableLhGt.fsmTable[binToDec(tableLhGt.rows[indexInBtb(pc,btbSize)].history)] += 1;
                } else if (targetPc != pred_dst && taken) { // the predict is wrong
                    stats.flush_num += 1;
                    if (tableLhGt.fsmTable[binToDec(tableLhGt.rows[indexInBtb(pc,btbSize)].history)] != 3)
                        tableLhGt.fsmTable[binToDec(tableLhGt.rows[indexInBtb(pc,btbSize)].history)] += 1;
                } else if (targetPc != pred_dst && !taken) { // the predict is right
                    if (tableLhGt.fsmTable[binToDec(tableLhGt.rows[indexInBtb(pc,btbSize)].history)] != 0)
                        tableLhGt.fsmTable[binToDec(tableLhGt.rows[indexInBtb(pc,btbSize)].history)] -= 1;
                }
                else if (targetPc != pred_dst && !taken) { // the predict is wrong
                    stats.flush_num += 1;
                    if (tableLhGt.fsmTable[binToDec(tableLhGt.rows[indexInBtb(pc,btbSize)].history)] != 0)
                        tableLhGt.fsmTable[binToDec(tableLhGt.rows[indexInBtb(pc,btbSize)].history)] -= 1;
                }
                shift(tableLhGt.rows[indexInBtb(pc,btbSize)].history,taken);
            }

            else {
                tableLhGt.rows[indexInBtb(pc,btbSize)].tag = tagFromPc(pc,btbSize,tableLhGt.tagSize);
                tableLhGt.rows[indexInBtb(pc,btbSize)].target = targetPc;
                tableLhGt.rows[indexInBtb(pc,btbSize)].valid = true;
                tableLhGt.rows[indexInBtb(pc,btbSize)].history = newHistory;
                if (taken){
                    stats.flush_num += 1;
                }
            }
        }

        if (tableLhLt.currentMachine) {
            btbSize = tableLhLt.rows.size();
            vector<bool> newHistory = vector<bool> (tableLhLt.rows[0].history.size(),false);
            newHistory[0] = taken;

            if(tableLhLt.rows[indexInBtb(pc,btbSize)].tag == tagFromPc(pc,btbSize,tableLhLt.tagSize)) {
                if (targetPc == pred_dst && taken) {  // the predict is right
                    if (tableLhLt.rows[indexInBtb(pc,btbSize)].fsmTable[binToDec(tableLhLt.rows[indexInBtb(pc,btbSize)].history)] != 3)
                        tableLhLt.rows[indexInBtb(pc,btbSize)].fsmTable[binToDec(tableLhLt.rows[indexInBtb(pc,btbSize)].history)] += 1;
                } else if (targetPc != pred_dst && taken) { // the predict is wrong
                    stats.flush_num += 1;
                    if (tableLhLt.rows[indexInBtb(pc,btbSize)].fsmTable[binToDec(tableLhLt.rows[indexInBtb(pc,btbSize)].history)] != 3)
                        tableLhLt.rows[indexInBtb(pc,btbSize)].fsmTable[binToDec(tableLhLt.rows[indexInBtb(pc,btbSize)].history)] += 1;
                } else if (targetPc != pred_dst && !taken) { // the predict is right
                    if (tableLhLt.rows[indexInBtb(pc,btbSize)].fsmTable[binToDec(tableLhLt.rows[indexInBtb(pc,btbSize)].history)] != 0)
                        tableLhLt.rows[indexInBtb(pc,btbSize)].fsmTable[binToDec(tableLhLt.rows[indexInBtb(pc,btbSize)].history)] -= 1;
                } else if (targetPc != pred_dst && !taken) { // the predict is wrong
                    stats.flush_num++;
                    if (tableLhLt.rows[indexInBtb(pc,btbSize)].fsmTable[binToDec(tableLhLt.rows[indexInBtb(pc,btbSize)].history)] != 0)
                        tableLhLt.rows[indexInBtb(pc,btbSize)].fsmTable[binToDec(tableLhLt.rows[indexInBtb(pc,btbSize)].history)] -= 1;
                }
                shift(tableLhLt.rows[indexInBtb(pc,btbSize)].history,taken);

            }
            else {
                tableLhLt.rows[indexInBtb(pc,btbSize)].tag = tagFromPc(pc,btbSize,tableLhLt.tagSize);
                tableLhLt.rows[indexInBtb(pc,btbSize)].target = targetPc;
                tableLhLt.rows[indexInBtb(pc,btbSize)].valid = true;
                tableLhLt.rows[indexInBtb(pc,btbSize)].history = newHistory;
                tableLhLt.rows[indexInBtb(pc,btbSize)].fsmTable = vector<unsigned> (tableLhLt.rows[indexInBtb(pc,btbSize)].fsmTable.size(),tableLhLt.fsmState);
                if (taken){
                    stats.flush_num += 1;
                }
            }
        }
    if (tableGhLT.currentMachine) {
        btbSize = tableGhLT.rows.size();
        bool isTagRight = (tableGhLT.rows[indexInBtb(pc,btbSize)].tag == tagFromPc(pc,btbSize,tableGhLT.tagSize));
        if(isTagRight) {
            if (targetPc == pred_dst && taken) {  // the predict is right

                if (tableGhLT.rows[indexInBtb(pc,btbSize)].fsmTable[binToDec(tableGhLT.globalHistory)] != 3) {
                    tableGhLT.rows[indexInBtb(pc,btbSize)].fsmTable[binToDec(tableGhLT.globalHistory)] += 1;
                    //shift(tableGhGt.globalHistory, taken);
                }
            }  if (targetPc != pred_dst && taken) { // the predict is wrong
                stats.flush_num += 1;
                if (tableGhLT.rows[indexInBtb(pc,btbSize)].fsmTable[binToDec(tableGhLT.globalHistory)] != 3)
                    tableGhLT.rows[indexInBtb(pc,btbSize)].fsmTable[binToDec(tableGhLT.globalHistory)] += 1;
            } else if (targetPc != pred_dst && (!taken)) { // the predict is right
                if (tableGhLT.rows[indexInBtb(pc,btbSize)].fsmTable[binToDec(tableGhLT.globalHistory)] != 0) {
                    tableGhLT.rows[indexInBtb(pc,btbSize)].fsmTable[binToDec(tableGhLT.globalHistory)] -= 1;
                }
            }
            else if (targetPc == pred_dst && (!taken)) { // the predict is wrong
                stats.flush_num += 1;
                if (tableGhLT.rows[indexInBtb(pc,btbSize)].fsmTable[binToDec(tableGhLT.globalHistory)] != 0) {
                    tableGhLT.rows[indexInBtb(pc,btbSize)].fsmTable[binToDec(tableGhLT.globalHistory)] -= 1;
                }
            }
        }
        else {
            tableGhLT.rows[indexInBtb(pc,btbSize)].tag = tagFromPc(pc,btbSize,tableGhLT.tagSize);
            tableGhLT.rows[indexInBtb(pc,btbSize)].target = targetPc;
            tableGhLT.rows[indexInBtb(pc,btbSize)].valid = true;
            if (taken){
                stats.flush_num += 1;
            }
        }
        shift(tableGhLT.globalHistory,taken);
    }
	return;
}

void BP_GetStats(SIM_stats *curStats){
    curStats->flush_num = stats.flush_num;
    curStats->br_num = stats.br_num;
    if (tableGhGt.currentMachine) {
        curStats->size = tableGhGt.rows.size()*(tableGhGt.tagSize+1+30)+tableGhGt.globalHistory.size()+
                         pow(2,tableGhGt.globalHistory.size())*2;
    }
    if (tableLhGt.currentMachine) {
        curStats->size = tableLhGt.rows.size()*(tableLhGt.tagSize+tableLhGt.rows[0].history.size()+30+1)+
                pow(2,tableLhGt.rows[0].history.size())*2;
    }
    if (tableLhLt.currentMachine) {
        curStats->size = tableLhLt.rows.size()*(tableLhLt.tagSize+tableLhLt.rows[0].history.size()+30+1+
                         pow(2,tableLhLt.rows[0].history.size())*2);
    }
    if (tableGhLT.currentMachine) {
        curStats->size = tableGhLT.rows.size()*(tableGhLT.tagSize+1+30+pow(2,tableGhLT.globalHistory.size())*2)
                +tableGhLT.globalHistory.size();

    }
	return;
}

vector<bool> decToBin(uint32_t number) {
    int i = 0;
    vector<bool> binNumber;
    while (number != 0 ) {
        binNumber.push_back(bool(number % 2));
        i++;
        number = number/2;
    }
    return binNumber;
}

int binToDec (vector<bool> pcBits) {
    int res= 0;
    for (int i = 0; i < pcBits.size(); i++) {
        if(pcBits[i] == true)
        {
            res+= pow(2,i);
        }
    }
    return res;
}

int decimalXor(int x, int y) {
    return (x | y) & (~x |~y);
}

int indexInBtb(uint32_t pc, int btbSize) {
    vector<bool> pcBits = decToBin(pc);
    vector<bool> indexInBtb;
    int indexInBtbDec = 0;

    for (int i = 2; i < log2(btbSize)+2; i++){
        indexInBtb.push_back(pcBits[i]);
    }
    indexInBtbDec = binToDec(indexInBtb);
    return indexInBtbDec;
}

int tagFromPc(uint32_t pc, int btbSize, unsigned tagSize) {
    vector<bool> tabBin;
    vector<bool> pcBits = decToBin(pc);
    int tag = 0;

    for (int i = pcBits.size(); i < 32; i++) {
        pcBits.push_back(false);
    }

    for (int i = log2(btbSize)+2; i < log2(btbSize)+2+tagSize; i++){
        tabBin.push_back(pcBits[i]);
    }
    tag = binToDec(tabBin);
    return tag;
}

int sharedLsb(uint32_t pc, int historySize) {
    vector<bool> pcBits = decToBin(pc);
    vector<bool> lsbBitsSharedBin;
    int lsbBitsSharedDec = 0;

    for (int i = 2; i < historySize+2; i++){
        lsbBitsSharedBin.push_back(pcBits[i]);
    }
    lsbBitsSharedDec = binToDec(lsbBitsSharedBin);
    return lsbBitsSharedDec;
}

int sharedMid(uint32_t pc, int historySize) {
    vector<bool> pcBits = decToBin(pc);
    vector<bool> midBitsSharedBin;
    int midBitsSharedDec = 0;

    for (int i = 16; i < historySize+16; i++){
        midBitsSharedBin.push_back(pcBits[i]);
    }
    midBitsSharedDec = binToDec(midBitsSharedBin);
    return midBitsSharedDec;
}

void shift(vector<bool> &vec, bool taken) {
    for (int i = 0; i < vec.size()-1; i++) {
        vec[i+1] = vec[i];
    }
    vec[0] = taken;
}
