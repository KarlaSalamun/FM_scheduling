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

void RTO::set_pending( std::vector<Task *> tasks )
{
    this->pending = tasks;
    for( auto & element : pending ) {
        element->initialize_task();
        element->set_skip_factor( 2 );
    }
}

void RTO::simulate( double time_slice )
{
    FILE *fd = fopen( filename.c_str(), "w+" );
    if( display_sched ) {
        fprintf( fd, "\\documentclass{article}" );
        fprintf( fd, "\\usepackage{rtsched}" );
        fprintf( fd, "\\begin{document}" );
        fprintf( fd, "\\begin{RTGrid}[width=15cm]{%zu}{%d}\n", pending.size(), static_cast<int>( finish_time ));
    }
    abs_time = 0;
    all_tasks = 0;
    completed_tasks = 0;
    missed = 0;
    double tmp_idle = 0;
    idle = false;
    double start_idle = abs_time;
    idle_time_vector.clear();
    deadline_vector.clear();
    ready.clear();
    blue.clear();
    running = nullptr;

    while( abs_time < finish_time ) {

        std::vector<Task *>::iterator it;
        it = blue.begin();
        while( it != blue.end() ) {
            if ((*it)->is_next_instance(abs_time)) {
                (*it)->set_curr_skip_value(1);
                (*it)->set_state(RED);
                (*it)->inc_instance();
                (*it)->update_params();
                pending.push_back(std::move(*it));
                it = blue.erase(it);
                missed++;
            } else {
                it++;
            }
        }

        it = pending.begin();
        std::sort(pending.begin(), pending.end(),
                  [](const Task *a, const Task *b)
                          -> bool { return a->get_arrival_time() < b->get_arrival_time(); });

        while (it != pending.end()) {
            if ((*it)->get_arrival_time() > abs_time) {
                break;
            }
            if ((*it)->get_state() == RED) {
                ready.push_back(*it);
            } else {
                blue.push_back(*it);
            }
            if( display_sched )
                fprintf( fd, "\t\\TaskArrDead{%d}{%d}{%d}\n", (*it)->get_id()+1, static_cast<int>( abs_time ), static_cast<int>( (*it)->get_period() ) );
            it = pending.erase( it );
            all_tasks++;
        }

//        std::copy(blue.begin(), blue.end(), std::back_inserter(pending));

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
            if( display_sched )
                fprintf( fd, "\t\\TaskExecDelta{%d}{%d}{%d}\n", (*it)->get_id()+1, static_cast<int>( abs_time ), static_cast<int>( time_slice ));
        }

        abs_time += time_slice;
//        printf("\ntime: %f\n\n", abs_time);

        if (running) {
            idle = false;
            if (running->isFinished()) {
//                printf("task %d is finished!\n", running->get_id());
                running->update_tardiness(abs_time);
                running->reset_remaining();
                running->inc_instance();
                running->update_rb_params();
                pending.push_back(std::move(running));
                running = nullptr;
                completed_tasks++;
            } else {
                running->update_remaining();
//				printf("remaining time: %f\n", running->get_remaining() );
            }
        }

//        for (auto &element : pending) {
//            if (element->is_next_instance(abs_time)) {
//                element->update_params();
//            }
//        }
    }

    if( idle ) {
        deadline_vector.push_back( start_idle );
        idle_time_vector.push_back( tmp_idle + time_slice );
    }

    if( running ) {
        if( running->isFinished() ) {
            completed_tasks++;
        }
    }

    if( display_sched ) {
        fprintf( fd, "\\end{RTGrid}\n" );
        fprintf( fd, "\\end{document}\n" );
        fclose( fd );
    }

    set_qos( ( completed_tasks ) / static_cast<double>(all_tasks) );
//    printf( "qos: %f\n", get_qos() );
}

double RTO::compute_eq_utilization()
{
    double max=0;
    int max_index = 1;
    for( int l=1; l<finish_time; l++ ) {
        double sum = 0;
        for( auto & element : pending ) {
            sum += compute_D( l, element->get_period(), element->get_skip_factor(), element->get_duration() );
        }
        if( sum/l > max ) {
            max = sum/l;
            max_index = l;
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
