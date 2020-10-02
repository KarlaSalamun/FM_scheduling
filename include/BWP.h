//
// Created by karla on 15. 04. 2020..
//

#ifndef FM_SCHEDULING_BWP_H
#define FM_SCHEDULING_BWP_H

#include "RTO.h"

class BWP : public RTO {
public:
    BWP( int task_number, UunifastCreator *creator, Scheduler *sched, double finish_time, bool display_sched ) :
        RTO( creator, task_number, sched, finish_time, display_sched) {}
    virtual void simulate( double time_slice );
};

#endif //FM_SCHEDULING_BWP_H
