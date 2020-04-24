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

    while( abs_time <= finish_time ) {
        algorithm(abs_time);
        abs_time += time_slice;
    }
}

void RLP::algorithm( double current_time )
{
//    printf("current time: %f\n", current_time);

    // check if running task is going to miss deadline
    if( running ) {
        if( current_time == running->get_abs_due_date() and
            std::isgreater( running->get_remaining()-time_slice, 0  )  ) {
            running->inc_instance();
            running->set_curr_skip_value(1);
            running->set_state( RED );
            running->set_arrival_time();
            running->reset_remaining();
            waiting.push_back( std::move( running ) );
            running = nullptr;
        }
    }
    // check blue ready in order to abort tasks
    std::vector<Task *>::iterator it;
    it = blue_ready.begin();
    while( it != blue_ready.end() ) {
        if ((*it)->is_next_instance(current_time)) {
            (*it)->set_curr_skip_value(1);
            (*it)->set_state(RED);
            (*it)->inc_instance();
            (*it)->set_arrival_time();
            waiting.push_back(std::move(*it));
            it = blue_ready.erase(it);
        } else {
            it++;
        }
    }

    it = waiting.begin();
    std::sort(waiting.begin(), waiting.end(),
              [](const Task *a, const Task *b)
                      -> bool { return a->get_arrival_time() < b->get_arrival_time(); });
    while( it != waiting.end() ) {
        if ((*it)->get_arrival_time() > current_time) {
            break;
        }
        if ((*it)->get_state() == RED) {
            red_ready.push_back(*it);
            it = waiting.erase(it);
        } else {
            blue_ready.push_back(*it);
            it = waiting.erase(it);
        }
    }

    bool availability;

    if( running ) {
        availability = edl->dynamic_sched(  red_ready, running->get_state()==BLUE ? nullptr : running, current_time  );
    }
    else {
        availability = edl->dynamic_sched( red_ready, nullptr, current_time);
    }
    // idle time -> suspend red task
    if( !blue_ready.empty() and availability ) {
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
    if( !blue_ready.empty() ) {
        std::sort(blue_ready.begin(), blue_ready.end(),
                  [](const Task *a, const Task *b)
                          -> bool { return a->get_abs_due_date() < b->get_abs_due_date(); });
    }

    if( running ) {
        if( running->isFinished() ) {
            running->reset_remaining();
            running->inc_instance();
            running->update_rb_params();
            printf( "time : %f\ttask %d is finished\n", current_time, running->get_id() );
            waiting.push_back( std::move(running) );
            running = nullptr;
        }
    }

    if( !running ) {
        if( blue_ready.empty() ) {
            running = std::move( red_ready[0] );
            red_ready.erase( red_ready.begin() );
            printf( "time : %f\tred instance of task %d is running, %f remaining\n", current_time, running->get_id(), running->get_remaining() );
        }
        else {
            if( availability ) {
                if( blue_ready.size() > 1 ) {
                    break_dd_tie( blue_ready );
                }
                running = std::move( blue_ready[0] );
                blue_ready.erase( blue_ready.begin() );
                printf( "time : %f\tblue instance of task %d is running, %f remaining\n", current_time, running->get_id(), running->get_remaining() );
            }
            else {   // schedule red by EDL
                running = std::move( red_ready[0] );
                red_ready.erase( red_ready.begin() );
                printf( "time : %f\tred instance of task %d is running, %f remaining\n", current_time, running->get_id(), running->get_remaining() );
            }
        }
    }

    else {
        running->update_remaining();
//        availability = edl->dynamic_sched( red_ready, running->get_state()==BLUE ? nullptr : running, current_time );
        if( running->get_state() == RED and availability and !blue_ready.empty() ) {
//            running->update_remaining();
            red_ready.push_back( running );
            running = std::move( blue_ready[0] );
            blue_ready.erase( blue_ready.begin() );
            printf( "time : %f\tblue instance of task %d is running, %f remaining\n", current_time, running->get_id(), running->get_remaining() );

        }
        else {
            if( !red_ready.empty() and running->get_state() == BLUE ) {
                availability = edl->dynamic_sched(red_ready, nullptr, current_time);
                if (!availability) {
//                    running->update_remaining();
                    blue_ready.push_back(running);
                    running = std::move(red_ready[0]);
                    red_ready.erase(red_ready.begin());
                    printf( "time : %f\tred instance of task %d is running, %f remaining\n", current_time, running->get_id(), running->get_remaining() );

                }
            }
        }
    }
//    printf( "task %d is running, %f remaining\n", running->get_id(), running->get_remaining() );
}

// if multiple tasks in ready list have the same abs due date, schedule the one with earliest release time
void RLP::break_dd_tie( std::vector<Task *> tasks )
{
//    Task *earliest = new Task( tasks[0] );
    for( size_t i=1; i<tasks.size(); i++ ) {
        if( fabs( tasks[i]->get_abs_due_date() - tasks[0]->get_abs_due_date() ) < 0.001 ) {
            if( std::isless( tasks[i]->get_arrival_time(), tasks[0]->get_arrival_time() ) ) {
                std::iter_swap( tasks.begin(), tasks.begin() + i );
            }
        }
    }
}