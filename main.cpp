//
// Created by karla on 13. 04. 2020..
//
#include <BWP.h>
#include "RTO.h"
#include "EDL.h"

int main()
{
    std::vector<Task *> pending_tasks;
    UunifastCreator *tc = new UunifastCreator( 4, "./../../test_inputs/rto.txt", true, 5, 1, 1, 1.2 );
    tc->set_time_slice(1);
    Scheduler *sched = new Scheduler();
    Simulator<Dummy *> *simulator = new Simulator<Dummy *>( 0.5, 0, tc, sched, false );
    EDL *edl = new EDL( simulator );
    edl->compute_schedule( pending_tasks );
//    RTO *alg = new RTO( 4, tc, sched, 72 );
//    alg->load();
    tc->load_tasks( pending_tasks );
//    printf( "utilization: %f\n",  alg->compute_eq_utilization() );
//    alg->simulate( 1 );
    return 0;
}
