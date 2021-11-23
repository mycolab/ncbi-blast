#ifndef BMTPOOL__H__INCLUDED__
#define BMTPOOL__H__INCLUDED__
/*
Copyright(c) 2002-2021 Anatoliy Kuznetsov(anatoliy_kuznetsov at yahoo.com)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

For more information please visit:  http://bitmagic.io
*/

#include <type_traits>
#include <queue>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

#include "bmbuffer.h"
#include "bmtask.h"

namespace bm
{


/// Pad 60 bytes so that the final  ocupiles 64 bytes (1 cache line)
/// @internal
struct pad60_struct { char c[60]; };
/// Empty padding
/// @internal
struct pad0_struct {  };

/**
    Spin-lock with two-phase acquire (read + cas)
    padding parameter optionally adds a buffer to avoid CPU cache
    line contention.
    TODO: test if padding realy helps in our case

    Generally spin_lock does not have advantage over std::mutex
    but in some specific cases like WebAssembly it may be better
    due no "noexcept" property

    @ingroup bmtasks
 */
template<class Pad = bm::pad0_struct>
class spin_lock
{
public:
    spin_lock() noexcept : locked_(0) {}

    /// Lock the lock
    void lock() noexcept
    {
        while(1) // spin loop
        {
            unsigned locked = locked_.load(std::memory_order_relaxed);
            if (!locked &&
                 locked_.compare_exchange_weak(locked, true,
                                               std::memory_order_acquire))
                break;
#if defined(BMSSE2OPT) || defined(BMSSE42OPT) || defined(BMAVX2OPT) || defined(BMAVX512OPT)
            _mm_pause();
#endif
        } // while
    }

    /// Try to acquire the lock, return true if successfull
    ///
    bool try_lock() noexcept
    {
        unsigned locked = locked_.load(std::memory_order_relaxed);
        if (!locked &&
             locked_.compare_exchange_weak(locked, true,
                                           std::memory_order_acquire))
            return true;
        return false;
    }

    /// Unlock the lock
    void unlock() noexcept
    {
        locked_.store(false, std::memory_order_release);
    }
private:
    spin_lock(const spin_lock&)=delete;
    spin_lock& operator=(const spin_lock&)=delete;

private:
    std::atomic<unsigned> locked_;
    Pad p_;
};

/// Wait for multiple threads to exit
///
/// @internal
/// @ingroup bmtasks
///
template<typename TCont>
void join_multiple_threads(TCont& tcont)
{
    typename TCont::iterator it_end = tcont.end();
    for (typename TCont::iterator it = tcont.begin(); it != it_end; ++it)
    {
        if (it->joinable())
            it->join();
    } // for it
}


template<typename QValue, typename Lock> class thread_pool;


/**
    Thread-sync queue with MT access protecion

    @ingroup bmtasks
    @internal
 */
template<typename Value, typename Lock>
class queue_sync
{
public:
    typedef Value     value_type;
    typedef Lock      lock_type;

    /// constructor
    ///
    queue_sync() noexcept {}

    /// Push value to the back of the queue
    /// @param v - value to put in the queue
    ///
    /// @sa push_no_lock
    ///
    void push(const value_type& v) //noexcept(bm::is_lock_noexcept<lock_type>::value)
    {
        {
            std::lock_guard<lock_type> lg(dq_lock_);
            data_queue_.push(v);
        }
        queue_push_cond_.notify_one(); // noexcept
    }

    /// Push value to the back of the queue without lock protection
    /// It is assumed that processing did not start and we are just staging
    /// the batch
    ///
    /// @param v - value to put in the queue
    /// @sa push
    ///
    void push_no_lock(const value_type& v)
    {
        data_queue_.push(v);
    }


    /// Extract value
    /// @param v - [out] value returned
    /// @return true if extracted
    ///
    bool try_pop(value_type& v)
    {
        std::lock_guard<lock_type> guard(dq_lock_);
        if (data_queue_.empty())
            return false;
        v = data_queue_.front();
        data_queue_.pop();
        return true;
    }

    /// @return true if empty
    bool empty() const //noexcept(bm::is_lock_noexcept<lock_type>::value)
    {
        std::lock_guard<lock_type> guard(dq_lock_);
        return data_queue_.empty();
    }

    /// lock the queue access
    /// @sa push_no_lock, unlock
    void lock() noexcept(bm::is_lock_noexcept<lock_type>::value)
        { dq_lock_.lock(); }

    /// Try to lock the queue exclusively
    ///
    bool try_lock() noexcept(bm::is_lock_noexcept<lock_type>::value)
        { return dq_lock_.try_lock(); }

    /// unlock the queue access
    /// @sa push_no_lock, lock
    void unlock() noexcept(bm::is_lock_noexcept<lock_type>::value)
    {
        dq_lock_.unlock();
        // lock-unlock is done to protect bulk push, need to wake up
        // all waiting workers
        queue_push_cond_.notify_all(); // noexcept
    }

    template<typename QV, typename L> friend class bm::thread_pool;

protected:
    typedef std::queue<value_type>     queue_type;

private:
    queue_sync(const queue_sync&) = delete;
    queue_sync& operator=(const queue_sync&) = delete;
private:
    queue_type                data_queue_; ///< queue object
    mutable lock_type         dq_lock_;    ///< lock for queue

    // signal structure for wait on empty queue
protected:
    mutable std::mutex        signal_mut_; ///< signal mutex for q submissions
    std::condition_variable   queue_push_cond_;   ///< mutex paired conditional
};


/**
    Thread pool with custom (thread safe) queue 

    Thread pool implements a busy-wait task stealing 
    design pattern

    QValue - task queue value parameter
    Lock   - locking protection type (like std::mutex or spinlock)    
    
    @ingroup bmtasks
*/
template<typename QValue, typename Lock>
class thread_pool
{
public:
    typedef QValue    value_type;
    typedef Lock      lock_type;
    typedef bm::queue_sync<QValue, lock_type> queue_type;

    /**
        Stop modes for threads:
        0 - keep running/waiting for jobs
        1 - wait for empty task queue then stop threads
        2 - stop threads now even if there are pending tasks
    */
    enum stop_mode
    {
        no_stop = 0,          ///< keep spinning on busy-wait
        stop_when_done = 1,   ///< stop if tsak queue is empty
        stop_now = 2          ///< stop right now
    };

public:
    thread_pool(stop_mode sm = no_stop)
        : stop_flag_(sm)
    {}

    ~thread_pool()
    {
        int is_stop = stop_flag_;
        if (!is_stop) // finish the outstanding jobs and close threads
            set_stop_mode(stop_when_done);
        join();
    }

    /** Setup the criteria for threads shutdown
        Also notifies all threads on a new directive
        @param sm - stop mode
    */
    void set_stop_mode(stop_mode sm) noexcept
    {
        stop_flag_ = sm;
        job_queue_.queue_push_cond_.notify_all(); // this is noexcept
    }

    /**
        Request an immediate stop of all threads in the pool
     */
    void stop() noexcept { set_stop_mode(stop_now); }

    /**
        Start thread pool worker threads.
        @param tcount - number of threads to start
     */
    void start(unsigned tcount)
    {
        int is_stop = stop_flag_.load(std::memory_order_relaxed);
        if (is_stop == stop_now) // immediate stop requested
            return;
        // TODO: consider lock protect of thread_vect_ member
        for(unsigned i = 0;i < tcount; ++i)
        {
            thread_vect_.emplace_back(
                            std::thread(&thread_pool::worker_func,this));
        } // for
    }

    /**
        Wait for threads to stop
    */
    void join()
    {
        bm::join_multiple_threads(thread_vect_);
        thread_vect_.resize(0);
    }

    /**
        Conditional spin-wait for the queue to empty
     */
     void wait_empty_queue()
     {
        const std::chrono::duration<int, std::milli> wait_duration(20);
        while(1)
        {
            if (job_queue_.empty())
            {
                break;
            }
            std::cv_status wait_res;
            {
                std::unique_lock<std::mutex> lk(task_done_mut_);
                wait_res = task_done_cond_.wait_for(lk, wait_duration);
            }
            if (wait_res == std::cv_status::timeout)
            {
                std::this_thread::yield();
            }
        } // while
     }

    /// Get access to the job submission queue
    ///
    queue_type& get_job_queue() noexcept { return job_queue_; }

protected:

    /// Internal worker wrapper with busy-wait spin loop
    /// making pthread-like call for tasks
    ///
    void worker_func()
    {
        const std::chrono::duration<int, std::milli> wait_duration(10);
        while(1)
        {
            int is_stop = stop_flag_.load(std::memory_order_relaxed);
            if (is_stop == stop_now) // immediate stop requested
                break;

            bm::task_description* task_descr;
            if (job_queue_.try_pop(task_descr))
            {
                // TODO: consider try-catch here
                task_descr->ret = task_descr->func(task_descr->argp);
                task_descr->done = 1;

                task_done_cond_.notify_one();
                continue;
            }
            // queue appears to be empty, check if requested to stop
            //
            is_stop = stop_flag_.load(std::memory_order_relaxed);
            if (is_stop)
                return;

            // enter a temporal condition wait
            //   notifications are treated as unreliable re-verified
            //   via spin over the poll of the queue
            std::cv_status wait_res;
            {
                std::unique_lock<std::mutex> lk(job_queue_.signal_mut_);
                wait_res =
                    job_queue_.queue_push_cond_.wait_for(lk, wait_duration);
            }
            if (wait_res == std::cv_status::timeout)
            {
                is_stop = stop_flag_.load(std::memory_order_relaxed);
                if (is_stop == stop_now) // immediate stop requested
                    return;

                std::this_thread::yield();
            }
        } // while
        return;
    }


private:
    thread_pool(const thread_pool&)=delete;
    thread_pool& operator=(const thread_pool&)=delete;

private:
    queue_type                job_queue_;    ///< queue (thread sync)
    std::vector<std::thread>  thread_vect_;  ///< threads servicing queue
    std::atomic<int>          stop_flag_{0}; ///< stop flag to all threads

    // notification channel for results wait
    mutable std::mutex        task_done_mut_; ///< signal mutex for task done
    std::condition_variable   task_done_cond_;///< mutex paired conditional

};

/**
    Utility class to submit task batch to the running thread pool
    and optionally wait for it getting done
    
    @ingroup bmtasks
 */
template<typename TPool>
class thread_pool_executor
{
public:
    typedef TPool thread_pool_type;
    typedef task_batch_base::size_type size_type;

public:
    thread_pool_executor()
    {}

    void run(thread_pool_type&    tpool,
             bm::task_batch_base& tasks,
             bool                  wait_for_batch)
    {
        typename thread_pool_type::queue_type& qu = tpool.get_job_queue();
 
        //qu.lock();

        task_batch_base::size_type batch_size = tasks.size();
        for (task_batch_base::size_type i = 0; i < batch_size; ++i)
        {
            bm::task_description* tdescr = tasks.get_task(i);
            tdescr->argp = tdescr; // restore the self referenece

            // check if this is a barrier call
            if (tdescr->flags != bm::task_description::no_flag && i > 0)
            {
                //qu.unlock();

                // barrier task
                //   wait until all previously scheduled tasks are done
                tpool.wait_empty_queue();
                wait_for_batch_done(/*tpool,*/ tasks, 0, batch_size - 1);

                // run the barrier proc on the curent thread
                tdescr->ret = tdescr->func(tdescr->argp);
                tdescr->done = 1;

                // re-read the batch size, if barrier added more tasks
                task_batch_base::size_type new_batch_size = tasks.size();
                if (new_batch_size != batch_size)
                {
                    batch_size = new_batch_size;
                    //qu.lock(); // re-lock
                }
                continue;
            }

            qu.push(tdescr); // locked push to the thread queue
            //qu.push_no_lock(tdescr);
        } // for

        //qu.unlock();

        // implicit wait barrier for all tasks
        if (wait_for_batch && batch_size)
        {
            tpool.wait_empty_queue();
            wait_for_batch_done(/*tpool,*/ tasks, 0, batch_size - 1);
        }
    }

    /**
        Check if all batch jobs in the specified interval are done
        Spin wait if not.
    */
    static
    void wait_for_batch_done(//thread_pool_type&          tpool,
                             bm::task_batch_base&       tasks,
                             task_batch_base::size_type from_idx,
                             task_batch_base::size_type to_idx)
    {
        BM_ASSERT(from_idx <= to_idx);
        BM_ASSERT(to_idx < tasks.size());

        for (task_batch_base::size_type i = from_idx; i <= to_idx; ++i)
        {
            const bm::task_description* tdescr = tasks.get_task(i);
            unsigned done = tdescr->done.load(std::memory_order_relaxed);
            while (!done)
            {
                std::this_thread::yield();
                // TODO: subscribe to a conditional wait for job done in tpool
                done = tdescr->done.load(std::memory_order_relaxed);
            } // while
        } // for 
    }

private:
    thread_pool_executor(const thread_pool_executor&) = delete;
    thread_pool_executor& operator=(const thread_pool_executor&) = delete;
};


} // bm

#endif
