#include <pico/stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <etl/priority_queue.h>

static const uint8_t TASK_COUNT = 3;


class Task
{
public:
    /**
     * \brief constructor
     */
    Task(uint32_t update_interval_us)
    :update_interval_us_{update_interval_us}
    {};

    /**
     * \brief "less-than" operator for implementing task sorting.
     * \note naive implementation. Must handle uint32_t rollover.
     */
    friend bool operator<(const Task& lhs, const Task& rhs)
    {return lhs.next_update_time_ < rhs.next_update_time_;}

    void start_task(uint32_t start_time)
    {
        next_update_time_ = start_time;
        update();
    }

    void update()
    {
        // do stuff here. Set/clear/read some GPIOs, etc.
        // ...

        // Record next time to update.
        next_update_time_ += update_interval_us_;
    }


    uint32_t next_update_time_;

private:
    uint32_t update_interval_us_;
};


// Task object container
Task tasks[TASK_COUNT]
    {Task(1e6),     // 1 second update interval.
     Task(1e6),     // 1 second update interval.
     Task(2e6)};    // 2 second update interval.


// Priority queue of Task references.
// Sorting relies on the item type implementing the '<' operator
etl::priority_queue<Task, TASK_COUNT> pqueue{};

int main()
{
// USB stdio init. Block until connected to the seral port.
    stdio_usb_init();
    stdio_set_translate_crlf(&stdio_usb, false); // Don't replace outgoing chars.
    while (!stdio_usb_connected()){}

// Load the priority queue.
    for (auto task : tasks)
        pqueue.push(task);

// Kick off the schedule.
    const uint32_t& curr_time_us = uint32_t(get_absolute_time());
    for (auto task : tasks)
        task.start_task(curr_time_us);

// Run the schedule.
    while (true)
    {
        const uint32_t& curr_time_us = uint32_t(get_absolute_time());
        Task& curr_task = pqueue.top();
        if (curr_time_us < curr_task.next_update_time_)
            continue;
        curr_task.update();
        pqueue.pop();
        pqueue.push(curr_task);
    }
}
