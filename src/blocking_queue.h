
//==============================================================================
//
//   blocking_queue.h - exported_h
//
//==============================================================================
//  Copyright (C) 2024 Luminator Tech. Group  <guillaume.plante@luminator.com>
//==============================================================================


#ifndef __BLOCKINGQUEUE_H__
#define __BLOCKINGQUEUE_H__

#include <condition_variable>
#include <mutex>
#include <thread>
#include <queue>
#include <string>
#include <iostream>
#include <sstream>

template <typename T>
class BlockingQueue {
public:
	BlockingQueue() {}
	BlockingQueue(BlockingQueue<T>&& bq);
	BlockingQueue<T>& operator=(BlockingQueue<T>&& bq);
	BlockingQueue(const BlockingQueue<T>&) = delete;
	BlockingQueue<T>& operator=(const BlockingQueue<T>&) = delete;
	T deQ();
	void enQ(const T& t);
	T& front();
	void clear();
	size_t size();
private:
	std::queue<T> q_;
	std::mutex mtx_;
	std::condition_variable cv_;
};

template<typename T>
BlockingQueue<T>::BlockingQueue(BlockingQueue<T>&& bq) // need to lock so can't initialize
{
	std::lock_guard<std::mutex> l(mtx_);
	q_ = bq.q_;
	while (bq.q_.size() > 0)  // clear bq
		bq.q_.pop();
	
}

template<typename T>
BlockingQueue<T>& BlockingQueue<T>::operator=(BlockingQueue<T>&& bq)
{
	if (this == &bq) return *this;
	std::lock_guard<std::mutex> l(mtx_);
	q_ = bq.q_;
	while (bq.q_.size() > 0)  // clear bq
		bq.q_.pop();
	// can't move assign mutex or condition variable so use target's 
	return *this;
}

template<typename T>
T BlockingQueue<T>::deQ()
{
	std::unique_lock<std::mutex> l(mtx_);
	/*
	This lock type is required for use with condition variables.
	The operating system needs to lock and unlock the mutex:
	- when wait is called, below, the OS suspends waiting thread
	and releases lock.
	- when notify is called in enQ() the OS relocks the mutex,
	resumes the waiting thread and sets the condition variable to
	signaled state.
	std::lock_quard does not have public lock and unlock functions.
	*/
	if (q_.size() > 0)
	{
		T temp = q_.front();
		q_.pop();
		return temp;
	}
	// may have spurious returns so loop on !condition

	while (q_.size() == 0)
		cv_.wait(l, [this]() { return q_.size() > 0; });
	T temp = q_.front();
	q_.pop();
	return temp;
}

template<typename T>
void BlockingQueue<T>::enQ(const T& t)
{
	{
		std::unique_lock<std::mutex> l(mtx_);
		q_.push(t);
	}
	cv_.notify_one();
}

template <typename T>
T& BlockingQueue<T>::front()
{
	std::lock_guard<std::mutex> l(mtx_);
	if (q_.size() > 0)
		return q_.front();
	throw std::exception("attempt to deQue empty queue");
}

template <typename T>
void BlockingQueue<T>::clear()
{
	std::lock_guard<std::mutex> l(mtx_);
	while (q_.size() > 0)
		q_.pop();
}

template<typename T>
size_t BlockingQueue<T>::size()
{
	std::lock_guard<std::mutex> l(mtx_);
	return q_.size();
}



#endif //__BLOCKINGQUEUE_H__
