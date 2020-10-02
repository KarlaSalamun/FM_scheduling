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

    rto->creator->compute_hyperperiod( tmp_tasks );     // TODO sto s doubleovima
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

    std::sort(tmp_tasks.begin(), tmp_tasks.end(),
              [](const Task *a, const Task *b)
                      -> bool { return a->get_period() > b->get_period(); });

    tmp_tasks[0]->set_abs_dd();
    double time_started = tmp_tasks[0]->get_abs_due_date() - tmp_tasks[0]->get_remaining();
    tmp_tasks[0]->set_time_started( time_started );
    for( size_t i = 1; i<tmp_tasks.size(); i++ ) {
        tmp_tasks[i]->set_abs_dd();
        if( tmp_tasks[i]->get_abs_due_date() > time_started ) {
            time_started = time_started - tmp_tasks[i]->get_remaining();
        }
        else {
            time_started = tmp_tasks[i]->get_abs_due_date() - tmp_tasks[i]->get_remaining();
        }
        tmp_tasks[i]->set_time_started( time_started );
    }

    for( auto & element : tmp_tasks ) {
        if( current_time >= element->get_time_started() && current_time < element->get_abs_due_date()  ) {
            return false;
        }
    }
    return true;
}