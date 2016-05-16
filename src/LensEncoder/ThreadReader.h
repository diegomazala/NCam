#ifndef __THREAD_READER_H__
#define __THREAD_READER_H__


#include "FujinonEncoder.h"
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>

class ThreadReader
{
public:

	ThreadReader():
			the_thread(),
			stop_thread(false), 
			data(3, 0),
			port(3)
	{
	}
	
	~ThreadReader()
	{
		Stop();
	}

	bool Start(int port_number)
	{
		
		lensHA22x7.setPortNumber(port_number);
		if (lensHA22x7.initConnection())
		{
			stop_thread = false;
			// This will start the thread. Notice move semantics!
			the_thread = std::thread(&ThreadReader::ThreadMain, this);
		}
		else
		{
			stop_thread = true;
		}

		return !stop_thread;
	}

	void Stop()
	{
		stop_thread = true;
		if (the_thread.joinable()) 
			the_thread.join();
		lensHA22x7.finishConnection();
	}

	bool IsRunning() const
	{
		return !stop_thread;
	}

	void Data(std::vector<int>& encoder_data) 
	{
		std::lock_guard<std::mutex> guard(mutex);
		encoder_data = data;
	}

	void Data(int& zoom, int& focus, int& irirs)
	{
		std::lock_guard<std::mutex> guard(mutex);
		zoom = data[0];
		focus = data[1];
		irirs = data[2];
	}

private:
	
	void ThreadMain()
	{
		while (!stop_thread)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(3));

			std::lock_guard<std::mutex> guard(mutex);
			data[0] = lensHA22x7.getZoomPos();	// Request zoom
			data[1] = lensHA22x7.getFocusPos();	// Request focus
			data[2] = lensHA22x7.getIrisPos();	// Request iris
		}
	}

	std::thread the_thread;
	std::mutex mutex;
	std::atomic<bool> stop_thread; // Super simple thread stopping.
	
	std::vector<int> data;
	int port;
	FujinonEncoder lensHA22x7;
};


#endif // __THREAD_READER_H__
