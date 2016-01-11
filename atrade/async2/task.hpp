#ifndef ATRADE_ASYNC_2_TASK_H
#define ATRADE_ASYNC_2_TASK_H

#include "../headers.h"

namespace atrade {
namespace async2 {

class Task
{
    friend class TaskService;

    template <typename Function>
    Task(TaskService* service, const pt::ptime& datetime, Function&& task);

public:
    template <typename Function>
    Task(TaskService& service, const pt::ptime& datetime, Function&& task) :
        Task(&service, datetime, std::forward<Function>(task))
    {
    }

    template <typename Function>
    Task(const pt::ptime& datetime, Function&& task) :
        Task(nullptr, datetime, std::forward<Function>(task))
    {
    }

    Task() :
        Task(nullptr, pt::not_a_date_time, nullptr)
    {
    }

    Task(const Task&) = delete;
    Task& operator= (const Task&) = delete;

    Task(Task&& task);
    Task& operator= (Task&& task);

    Task& operator= (std::nullptr_t);

    ~Task();

    explicit operator bool() const;
    bool scheduled() const;
    bool executed() const;
    bool canceled() const;
    pt::ptime datetime() const;
    void cancel();
    void reset();

    class Order
    {
    public:
        bool operator() (const Task* t1, const Task* t2);
    };

private:
    TaskService* m_service;
    u_int64_t m_id;
    pt::ptime m_datetime;
    std::function<void()> m_task;
    bool m_executed;
    bool m_canceled;

    void run();
    void move(Task&& task);
};

class TaskService
{
public:
    TaskService();
    ~TaskService();

    void push(Task* task);
    void remove(Task* task);
    void execute_first();
    void clear();
    pt::ptime datetime() const;

private:
    u_int64_t m_task_id;
    std::multiset<Task*, Task::Order> m_tasks;
};

template <typename Function>
Task::Task(TaskService* service, const pt::ptime& datetime, Function&& task) :
    m_service(nullptr),
    m_id(0),
    m_datetime(datetime),
    m_task(std::forward<Function>(task)),
    m_executed(false),
    m_canceled(false)
{
    if (service)
    {
        service->push(this);
    }
}

}
}

#endif // ATRADE_ASYNC_2_TASK_H
