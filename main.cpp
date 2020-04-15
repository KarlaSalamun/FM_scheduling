//
// Created by karla on 13. 04. 2020..
//
#include "RTO.h"

int main()
{
    UunifastCreator *tc = new UunifastCreator( 3, "./../../test_inputs/rto.txt", true, 5, 1, 1, 1.2 );
    tc->set_time_slice(1);
    Scheduler *sched = new Scheduler();
    RTO *rto = new RTO( 3, tc, sched );
    rto->load();
    rto->simulate( 27, 1 );
    return 0;
}
