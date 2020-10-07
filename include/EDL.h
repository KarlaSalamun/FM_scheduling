//
// Created by karla on 18. 04. 2020..
//

#ifndef FM_SCHEDULING_EDL_H
#define FM_SCHEDULING_EDL_H

#include <Simulator.h>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <utility>
#include "RTO.h"

class EDL {
public:
    EDL( RTO *rto ) : rto( rto ) {}
    void compute_schedule( const std::vector<Task *> tasks, const Task *running, double current_time );
    std::vector<uint32_t> get_deadline_vector()
    {
        return deadline_vector;
    }
    std::vector<uint32_t > get_idle_time_vector()
    {
        return idle_time_vector;
    }
    std::vector<uint32_t> get_EDL_deadline_vector()
    {
        return EDL_deadline_vector;
    }
    std::vector<uint32_t> get_EDL_idle_time_vector()
    {
        return EDL_idle_time_vector;
    }
    void set_EDL_idle_time_vector( std::vector<uint32_t> vector ) {
        this->EDL_idle_time_vector = vector;
    }
    void set_EDL_deadline_vector( std::vector<uint32_t> vector ) {
        this->EDL_deadline_vector = vector;
    }
    void set_tasks( std::vector<Task *> tasks ) {
        this->rto->set_pending( tasks );
    }
    void update_EDL_idle_time_vector() {
        EDL_idle_time_vector.clear();
        std::copy(idle_time_vector.begin(), idle_time_vector.end(), std::back_inserter( EDL_idle_time_vector ));
        std::reverse( EDL_idle_time_vector.begin(), EDL_idle_time_vector.end() );
    }
    void compute_EDL_deadline_vector();
    void update_schedule( double curr_time );
    bool compute_availability( double time );
    bool dynamic_sched( const std::vector<Task *> tasks, const Task *running, int current_time );
    void compute_static( std::vector<Task *> tasks );
    void compute_dynamic( uint32_t time, const std::vector<Task *> &ready_tasks, Task *running );

private:
    double hyperperiod;
    std::vector<uint32_t> deadline_vector;
    std::vector<uint32_t> EDL_deadline_vector;
    std::vector<uint32_t> EDL_dynamic_deadline_vector;
    std::vector<uint32_t> idle_time_vector;
    std::vector<uint32_t> EDL_idle_time_vector;
    std::vector<uint32_t> EDL_dynamic_idle_time_vector;
    void compute_hyperperiod(std::vector<Task *> &tasks);
    RTO *rto;
};

#endif //FM_SCHEDULING_EDL_H
