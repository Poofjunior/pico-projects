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
    Task(uint32_t update_interval_us, const char* name)
    :update_interval_us_{update_interval_us}, name_{name}
    {};

    /**
     * \brief "less-than" operator for implementing task sorting.
     * \note Signed subtraction handles uint32_t rollover.
     */
    friend bool operator<(const Task& lhs, const Task& rhs)
    {return int32_t(rhs.next_update_time_ - lhs.next_update_time_) > 0;}

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
        printf("%s updated. Next update @ t = %d\r\n", name_, next_update_time_);
    }

    uint32_t next_update_time_;
    const char* name_;

private:
    uint32_t update_interval_us_;
};


// Task object container
Task tasks[TASK_COUNT]
    {{1000000, "task0"},     // 1 second update interval.
     {2000000, "task1"},     // 2 second update interval.
     {3000000, "task2"}};    // 3 second update interval.


// Priority queue of Task references.
//   Use references so that we can instatiate tasks with custom params rather
//   than pushing or emplacing objects one by one.
// Note: Sorting relies on the item type implementing the '<' operator
etl::priority_queue<std::reference_wrapper<Task>,
                    TASK_COUNT,
                    etl::vector<std::reference_wrapper<Task>, TASK_COUNT>,
                    etl::greater<std::reference_wrapper<Task>>> pqueue{};

int main()
{
// USB stdio init. Block until connected to the seral port.
    stdio_usb_init();
    stdio_set_translate_crlf(&stdio_usb, false); // Don't replace outgoing chars.
    while (!stdio_usb_connected()){}

// Load the priority queue.
    for (auto& task : tasks)
        pqueue.push(task);

// Kick off the schedule.
    const uint32_t& curr_time_us = uint32_t(get_absolute_time());
    printf("starting tasks at: %d\r\n", curr_time_us);
    for (auto& task : tasks)
        task.start_task(curr_time_us);

// Run the schedule.
    while (true)
    {
        const uint32_t& curr_time_us = uint32_t(get_absolute_time());
        auto& curr_task = pqueue.top().get();
        if (curr_time_us < curr_task.next_update_time_)
            continue;
        pqueue.pop();
        printf("update @ %d. | ", curr_time_us);
        curr_task.update();
        pqueue.push(curr_task);
    }
}
