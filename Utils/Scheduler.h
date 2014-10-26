// Generic class for performing tasks on specific time intervals
// Helps get rid of delay() clutter

// Uses the LinkedList library implementation
// https://github.com/ivanseidel/LinkedList

#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include <Arduino.h>
#include <LinkedList.h>


// Interface for a scheduler task
class ITask {
    public:
        unsigned long delay;  // interval for calling the work() function, in milliseconds

        virtual void run();  // method that will be called periodically
};


// This class is used internally by the Scheduler
// to track when the task tick was made
class TaskWithTick {
    public:
        unsigned long tick;
        ITask* task;

        TaskWithTick(ITask* task);
        ~TaskWithTick();
};


class Scheduler {
    private:
        LinkedList <TaskWithTick*> *tasks;  // array of pointers to tasks

    public:
        Scheduler();
        ~Scheduler();

        void schedule(ITask* task);  // append task to Scheduler
        void run();  // perform all pending tasks
};

#endif
