//
// Created by karla on 18. 04. 2020..
//

#ifndef FM_SCHEDULING_EDL_H
#define FM_SCHEDULING_EDL_H

#include <Simulator.h>
#include <algorithm>
#include <numeric>
#include <cmath>

class EDL {
public:
    EDL( Simulator<Dummy *> *simulator ) : simulator( simulator ) {}
    void compute_schedule( std::vector<Task *> &pending );
    std::vector<double> get_deadline_vector()
    {
        return deadline_vector;
    }
    std::vector<double> get_idle_time_vector()

    {
        return idle_time_vector;
    }
    void set_EDF_idle_time_vector( std::vector<double> vector ){
        idle_time_vector = vector;
        std::reverse( idle_time_vector.begin(), idle_time_vector.end() );
    }
    std::vector<double> compute_EDL_deadline_vector( std::vector<double> init_deadline_vector );
    std::vector<double> update_schedule( std::vector<double> init_deadline_vector, double curr_time );

private:
    double hyperperiod;
    std::vector<double> deadline_vector;
    std::vector<double> idle_time_vector;
    void compute_hyperperiod(std::vector<Task *> &tasks);
    Simulator<Dummy *> *simulator;
    Scheduler *sched;
};

#endif //FM_SCHEDULING_EDL_H
