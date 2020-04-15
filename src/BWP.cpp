//
// Created by karla on 15. 04. 2020..
//
#include "BWP.h"

void BWP::simulate( double finish_time, double time_slice )
{
    Task *running = nullptr;				// TODO: ovo je leak
    while( abs_time < finish_time ) {
//        red.clear();
//        blue.clear();
        std::vector<Task *>::iterator it = pending.begin();

        while( it != pending.end() ) {
            if ( (*it)->isReady( abs_time ) ) {
                all_tasks++;
                (*it)->inc_instance();
                if( (*it)->get_state() == RED ) {
                    red.push_back( std::move( *it ) );
                }
                else {
                    printf( "task %d is skipped\n", (*it)->get_id() );
                    blue.push_back( std::move( *it ) );
                }
                it = pending.erase( it );
                printf( "task %d is ready!\n", (*it)->get_id() );
            }
            else {
                it++;
            }
        }
//        std::copy( blue.begin(), blue.end(), std::back_inserter( pending ) );

        if( !red.empty() ) {
            printf( "scheduling red tasks : " );
            for( auto & element : red ) {
                printf( "%d ", element->get_id() );
            }
            printf( "\n" );

            for( size_t i=0; i<red.size(); i++ ) {
                red[i]->update_priority(abs_time);
            }
            if( running ) {
                if( running->get_state() == BLUE ) {    // suspend blue task
                    blue.push_back( std::move(running) );
                    running = nullptr;
                }
                else {
                    running->update_priority( abs_time );
                }
            }
            // printf( "%d: %f\n", ready[i]->id, ready[i]->priority );
            sched->schedule_next( red, running, abs_time );
        }
        else if (!blue.empty() ) {
            if( running == nullptr  || running->get_state() == BLUE ) {
                printf( "scheduling blue tasks : " );
                for( auto & element : blue ) {
                    printf( "%d", element->get_id() );
                }
                printf( "\n" );

                for( size_t i=0; i<blue.size(); i++ ) {
                    blue[i]->update_priority( abs_time );
                }
                if( running ) {
                    running->update_priority( abs_time );
                }
                sched->schedule_next( blue, running, abs_time );
            }
        }

        it = red.begin();
        while( it != red.end() ) {
            // next arrival time is less than current time -> missed task, new instance appeared
            if( (*it)->is_missed( abs_time + time_slice ) ) {
//				printf("Missed task %d\n", (*it)->get_id() );
                pending.push_back( *it );
                it = red.erase( it );
                missed++;
            }
            else {
                it++;
            }
        }
        it = blue.begin();
        while( it != blue.end() ) {
            // next arrival time is less than current time -> missed task, new instance appeared
            if( (*it)->is_missed( abs_time + time_slice ) ) {
//				printf("Missed task %d\n", (*it)->get_id() );
                pending.push_back( *it );
                it = blue.erase( it );
                missed++;
            }
            else {
                it++;
            }
        }

        abs_time += time_slice;
        printf( "\ntime: %f\n\n", abs_time );

        if( running ) {
            if( running->missed_deadline( abs_time ) ) {
                printf( "task %d missed deadline!\n", running->get_id() );
                running->update_tardiness( abs_time );
                running->reset_remaining();
                pending.push_back( std::move( running ) );
                running = nullptr;
            }
            else if( running->isFinished() ) {
                printf( "task %d is finished!\n", running->get_id() );
                running->update_tardiness( abs_time );
                running->reset_remaining();
                pending.push_back( std::move( running ) );
                running = nullptr;
                completed_tasks++;
            }
            else {
                running->update_remaining();
//				printf("remaining time: %f\n", running->get_remaining() );
            }
        }

        for( auto & element : pending ) {
            if( element->is_next_instance( abs_time ) ) {
                element->update_rb_params();
            }
        }
    }

    set_qos( ( completed_tasks ) / static_cast<double>(all_tasks) );
    printf( "qos: %f\n", get_qos() );
}
