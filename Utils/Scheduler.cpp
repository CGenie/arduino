#include "Scheduler.h"


TaskWithTick::TaskWithTick(ITask* task) {
    this->tick = millis();
    this->task = task;
}


Scheduler::Scheduler() {
    this->tasks = new LinkedList<TaskWithTick*>();
};

Scheduler::~Scheduler() {
    this->tasks->clear();
    delete this->tasks;
}

void Scheduler::schedule(ITask* task) {
    this->tasks->add(new TaskWithTick(task));
};

void Scheduler::run() {
    unsigned long current_tick = millis();
    int i;
    TaskWithTick* task_with_tick;

    for(i = 0; i < this->tasks->size(); i++) {
        task_with_tick = this->tasks->get(i);
        if(current_tick - task_with_tick->tick >= task_with_tick->task->delay) {
            task_with_tick->task->run();
            task_with_tick->tick = current_tick;
        }
    }
};
