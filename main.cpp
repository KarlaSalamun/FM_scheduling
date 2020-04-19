//
// Created by karla on 13. 04. 2020..
//
#include <BWP.h>
#include "RTO.h"
#include "EDL.h"
#include "Simulator.tpp"

void print_vector( std::vector<double> vector );

int main()
{
    UunifastCreator *tc = new UunifastCreator( 3, "./../../test_inputs/edl.txt", true, 5, 1, 1, 1.2 );
    tc->set_time_slice(1);
    Scheduler *sched = new Scheduler();
    Simulator<Dummy *> *simulator = new Simulator<Dummy *>( 0.5, 0, tc, sched, false );
    simulator->set_heuristic(nullptr );
    simulator->load();
    EDL *edl = new EDL( simulator );
//    tc->load_tasks( pending_tasks );
    edl->compute_schedule();
    print_vector( edl->get_deadline_vector() );
    print_vector( edl->get_idle_time_vector() );
    print_vector( edl->get_EDL_deadline_vector() );
    print_vector( edl->get_EDL_idle_time_vector() );

//    RTO *alg = new RTO( 4, tc, sched, 72 );
//    alg->load();
//    printf( "utilization: %f\n",  alg->compute_eq_utilization() );
//    alg->simulate( 1 );
    return 0;
}

void print_vector( std::vector<double> vector)
{
    for( auto & element : vector ) {
        printf( "%f\n", element );
    }
    printf("\n\n");
}