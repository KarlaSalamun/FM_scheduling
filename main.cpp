//
// Created by karla on 13. 04. 2020..
//
#include <BWP.h>
#include "RTO.h"

int main()
{
    UunifastCreator *tc = new UunifastCreator( 4, "./../../test_inputs/rto.txt", true, 5, 1, 1, 1.2 );
    tc->set_time_slice(1);
    Scheduler *sched = new Scheduler();
    BWP *bwp = new BWP( 4, tc, sched );
    bwp->load();
    bwp->simulate( 72, 1 );
    return 0;
}
