#include "task.hpp"
#include "../utils.hpp"
#include "exceptions.hpp"

namespace atrade {
namespace async2 {

Task::Task(Task&& task)
{
    move(std::move(task));
}

Task& Task::operator= (Task&& task)
{
    if (this == &task)
    {
        return *this;
    }
    move(std::move(task));
    return *this;
}

Task& Task::operator= (std::nullptr_t)
{
    reset();
    return *this;
}

Task::~Task()
{
    cancel();
}

Task::operator bool() const
{
    return scheduled() || m_executed || m_canceled;
}

bool Task::scheduled() const
{
    return m_service != nullptr;
}

bool Task::executed() const
{
    return m_executed;
}

bool Task::canceled() const
{
    return m_canceled;
}

pt::ptime Task::datetime() const
{
    return m_datetime;
}

void Task::cancel()
{
    if (m_service)
    {
        m_service->remove(this);
    }
}

void Task::reset()
{
    cancel();
    m_service = nullptr;
    m_datetime = pt::max_date_time;
    m_task = nullptr;
    m_executed = false;
    m_canceled = false;
}

void Task::run()
{
    if (m_task)
    {
        m_task();
    }
}

void Task::move(Task&& task)
{
    cancel();
    m_service = nullptr;
    TaskService* service = task.m_service;
    m_id = task.m_id;
    m_datetime = task.m_datetime;
    m_task = std::move(task.m_task);
    m_executed = task.m_executed;
    m_canceled = task.m_canceled;
    task.cancel();
    if (service)
    {
        service->push(this);
    }
}

bool Task::Order::operator() (const Task* t1, const Task* t2)
{
    if (t1->m_datetime != t2->m_datetime)
    {
        return t1->m_datetime < t2->m_datetime;
    }
    else
    {
        return t1->m_id < t2->m_id;
    }
}

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

TaskService::TaskService() :
    m_task_id(0)
{
}

TaskService::~TaskService()
{
    clear();
}

void TaskService::push(Task* task)
{
    if (!task || task->m_datetime == pt::not_a_date_time || !task->m_task)
    {
        throw BadTaskException();
    }
    if (task->m_canceled)
    {
        throw TaskCanceledException();
    }
    if (task->m_executed)
    {
        throw TaskExecutedException();
    }
    if (task->m_service)
    {
        throw TaskScheduledException();
    }
    task->m_service = this;
    if(!task->m_id)
    {
        ++m_task_id;
        task->m_id = m_task_id;
    }
    m_tasks.emplace(task);
}

void TaskService::remove(Task* task)
{
    if (!task || task->m_service != this)
    {
        return;
    }
    task->m_service = nullptr;
    task->m_canceled = true;
    m_tasks.erase(task);
}

void TaskService::execute_first()
{
    Task& task = **m_tasks.begin();
    m_tasks.erase(m_tasks.begin());
    task.m_executed = true;
    task.m_service = nullptr;
    if (task.m_task)
    {
        task.m_task();
    }
}

void TaskService::clear()
{
    for (auto& task : m_tasks)
    {
        task->m_service = nullptr;
        task->m_canceled = true;
    }
    m_tasks.clear();
}

pt::ptime TaskService::datetime() const
{
    if (!m_tasks.empty())
    {
        return (**m_tasks.begin()).m_datetime;
    }
    else
    {
        return pt::max_date_time;
    }
}

}
}
