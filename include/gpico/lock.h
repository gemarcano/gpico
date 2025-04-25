// SPDX-License-Identifier: GPL-2.0-or-later OR LGPL-2.1-or-later
// SPDX-FileCopyrightText: Gabriel Marcano, 2025
/// @file

#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>

namespace gpico
{

class mutex
{
public:
	mutex(const mutex&) = delete;
	mutex& operator=(const mutex&) = delete;

	mutex()
	{
		handle = xSemaphoreCreateMutexStatic(&storage);
	}

	void lock()
	{
		xSemaphoreTake(handle, portMAX_DELAY);
	}

	bool try_lock()
	{
		return xSemaphoreTake(handle, 0);
	}

	void unlock()
	{
		xSemaphoreGive(handle);
	}
private:
	StaticSemaphore_t storage;
	SemaphoreHandle_t handle;
};

template <class T>
class unique_lock
{
public:
	unique_lock(T& mutex)
	:mutex_(&mutex)
	{
		mutex_->lock();
	}

	unique_lock(T&& lock)
	:mutex(&lock.mutex)
	{
		lock.mutex_ = nullptr;
	}

	~unique_lock()
	{
		release();
	}

	unique_lock& operator=(T& mutex__)
	{
		if (mutex_)
		{
			mutex_->unlock();
		}

		mutex_ = &mutex__;
		mutex_->lock();
	}

	unique_lock& operator=(unique_lock&& lock)
	{
		mutex_ = lock.mutex_;
		lock.mutex_ = nullptr;
		return *this;
	}

	void lock()
	{
		mutex_->lock();
	}

	void unlock()
	{
		mutex_->unlock();
	}

	bool try_lock()
	{
		return mutex_->try_lock();
	}

	void release()
	{
		if (mutex_)
		{
			mutex_->unlock();
			mutex_ = nullptr;
		}
	}

private:
	T* mutex_;
};

}
