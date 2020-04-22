//
// Created by karla on 13. 04. 2020..
//
#include <cmath>
#include "RTO.h"

void RTO::initialize()
{
    creator->create_test_set( pending );
    creator->write_tasks( pending );
    for( auto & element : pending ) {
        element->initialize_task();
//        element->set_skip_factor(2);
    }
}

void RTO::load()
{
    creator->load_tasks( pending );
    for( auto & element : pending ) {
        element->initialize_task();  //      deeply-red condition: first instance of every task is red
//        element->set_skip_factor(2);
    }
}

void RTO::simulate( double time_slice )
{
    double tmp_idle = 0;
    idle = false;
    double start_idle = abs_time;
    idle_time_vector.clear();
    deadline_vector.clear();

//    double max_period = pending[0]->get_period();
//
//    for( auto & element : pending ) {
//        if( element->get_period() > max_period ) {
//            max_period = element->get_period();
//        }
//    }
//
//    set_finish_time( max_period );
    while( abs_time < finish_time ) {
        red.clear();
        blue.clear();
        std::vector<Task *>::iterator it = pending.begin();
        while (it != pending.end()) {
            if ((*it)->isReady(abs_time)) {
                all_tasks++;
                (*it)->inc_instance();
                if ((*it)->get_state() == RED) {
                    ready.push_back(std::move(*it));
                } else {
//                    printf("task %d is skipped\n", (*it)->get_id());
                    blue.push_back(std::move(*it));
                }
                it = pending.erase(it);
//                printf("task %d is ready!\n", (*it)->get_id());
            } else {
                it++;
            }
        }
        std::copy(blue.begin(), blue.end(), std::back_inserter(pending));

        if( !running ) {
            if( !idle ) {
                start_idle = abs_time;
                idle = true;
//                printf("IDLE\n");
            }
            else {
                tmp_idle += time_slice;
//                printf("idle: %f\n", tmp_idle);
            }
        }
        else {
            if( std::isgreater(fabs( tmp_idle ), 0.) ) {
                idle_time_vector.push_back( tmp_idle );
                deadline_vector.push_back( start_idle );
                tmp_idle = 0;
                idle = false;
            }
        }

        if (!ready.empty()) {
//            printf("scheduling tasks : ");
//            for (auto &element : ready) {
//                printf("%d ", element->get_id());
//            }
//            printf("\n");

            for (size_t i = 0; i < ready.size(); i++) {
                ready[i]->update_priority(abs_time);
            }
            if (running) {
                running->update_priority(abs_time);
            }
            // printf( "%d: %f\n", ready[i]->id, ready[i]->priority );
            sched->schedule_next(ready, running, abs_time);
        }

        it = ready.begin();
        while (it != ready.end()) {
            // next arrival time is less than current time -> missed task, new instance appeared
            if ((*it)->is_missed(abs_time + time_slice)) {
//				printf("Missed task %d\n", (*it)->get_id() );
                pending.push_back(*it);
                it = ready.erase(it);
                missed++;
            } else {
                it++;
            }
        }

        abs_time += time_slice;
//        printf("\ntime: %f\n\n", abs_time);

        if (running) {
            idle = false;
            if (running->isFinished()) {
//                printf("task %d is finished!\n", running->get_id());
                running->update_tardiness(abs_time);
                running->reset_remaining();
                pending.push_back(std::move(running));
                running = nullptr;
                completed_tasks++;
            } else {
                running->update_remaining();
//				printf("remaining time: %f\n", running->get_remaining() );
            }
        }

        for (auto &element : pending) {
            if (element->is_next_instance(abs_time)) {
                element->update_rb_params();
            }
        }
    }

    if( idle ) {
        deadline_vector.push_back( start_idle );
        idle_time_vector.push_back( tmp_idle + time_slice );
    }

    set_qos( ( completed_tasks ) / static_cast<double>(all_tasks) );
//    printf( "qos: %f\n", get_qos() );
}

double RTO::compute_eq_utilization()
{
    double max=0;
    for( int l=1; l<finish_time; l++ ) {
        double sum = 0;
        for( auto & element : pending ) {
            sum += compute_D( l, element->get_period(), element->get_skip_factor(), element->get_duration() );
        }
        if( sum/l > max ) {
            max = sum/l;
        }
    }
    return max;
}

double RTO::compute_D( int l, double period, double factor, double duration )
{
    return ( floor( l / period ) - floor( l / ( period * factor ) )) * duration;
}

void RTO::set_qos( double value )
{
    this->qos = value;
}

double RTO::get_qos()
{
    return this->qos;
}
