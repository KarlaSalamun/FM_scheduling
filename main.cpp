//
// Created by karla on 13. 04. 2020..
//
#include <BWP.h>
#include "RTO.h"
#include "EDL.h"
#include "RLP.h"
#include "Simulator.tpp"

void print_vector( std::vector<double> vector );
void generate_csv(std::vector<double> results, std::vector<double> utils, std::string filename );


int main()
{
    double overload;
    std::vector<double> qos_rto;
    std::vector<double> qos_bwp;
    std::vector<double> qos_rlp;
    std::vector<double> overload_values;
    std::vector<Task *> pending;
    UunifastCreator *tc = new UunifastCreator( 4, "./../../test_inputs/rto.txt", true, 100, 10, 10, 1 );
    tc->set_time_slice( 0.01 );

//    tc->create_test_set( pending );
//    tc->write_tasks( pending );

    tc->load_tasks( pending );
    for( auto & element : pending ) {
        element->set_skip_factor( 2 );
    }
    std::vector<double> durations;
    for( auto & element : pending ) {
        durations.push_back( element->get_duration() );
        assert( element->get_duration() != 0 );
    }

    Scheduler *sched = new Scheduler();

    RTO *rto = new RTO( tc, 4, sched, 72 );
    BWP *bwp = new BWP( 4, tc, sched, 72 );
    EDL *edl = new EDL( rto );
    RLP *rlp = new RLP( edl, pending, 0.01 );
    tc->compute_hyperperiod( pending );
    rto->set_finish_time( tc->get_hyperperiod() );
    bwp->set_finish_time( tc->get_hyperperiod() );

    for( overload = 0.9; overload <= 1.65; overload = overload + 0.05 ) {
        overload_values.push_back( overload );
        double util = 0;
        tc->set_overload( overload );
        tc->compute_overloaded( pending, durations );
        for( auto & element : pending ) {
            element->initialize_task();
            util += element->get_duration() / element->get_period();
        }
        rto->set_finish_time( tc->get_hyperperiod() );
        rto->set_pending( pending );
        rto->simulate( 0.01 );
        qos_rto.push_back( rto->get_qos() );

        bwp->set_finish_time( tc->get_hyperperiod() );
        bwp->pending = pending;
        bwp->simulate( 0.01 );
        qos_bwp.push_back( bwp->get_qos() );

        rlp->set_waiting( pending );
        rlp->set_finish_time( tc->get_hyperperiod() );
        rlp->simulate();
        qos_rlp.push_back( rlp->get_qos() );
    }

    generate_csv( qos_rto, overload_values,"rto.csv" );
    generate_csv( qos_bwp, overload_values,"bwp.csv" );
    generate_csv( qos_rlp, overload_values,"rlp.csv" );

    return 0;
}

void print_vector( std::vector<double> vector)
{
    for( auto & element : vector ) {
        printf( "%f\n", element );
    }
    printf("\n\n");
}

void generate_csv(std::vector<double> results, std::vector<double> utils, std::string filename )
{
    std::string tmp = "./../../test_outputs/";
    FILE *fd = fopen( (tmp + filename).c_str(), "w+" );
    for( size_t i=0; i<results.size(); i++ ) {
        fprintf( fd, "%lf,%lf\n", utils[i], results[i] );
    }
    fclose( fd );
}
