#pragma once

#include <algorithm>
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
        missed_tasks = 0;
        completed = 0;
    }
    void simulate();
    double get_qos() {
        return qos;
    }
    void set_waiting( std::vector<Task *> pending )
    {
        this->waiting = pending;
    }
    void set_finish_time( double time )
    {
        this->finish_time = time;
    }

private:
    double abs_time;
    double finish_time;
    EDL *edl;
    std::vector<Task *> red_ready;
    std::vector<Task *> blue_ready;
    std::vector<Task *> waiting;
    double time_slice;
    Task *running;
    int missed_tasks;
    int completed;
    double qos;
    void algorithm( double current_time );
    static void break_dd_tie( std::vector<Task *> tasks );
    void compute_qos();
};