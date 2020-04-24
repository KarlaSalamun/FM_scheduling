//
// Created by karla on 18. 04. 2020..
//
#include "EDL.h"
#include "Simulator.tpp"

void EDL::compute_schedule( const std::vector<Task *> tasks, const Task *running, double current_time )
{
    std::vector<Task *> tmp;
    std::vector<Task *> tmp_tasks;
    for( auto & element : tasks ) {
        tmp_tasks.push_back( new Task( element ) );
    }

    Task *tmp_running;
    if( running ) {
        tmp_running = new Task( running );
        tmp_running->update_remaining();
        tmp_tasks.push_back( tmp_running );
    }

    compute_hyperperiod( tmp_tasks );     // TODO sto s doubleovima
//    hyperperiod = 18;
    rto->set_finish_time(static_cast<double> (hyperperiod) );
    for( auto & element : tmp_tasks ) {
        element->set_arrival_time( current_time );
        element->set_abs_dd();
        element->set_skip_factor( 2 );
    }
    rto->set_pending( tmp_tasks );
    rto->set_abs_time( current_time );
    rto->simulate( 1 );
    deadline_vector = rto->get_deadline_vector();
    idle_time_vector = rto->get_idle_time_vector();
//    set_EDL_idle_time_vector();
//    compute_EDL_deadline_vector();
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
    EDL_deadline_vector.clear();
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
            if( isless( time, idle_interval + EDL_idle_time_vector[index] ) ) {
                return true;
            }
        }
    }
    return false;
}

bool EDL::dynamic_sched( std::vector<Task *> tasks, const Task *running, int current_time )
{
    std::vector<Task *> tmp;
    std::vector<Task *> tmp_tasks;
    for( auto & element : tasks ) {
        tmp_tasks.push_back( new Task( element ) );
    }

    Task *tmp_running;
    if( running ) {
        tmp_running = new Task( running );
        tmp_running->update_remaining();
        tmp_tasks.push_back( tmp_running );
    }

    for( auto & element : tmp_tasks ) {
        element->set_abs_dd();
        element->set_time_started( element->get_abs_due_date() - element->get_remaining() );
    }
    for( auto & element : tmp_tasks ) {
        if( current_time >= element->get_time_started() && current_time < element->get_abs_due_date()  ) {
            return false;
        }
    }
    return true;
}