#pragma once

#include "Task.h"
#include "EDL.h"

class RLP {
public:

    RLP( EDL *edl, std::vector<Task *> waiting, double time_slice ) {
        this->edl = edl;
        this->waiting = waiting;
        this->time_slice = time_slice;
        abs_time = 0;
        running = nullptr;
    }
    void simulate( double finish_time );
private:
    double abs_time;
    EDL *edl;
    std::vector<Task *> red_ready;
    std::vector<Task *> blue_ready;
    std::vector<Task *> waiting;
    double time_slice;
    Task *running;
    void algorithm( double current_time );
};