//
// Created by xch on 25-8-13.
//

#include "mutex.h"

#include <stdexcept>

namespace sylar {

sylar::Semaphore::Semaphore(uint32_t count)
{
    if (sem_init(&sem_, 0, count)) {
        throw std::logic_error("sem_init error");
    }
}

sylar::Semaphore::~Semaphore()
{
    sem_destroy(&sem_);
}

void Semaphore::wait()
{
    if (sem_wait(&sem_)) {
        throw std::logic_error("sem_wait error");
    }
}

void sylar::Semaphore::notify()
{
    if (sem_post(&sem_)) {
        throw std::logic_error("sem_post error");
    }
}

}   // namespace sylar


namespace sylar {

Mutex::Mutex()
{
    if (pthread_mutex_init(&mutex_, nullptr)) {
        throw std::logic_error("mutex_init error");
    }
}

Mutex::~Mutex()
{
    pthread_mutex_destroy(&mutex_);
}

void Mutex::lock()
{
    if (pthread_mutex_lock(&mutex_)) {
        throw std::logic_error("mutex_lock error");
    }
}

void Mutex::unlock()
{
    if (pthread_mutex_unlock(&mutex_)) {
        throw std::logic_error("mutex_unlock error");
    }
}

}   // namespace sylar


namespace sylar {

RWMutex::RWMutex()
{
    if (pthread_rwlock_init(&mutex_, nullptr)) {
        throw std::logic_error("pthread_rwlock_init error");
    }
}

RWMutex::~RWMutex()
{
    pthread_rwlock_destroy(&mutex_);
}

void RWMutex::rdlock()
{
    if (pthread_rwlock_rdlock(&mutex_)) {
        throw std::logic_error("pthread_rwlock_rdlock error");
    }
}

void RWMutex::wrlock()
{
    if (pthread_rwlock_wrlock(&mutex_)) {
        throw std::logic_error("pthread_rwlock_wrlock error");
    }
}

void RWMutex::unlock()
{
    if (pthread_rwlock_unlock(&mutex_)) {
        throw std::logic_error("pthread_rwlock_unlock error");
    }
}

}   // namespace sylar


namespace sylar {

Spinlock::Spinlock()
{
    if (pthread_spin_init(&mutex_, 0)) {
        throw std::logic_error("pthread_spin_init error");
    }
}

Spinlock::~Spinlock()
{
    pthread_spin_destroy(&mutex_);
}

void Spinlock::lock()
{
    if (pthread_spin_lock(&mutex_)) {
        throw std::logic_error("pthread_spin_lock error");
    }
}

void Spinlock::unlock()
{
    if (pthread_spin_unlock(&mutex_)) {
        throw std::logic_error("pthread_spin_unlock error");
    }
}

}   // namespace sylar


namespace sylar {

CASLock::CASLock()
{
    mutex_.clear();
}

CASLock::~CASLock() {}

void CASLock::lock()
{
    // 原先为true，返回旧值true，因此会阻塞在此处
    // 原先为false,返回旧值false并设置新值为true,因此跳出循环且阻止其它线程进入
    while (std::atomic_flag_test_and_set_explicit(&mutex_, std::memory_order_acquire));
}

void CASLock::unlock()
{
    std::atomic_flag_clear_explicit(&mutex_, std::memory_order_release);
}

}   // namespace sylar


namespace sylar {

void RWSpinlock::rdlock()
{
    mutex_.lock_shared();
    isRead_ = true;
}

void RWSpinlock::wrlock()
{
    mutex_.lock();
    isRead_ = false;
}

void RWSpinlock::unlock()
{
    if (isRead_) {
        mutex_.unlock_shared();
    }
    else {
        mutex_.unlock();
    }
}

}   // namespace sylar