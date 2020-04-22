//
// Created by karla on 18. 04. 2020..
//
#include "EDL.h"
#include "Simulator.tpp"

void EDL::compute_schedule( std::vector<Task *> tasks, Task *running, double current_time )
{
    std::vector<Task *> tmp;
    tmp = rto->get_pending();
    compute_hyperperiod( tmp );     // TODO sto s doubleovima
//    hyperperiod = 18;
    rto->set_finish_time(static_cast<double> (hyperperiod) );
    if( running ) {
        tasks.push_back( running );
    }
    rto->set_pending( tasks );
    rto->set_abs_time( current_time );
    rto->simulate( 1 );
    deadline_vector = rto->get_deadline_vector();
    idle_time_vector = rto->get_idle_time_vector();
    set_EDL_idle_time_vector();
    compute_EDL_deadline_vector();
}

void EDL::compute_hyperperiod(std::vector<Task *> &tasks)
{
    std::vector<int> periods;
    for( auto & element : tasks ) {
        periods.push_back( static_cast<int>( element->get_period() ) );
    }
    hyperperiod = std::accumulate(periods.begin(), periods.end(), 1,
            [](int a, int b) -> int {
                return abs( a * b) / std::__gcd(a, b);
    });
}

void EDL::compute_EDL_deadline_vector()
{
    EDL_deadline_vector.resize( deadline_vector.size() );
    for( size_t i=0; i<deadline_vector.size(); i++ ) {
        EDL_deadline_vector[i] = hyperperiod -
                deadline_vector[deadline_vector.size() - i - 1] - EDL_idle_time_vector[i];
    }
}

void EDL::update_schedule( double curr_time )
{
    for( size_t i=0; i<EDL_deadline_vector.size(); i++ ) {
        EDL_deadline_vector[i] = EDL_deadline_vector[i] + curr_time;
    }
}

bool EDL::compute_availability( double time )
{
    double idle_interval = 0;
    int index = 0;
    for( size_t i=0; i<EDL_deadline_vector.size(); i++ ) {
        if( time >= EDL_deadline_vector[i] ) {
            idle_interval = EDL_deadline_vector[i];
            index = i;
            if( islessequal( time, idle_interval + EDL_idle_time_vector[index] ) ) {
                return true;
            }
        }
    }
    return false;
}