//
// Created by karla on 18. 04. 2020..
//
#include "EDL.h"
#include "Simulator.tpp"

void EDL::compute_schedule(std::vector<Task *> &pending)
{
    compute_hyperperiod( pending );
    simulator->set_finish_time( hyperperiod );
    simulator->run();
    deadline_vector = simulator->get_deadline_vector();
    idle_time_vector = simulator->get_idle_time_vector();
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

std::vector<double> EDL::compute_EDL_deadline_vector( std::vector<double> init_deadline_vector )
{
    std::vector<double> result( init_deadline_vector.size() );
    for( size_t i=0; i<deadline_vector.size(); i++ ) {
        result[i] = hyperperiod -
                deadline_vector[deadline_vector.size() - i - 1] - idle_time_vector[i];
    }
    return result;
}

std::vector<double> EDL::update_schedule( std::vector<double> init_deadline_vector, double curr_time )
{
    std::vector<double> result( init_deadline_vector.size() );
    int k = floor( curr_time / hyperperiod );
    for( size_t i=0; i<result.size(); i++ ) {
        result[i] = init_deadline_vector[i] + k * hyperperiod;
    }
    return result;
}