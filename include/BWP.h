//
// Created by karla on 15. 04. 2020..
//

#ifndef FM_SCHEDULING_BWP_H
#define FM_SCHEDULING_BWP_H

#include "RTO.h"

class BWP : public RTO {
public:
    BWP( int task_number, UunifastCreator *creator, Scheduler *sched ) : RTO( task_number, creator, sched ) {}
    virtual void simulate( double finish_time, double time_slice );
};

#endif //FM_SCHEDULING_BWP_H
