#include "RLP.h"

void RLP::schedule_next( double current_time )
{
    std::vector<Task *>::iterator it;
    it = blue_ready.begin();
    while( it != blue_ready.end() ) {
        if( (*it)->get_arrival_time() < current_time ) {
            break;
        }// critical delay ???
        (*it)->set_arrival_time( task->get_arrival_time() + task->get_period() );
        (*it)->set_skip_factor( 1 );
        waiting.push_back( *it );
        it = blue_ready.erase( it );
    }
    it = waiting.begin();
    while( (it != waiting.end() ) {
        if( (*it)->get_arrival_time() > current_time ) {
            break;
        }
        if( (*it)->get_skip_factor() < (*it)->get_max_skip_factor()
            and EDL( current_time ) = 0 ) {
            red_ready.push_back( *it );
            waiting.erase( it );
        }
        else {
            if( !blue_ready.empty() ) {
                edl->compute();
            }
            if( edl->get_f( current_time ) != 0 ) {
                blue_ready.push_back( *it );
                waiting.erase( it );
            }
        }
        (*it)->set_skip_factor( (*it)->get_skip_factor() + 1 );
    }
    if( !blue_ready.empty() and edl->get_f( current_time ) != 0 ) {
        it = red_ready.begin();
        while( it != red_ready.end() ) {
            waiting.push_back( *it );
            red_ready.erase( it );
        }
    }
}
