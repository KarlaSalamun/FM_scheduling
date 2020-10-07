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

void EDL::compute_static( std::vector<Task *> tasks )
{
    // create deadline vector
    EDL_deadline_vector.push_back( 0 );
    rto->creator->compute_hyperperiod( tasks );
    hyperperiod = rto->creator->get_hyperperiod();
    for( auto & element : tasks ) {
        for( size_t instance=0; instance < (hyperperiod / element->get_period()); instance += element->get_skip_factor() ) {
            EDL_deadline_vector.push_back( instance * static_cast<uint32_t>(element->get_period() ) + element->get_period());
        }
    }
    // sort by increasing arrival time
    std::sort( EDL_deadline_vector.begin(), EDL_deadline_vector.end() );
    // remove duplicates
    EDL_deadline_vector.erase( std::unique( EDL_deadline_vector.begin(), EDL_deadline_vector.end() ), EDL_deadline_vector.end() );

    // create idle time vector
    EDL_idle_time_vector.assign( EDL_deadline_vector.size(), 0 );
    // sort tasks by increasing period
    std::sort(tasks.begin(), tasks.end(),
              [](const Task *a, const Task *b)
                      -> bool { return a->get_period() < b->get_period(); });
    EDL_idle_time_vector.back() = 0;
    int f_value;
    for( int i=EDL_deadline_vector.size()-2; i>=0; i-- ) {
        int sum_1 = 0;
        int sum_2 = 0;
        f_value = static_cast<int>(hyperperiod) - EDL_deadline_vector[i];
        for( auto & task : tasks ) {
            sum_1 += std::ceil(static_cast<double> (f_value) / ( task->get_skip_factor() * task->get_period())) * task->get_duration();
        }
        for( size_t k=i+1; k<EDL_idle_time_vector.size(); k++ ) {
            sum_2 += EDL_idle_time_vector[k];
        }
        f_value = f_value - sum_1 - sum_2;
        EDL_idle_time_vector[i] = std::max( f_value, 0 );
    }
//    EDL_deadline_vector.erase(EDL_deadline_vector.end() - 1 );
//    EDL_idle_time_vector.erase( EDL_idle_time_vector.end() - 1 );
}

void EDL::compute_dynamic( uint32_t time, const std::vector<Task *> &ready_tasks, Task *running ) {
    size_t index = 0;
    EDL_dynamic_deadline_vector.clear();
    EDL_dynamic_idle_time_vector.clear();
    EDL_dynamic_deadline_vector.push_back( time );

    std::vector<Task *> tmp_tasks;
    for( auto & element : ready_tasks ) {
        tmp_tasks.push_back( element );
    }
    if( running ) {
        tmp_tasks.push_back(running);
        tmp_tasks.back()->update_remaining();
    }

    for( auto & element : tmp_tasks ) {
        uint32_t instance;
        for( instance=0; instance < std::floor(hyperperiod / element->get_period()); instance += 1 ) {
            uint32_t instant = instance * static_cast<uint32_t>(element->get_period() ) + element->get_period();
            if( instant > time ) {
                break;
            }
        }
        for( size_t i=instance; i<(hyperperiod / element->get_period()); i+=element->get_skip_factor() ) {
            EDL_dynamic_deadline_vector.push_back( instance * static_cast<uint32_t>(element->get_period() ) + element->get_period());
        }
    }

    std::sort( EDL_dynamic_deadline_vector.begin(), EDL_dynamic_deadline_vector.end() );
    if( EDL_dynamic_deadline_vector[0] == EDL_dynamic_deadline_vector[1] ) {
        EDL_dynamic_deadline_vector.erase( EDL_dynamic_deadline_vector.begin() );
    }
    if( EDL_dynamic_deadline_vector.back() != hyperperiod ) {
        EDL_dynamic_deadline_vector.push_back( hyperperiod );
    }
    EDL_dynamic_deadline_vector.erase( std::unique( EDL_dynamic_deadline_vector.begin(), EDL_dynamic_deadline_vector.end() ), EDL_dynamic_deadline_vector.end() );

    if( EDL_dynamic_deadline_vector.size() == 1 ) {
        return;
    }

    std::sort(tmp_tasks.begin(), tmp_tasks.end(),
              [](const Task *a, const Task *b)
                      -> bool { return a->get_abs_due_date() > b->get_abs_due_date(); });
    auto M = static_cast<uint32_t> ( tmp_tasks.front()->get_abs_due_date() );

    index = 0;
    while( EDL_dynamic_deadline_vector[index] <= M ) {
        index++;
    }
    EDL_dynamic_idle_time_vector.assign( EDL_dynamic_deadline_vector.size(), 0 );

//    if( index >= EDL_dynamic_deadline_vector.size() ) {
//        EDL_dynamic_idle_time_vector.back() = EDL_idle_time_vector.back();
//    }

    EDL_dynamic_idle_time_vector.back() = 0;

    for( size_t i=EDL_dynamic_deadline_vector.size() - 2; i>=index; i-- ) {
        EDL_dynamic_idle_time_vector[i] = EDL_idle_time_vector[i];
    }

    for( size_t i = EDL_dynamic_idle_time_vector.size() - 2; i>=1; i-- ) {
        double f_value = hyperperiod - EDL_dynamic_deadline_vector[i];
        double sum_1 = 0;
        double sum_2 = 0;
        double sum_3 = 0;
        for( auto & task : tmp_tasks ) {
            sum_1 += ( std::ceil( f_value / task->get_period() ) -
                    std::ceil( f_value / (task->get_skip_factor() * task->get_period())) ) * task->get_duration();
            if( task->get_abs_due_date() > EDL_dynamic_deadline_vector[i] ) {
                sum_2 += task->get_duration() - task->get_remaining();
            }
        }
        for( size_t k=i+1; k<EDL_dynamic_idle_time_vector.size(); k++ ) {
            sum_3 += EDL_dynamic_idle_time_vector[k];
        }
        EDL_dynamic_idle_time_vector[i] = f_value - sum_1 + sum_2 - sum_3;
    }
    double f_value = hyperperiod - time;
    double sum_1 = 0;
    for( auto & task : tmp_tasks ) {
//        sum_1 += std::ceil(static_cast<double> (f_value) / task->get_period()) * task->get_duration();
          sum_1 +=  std::ceil(static_cast<double> (f_value) / ( task->get_period() * task->get_skip_factor() )) * task->get_duration() ;
        sum_1 = sum_1 - ( task->get_duration() - task->get_remaining() );
    }
    double sum_2 = 0;
    for(size_t j=1; j<EDL_dynamic_idle_time_vector.size(); j++) {
        sum_2 += EDL_dynamic_idle_time_vector[j];
    }
    EDL_dynamic_idle_time_vector[0] = f_value - sum_1 - sum_2;

    assert( !EDL_dynamic_idle_time_vector.empty() );
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
    for( size_t i=0; i<EDL_dynamic_deadline_vector.size(); i++ ) {
        if( time >= EDL_dynamic_deadline_vector[i] ) {
            idle_interval = EDL_dynamic_deadline_vector[i];
            index = i;
            if( isless( time, idle_interval + EDL_dynamic_idle_time_vector[index] ) ) {
                return true;
            }
        }
    }
    return false;
}

bool EDL::dynamic_sched( const std::vector<Task *> tasks, const Task *running, int current_time )
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