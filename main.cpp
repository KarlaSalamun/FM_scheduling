//
// Created by karla on 13. 04. 2020..
//
#include <BWP.h>
#include "RTO.h"
#include "EDL.h"
#include "RLP.h"
#include "Simulator.tpp"

void print_vector( std::vector<double> vector );

int main()
{
    std::vector<Task *> pending;
    UunifastCreator *tc = new UunifastCreator( 4, "./../../test_inputs/rto.txt", true, 5, 1, 1, 1.2 );
    tc->set_time_slice(1);
    Scheduler *sched = new Scheduler();
    RTO *rto = new RTO( 4, tc, sched, 72 );
    tc->load_tasks( pending );
    rto->pending = pending;         // TODO napraviti ovo pametnije i staviti pending u private
    EDL *edl = new EDL( rto );
    RLP *rlp = new RLP( edl, pending, 1 );
    rlp->simulate( 20 );

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