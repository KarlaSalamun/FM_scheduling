//
// Created by karla on 13. 04. 2020..
//
#include "RTO.h"

void RTO::initialize()
{
    creator->create_test_set( pending );
    for( auto & element : pending ) {
        element->initialize_task();
        if( rand() % 2 ) {
            element->set_state( RED );
        }
        else {
            element->set_state( BLUE );
        }
    }
}

void RTO::simulate( double finish_time, double time_slice )
{
    Task *running = nullptr;				// TODO: ovo je leak
    while( abs_time < finish_time ) {
        std::vector<Task *>::iterator it = pending.begin();

        while( it != pending.end() ) {

            if ( (*it)->isReady( abs_time ) ) {
                (*it)->inc_instance();
                if( (*it)->get_state() == RED ) {
                    ready.push_back( std::move( *it ) );
                    it = pending.erase( it );
                }
//				printf( "task %d is ready!\n", (*it)->get_id() );
            }
            else {
                it++;
            }
        }

        if( !ready.empty() ) {
//			printf( "scheduling tasks : " );
//			for( auto & element : ready ) {
//				printf( "%d ", element->get_id() );
//			}
//			printf( "\n" );

            for( size_t i=0; i<ready.size(); i++ ) {
                ready[i]->update_priority(abs_time);
            }
                // printf( "%d: %f\n", ready[i]->id, ready[i]->priority );

            sched->schedule_next( ready, running, abs_time );
        }

        it = ready.begin();
        while( it != ready.end() ) {
            // next arrival time is less than current time -> missed task, new instance appeared
            if( (*it)->is_missed( abs_time + time_slice ) ) {
//				printf("Missed task %d\n", (*it)->get_id() );
                pending.push_back( *it );
                it = ready.erase( it );
                missed++;
            }
            else {
                it++;
            }
        }

        abs_time += time_slice;
//		printf( "\ntime: %f\n\n", abs_time );

        if( running ) {
            if( running->isFinished() ) {
//				printf( "task %d is finished!\n", running->get_id() );
                running->update_tardiness( abs_time );
                pending.push_back( std::move( running ) );
                running = nullptr;
            }
            else {
                running->update_remaining();
//				printf("remaining time: %f\n", running->get_remaining() );
            }
        }

        for( auto & element : pending ) {
            if( element->is_next_instance( abs_time ) ) {
                element->update_params();
            }
        }
    }

    qos = ( task_number - missed ) / static_cast<double>(task_number);
}