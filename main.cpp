//
// Created by karla on 13. 04. 2020..
//
#include <BWP.h>
#include "RTO.h"
#include "EDL.h"
#include "RLP.h"
#include "Simulator.tpp"

void print_vector( std::vector<double> vector );
void test_utils_qos();
void generate_csv(std::vector<double> results, std::vector<double> utils, std::string filename );
void test_utils_wCPU();

int main()
{
    std::vector<Task *> pending;
    UunifastCreator *tc = new UunifastCreator( 3, "./../../test_inputs/95.txt", true, 100, 10, 10, 1 );
    tc->set_time_slice( 1 );

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

    RTO *rto = new RTO( tc, 5, sched, 72, false );
    rto->set_pending( pending );
    rto->compute_eq_utilization();
    EDL *edl = new EDL( rto );
    RLP *rlp = new RLP( edl, pending, 1 );
    tc->compute_hyperperiod( pending );
    rto->set_finish_time( tc->get_hyperperiod() );

    rlp->set_waiting( pending );
    std::vector<double> K_vector;
    std::vector<double> D_vector;
    edl->compute_static( pending );
    rlp->set_finish_time( tc->get_hyperperiod() );
    rlp->simulate();

//    printf( "%lf %lf %lf\n", rlp->get_qos(), rlp->compute_mean_skip_factor(), rlp->compute_gini_coeff() );

    test_utils_qos();

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

void test_utils_qos()
{
    UunifastCreator *tc = new UunifastCreator( 6, "./../../test_inputs/test_1.txt", true, 20, 4, 2, 1 );
    Scheduler *sched = new Scheduler();

    RTO *rto = new RTO( tc, 6, sched, 72, false );
    EDL *edl = new EDL( rto );
    RLP *rlp = new RLP( edl, 1 );
    BWP *bwp = new BWP( 6, tc, sched, 72, false );

    std::vector<double> utils;
    std::vector<double> results;

    std::vector<Task *> test_tasks;

    for( size_t i=1; i<=14; i++ ) {
        utils.push_back( 0.90 + i * 0.05 );
    }

    std::vector<double> mean_qos_rto;
    std::vector<double> mean_qos_bwp;
    std::vector<double> mean_qos_rlp;

    std::vector<double> actual_utils;

    for( size_t i=0; i<utils.size(); i++ ) {
        tc->set_overload(utils[i]);
        tc->set_task_number(6);
        for (size_t j = 0; j < 100; j++) {
            do {
                tc->create_test_set(test_tasks);
                tc->compute_hyperperiod( test_tasks );
            } while( tc->get_hyperperiod() > 10000 );

            double tmp_util = 0;
            for( auto & element : test_tasks ) {
                element->set_skip_factor( 2 );
                tmp_util += static_cast<double> (element->get_duration()) / static_cast<double> (element->get_period());
            }
            actual_utils.push_back( tmp_util );

            rto->set_finish_time(tc->get_hyperperiod());
            bwp->set_finish_time( tc->get_hyperperiod() );
            rlp->set_waiting( test_tasks );
            edl->set_tasks( test_tasks );
            rto->set_pending( test_tasks );
            bwp->set_pending( test_tasks );
            bwp->simulate( 1 );
            mean_qos_bwp.push_back( bwp->get_qos() );
            rlp->set_finish_time( tc->get_hyperperiod() );
            rlp->simulate();
            assert( rlp->get_wasted_time() <= tc->get_hyperperiod() );
//            mean_qos_rlp.push_back( rlp->get_wasted_time() / tc->get_hyperperiod() );
            mean_qos_rlp.push_back( rlp->get_qos() );
            rto->set_abs_time(0);
            rto->set_pending( test_tasks );
            rto->set_finish_time( tc->get_hyperperiod() );
            rto->simulate( 1 );
            mean_qos_rto.push_back( rto->get_qos() );
//            assert( rlp->get_qos() >= bwp->get_qos() );
        }
    }
    generate_csv( mean_qos_bwp, actual_utils, "bwp_qos1.csv" );
    generate_csv( mean_qos_rlp, actual_utils, "rlp_qos1.csv" );
    generate_csv( mean_qos_rto, actual_utils, "rto_qos1.csv" );
}

void test_utils_wCPU()
{
    UunifastCreator *tc = new UunifastCreator( 2, "./../../test_inputs/test_1.txt", true, 20, 4, 2, 1 );
    Scheduler *sched = new Scheduler();

    RTO *rto = new RTO( tc, 2, sched, 72, false );
    EDL *edl = new EDL( rto );
    RLP *rlp = new RLP( edl, 1 );

    std::vector<double> utils;
    std::vector<double> results;

    std::vector<Task *> test_tasks;

    for( size_t i=0; i<=14; i++ ) {
        utils.push_back( 0.90 + i * 0.05 );
    }
    std::vector<double> mean_qos_rlp;

    std::vector<double> actual_utils;

    for( size_t i=0; i<utils.size(); i++ ) {
        tc->set_overload(utils[i]);
        tc->set_task_number(6);
        for (size_t j = 0; j < 100; j++) {
            do {
                tc->create_test_set(test_tasks);
                tc->compute_hyperperiod(test_tasks);
            } while (tc->get_hyperperiod() > 10000);

            double tmp_util = 0;
            for (auto &element : test_tasks) {
                element->set_skip_factor(2);
                element->initialize_task();
                tmp_util += static_cast<double> (element->get_duration()) / static_cast<double> (element->get_period());
            }
            actual_utils.push_back(tmp_util);

            rto->set_finish_time(tc->get_hyperperiod());
            rlp->set_waiting(test_tasks);
            edl->set_tasks(test_tasks);
            rto->set_pending(test_tasks);
            rlp->set_finish_time(tc->get_hyperperiod());
            rlp->simulate();
            mean_qos_rlp.push_back(rlp->get_wasted_time());
        }
    }

    generate_csv( mean_qos_rlp, actual_utils, "rlp_wCPU.csv" );
}