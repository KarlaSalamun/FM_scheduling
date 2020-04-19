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

class EDL {
public:
    EDL( Simulator<Dummy *> *simulator ) : simulator( simulator ) {}
    void compute_schedule();
    std::vector<double> get_deadline_vector()
    {
        return deadline_vector;
    }
    std::vector<double> get_idle_time_vector()
    {
        return idle_time_vector;
    }
    std::vector<double> get_EDL_deadline_vector()
    {
        return EDL_deadline_vector;
    }
    std::vector<double> get_EDL_idle_time_vector()
    {
        return EDL_idle_time_vector;
    }
    void set_EDL_idle_time_vector() {
        std::copy(idle_time_vector.begin(), idle_time_vector.end(), std::back_inserter( EDL_idle_time_vector ));
        std::reverse( EDL_idle_time_vector.begin(), EDL_idle_time_vector.end() );
    }
    void compute_EDL_deadline_vector();
    std::vector<double> update_schedule( std::vector<double> init_deadline_vector, double curr_time );

private:
    double hyperperiod;
    std::vector<double> deadline_vector;
    std::vector<double> EDL_deadline_vector;
    std::vector<double> idle_time_vector;
    std::vector<double> EDL_idle_time_vector;
    void compute_hyperperiod(std::vector<Task *> &tasks);
    Simulator<Dummy *> *simulator;
};

#endif //FM_SCHEDULING_EDL_H
