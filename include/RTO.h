//
// Created by karla on 13. 04. 2020..
//

#ifndef FM_SCHEDULING_RTO_H
#define FM_SCHEDULING_RTO_H

#include "UunifastCreator.h"
#include "Scheduler.h"

class RTO {
public:
    RTO( int task_number, UunifastCreator *creator, Scheduler *sched, double finish_time ) :
        task_number( task_number ), creator( creator ), sched( sched ), finish_time( finish_time ) {}
    void initialize();
    void load();
    virtual void simulate( double time_slice );
    void set_qos( double value );
    double get_qos();
    std::vector<Task *> pending;
    double compute_eq_utilization();
    std::vector<Task *> get_pending()
    {
        return pending;
    }
    void set_finish_time( double time ) {
        this->finish_time = time;
    }
    std::vector<double> get_deadline_vector() {
        return deadline_vector;
    }
    std::vector<double> get_idle_time_vector() {
        return idle_time_vector;
    }
    void set_pending( std::vector<Task *> tasks ) {
        pending = tasks;
    }
    void set_running( Task *task ) {
        this->running = task;
    }
    void set_abs_time( double current_time ) {
        this->abs_time = current_time;
    }

protected:
    int task_number;
    int all_tasks = 0;
    int completed_tasks = 0;
    std::vector<Task *> ready;
    std::vector<Task *> red;
    std::vector<Task *> blue;
    UunifastCreator *creator;
    Scheduler *sched;
    double abs_time = 0;
    int missed = 0;
    double qos;
    double finish_time;
    double compute_D( int l, double period, double factor, double duration );

private:
    Task *running;
    bool idle;
    std::vector<double> deadline_vector;
    std::vector<double> idle_time_vector;
};

#endif //FM_SCHEDULING_RTO_H
