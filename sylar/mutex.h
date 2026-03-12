#pragma once


#include "sylar/noncopyable.h"

#include <atomic>
#include <cstdint>
#include <pthread.h>
#include <semaphore.h>
#include <tbb/spin_rw_mutex.h>

namespace sylar {

class Semaphore : sylar::NonCopyable {
public:
    explicit Semaphore(uint32_t count = 0);
    ~Semaphore();
    void wait();
    void notify();

private:
    sem_t sem_;
};


template<typename T> struct ScopedLockImpl
{
public:
    explicit ScopedLockImpl(T& mutex)
        : mutex_(mutex)
    {
        mutex_.lock();
        locked_ = true;
    }

    ~ScopedLockImpl() { unlock(); }

    void lock()
    {
        if (!locked_) {
            mutex_.lock();
            locked_ = true;
        }
    }

    void unlock()
    {
        if (locked_) {
            mutex_.unlock();
            locked_ = false;
        }
    }

private:
    T& mutex_;
    bool locked_;
};


template<typename T> struct ReadScopedLockImpl
{
public:
    explicit ReadScopedLockImpl(T& mutex)
        : mutex_(mutex)
    {
        mutex_.rdlock();
        locked_ = true;
    }

    ~ReadScopedLockImpl() { unlock(); }

    void lock()
    {
        if (!locked_) {
            mutex_.rdlock();
            locked_ = true;
        }
    }

    void unlock()
    {
        if (locked_) {
            mutex_.unlock();
            locked_ = false;
        }
    }

private:
    T& mutex_;
    bool locked_;
};

template<typename T> struct WriteScopedLockImpl
{
public:
    explicit WriteScopedLockImpl(T& mutex)
        : mutex_(mutex)
    {
        mutex_.wrlock();
        locked_ = true;
    }

    ~WriteScopedLockImpl() { unlock(); }

    void lock()
    {
        if (!locked_) {
            mutex_.wrlock();
            locked_ = true;
        }
    }

    void unlock()
    {
        if (locked_) {
            mutex_.unlock();
            locked_ = false;
        }
    }

private:
    T& mutex_;
    bool locked_;
};


class Mutex : NonCopyable {
public:
    using Lock = ScopedLockImpl<Mutex>;
    Mutex();
    ~Mutex();
    void lock();
    void unlock();

private:
    pthread_mutex_t mutex_;
};


class NullMutex : NonCopyable {
public:
    using Lock = ScopedLockImpl<Mutex>;

public:
    NullMutex() = default;
    ~NullMutex() = default;
    void lock() {}
    void unlock() {}
};

class RWMutex : NonCopyable {
public:
    using ReadLock = ReadScopedLockImpl<RWMutex>;
    using WriteLock = WriteScopedLockImpl<RWMutex>;

public:
    RWMutex();
    ~RWMutex();
    void rdlock();
    void wrlock();
    void unlock();

private:
    pthread_rwlock_t mutex_;
};


class NullRWMutex : NonCopyable {
public:
    using ReadLock = ReadScopedLockImpl<NullMutex>;
    using WriteLock = WriteScopedLockImpl<NullMutex>;

public:
    NullRWMutex() = default;
    ~NullRWMutex() = default;
    void rdlock() {}
    void wrlock() {}
    void unlock() {}
};


class Spinlock : NonCopyable {
public:
    using Lock = ScopedLockImpl<Spinlock>;

public:
    Spinlock();
    ~Spinlock();
    void lock();
    void unlock();

private:
    pthread_spinlock_t mutex_;
};

class CASLock : NonCopyable {
public:
    using Lock = ScopedLockImpl<CASLock>;

public:
    CASLock();
    ~CASLock();
    void lock();
    void unlock();

private:
    volatile std::atomic_flag mutex_;
};

class RWSpinlock : NonCopyable {
public:
    using ReadLock = ReadScopedLockImpl<RWSpinlock>;
    using WriteLock = WriteScopedLockImpl<RWSpinlock>;

public:
    RWSpinlock() = default;
    ~RWSpinlock() = default;
    void rdlock();
    void wrlock();
    void unlock();

private:
    tbb::spin_rw_mutex mutex_;
    bool isRead_;
};

}   // namespace sylar
