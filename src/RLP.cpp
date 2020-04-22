#include "RLP.h"

void RLP::simulate( double finish_time )
{

//    edl->compute_EDL_deadline_vector();
//    edl->set_EDL_idle_time_vector();

    for( auto & element : waiting ) {
        element->set_state( RED );      // first instance is always red
        element->set_arrival_time( 0 );     // all instances have release time = 0
        element->initialize_task();
        element->set_skip_factor(2);
        element->set_abs_dd();
    }

    edl->compute_schedule( waiting, nullptr, 0 );
    std::vector<double> tmp = edl->get_idle_time_vector();
    edl->set_EDL_idle_time_vector( tmp );
    tmp = edl->get_deadline_vector();
    edl->set_EDL_deadline_vector( tmp );

    for( auto & element : waiting ) {
        element->set_state( RED );      // first instance is always red
        element->set_arrival_time( 0 );     // all instances have release time = 0
        element->initialize_task();
        element->set_skip_factor(2);
        element->set_abs_dd();
    }

    while( abs_time < finish_time ) {
        algorithm(abs_time);
        abs_time += time_slice;
    }
}

void RLP::algorithm( double current_time )
{
    printf("current time: %f\n", current_time);
    std::vector<Task *>::iterator it;
    it = blue_ready.begin();
    // check blue ready in order to abort tasks
    while( it != blue_ready.end() ) {
        if( (*it)->get_arrival_time() + (*it)->get_period() < current_time ) {
            break;
        }// critical delay = period???
        (*it)->set_arrival_time( (*it)->get_arrival_time() + (*it)->get_period() );
        (*it)->set_curr_skip_value( 1 );
        waiting.push_back( *it );
        it = blue_ready.erase( it );
    }
    it = waiting.begin();
    while( it != waiting.end() ) {
        if( (*it)->get_arrival_time() > current_time ) {
            break;
        }
        bool available = edl->compute_availability( current_time );
        if( (*it)->get_curr_skip_value() < (*it)->get_skip_factor() and !available ) {
            (*it)->set_state( RED );
            red_ready.push_back(*it);
            it = waiting.erase(it);
        }
        else {
            if( blue_ready.empty() ) {
                edl->compute_schedule( red_ready, running, current_time );
                printf("computed EDL schedule\n" );
                edl->update_EDL_idle_time_vector();
                edl->compute_EDL_deadline_vector();
                edl->update_schedule(current_time);
            }
            if( edl->compute_availability( current_time ) ) {
                // release blue task
                blue_ready.push_back( *it );
                (*it)->set_state( BLUE );
                it = waiting.erase( it );
            }
            else {
                it++;
            }
        }
        (*it)->set_curr_skip_value( (*it)->get_curr_skip_value() + 1 );
    }
    bool availability = edl->compute_availability( current_time );
    if( !blue_ready.empty() and !availability ) {
        it = red_ready.begin();
        while( it != red_ready.end() ) {
            waiting.push_back( *it );
            red_ready.erase( it );
        }
    }
    if( !red_ready.empty() ) {
        std::sort(red_ready.begin(), red_ready.end(),
                  [](const Task *a, const Task *b)
                          -> bool { return a->get_abs_due_date() < b->get_abs_due_date(); });
    }
    else {
        printf( "no red tasks are ready\n" );
    }
    if( !blue_ready.empty() ) {
        std::sort(blue_ready.begin(), blue_ready.end(),
                  [](const Task *a, const Task *b)
                          -> bool { return a->get_abs_due_date() < b->get_abs_due_date(); });
    }
    else {
        printf( "no blue tasks are ready\n" );
    }

    if( running ) {
        if( running->get_state() == RED and availability ) {
            running->update_remaining();
            red_ready.push_back( running );
            running = blue_ready[0];
            blue_ready.erase( blue_ready.begin() );
            printf( "blue task is running!\n" );
        }
        if( running->isFinished() ) {
            running->reset_remaining();
            running->inc_instance();
            running->set_arrival_time();
            running->update_params();
            printf( "task %d is finished\n", running->get_id() );
            waiting.push_back( running );
            running = nullptr;
        }
        else {
            running->update_remaining();
        }
    }

    if( !running ) {
        if( blue_ready.empty() ) {
            running = std::move( red_ready[0] );
            red_ready.erase( red_ready.begin() );
        }
        else {
            if( edl->compute_availability( abs_time ) ) {
                running = std::move( blue_ready[0] );
                blue_ready.erase( blue_ready.begin() );
            }
            else {   // schedule red by EDL
                running = std::move( red_ready[ red_ready.size() - 1 ] );
                red_ready.erase( red_ready.end() );
            }
        }
    }

//    printf( "red list: " );
//    for(auto & element: red_ready) {
//        printf( "%d ", element->get_id() );
//    }
//    printf( "\n" );
//    printf( "blue list: " );
//    for(auto & element: blue_ready) {
//        printf( "%d ", element->get_id() );
//    }
//    printf( "\n" );

    printf( "task %d is running\n", running->get_id() );
}
