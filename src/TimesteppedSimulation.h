#pragma once

#include <atomic>
#include <future>

#include "EventQueue.h"

class TimesteppedSimulation
{
	struct StopTimestepLoopRequest {};
	struct ComputeTimestepRequest { double dt; };

	using StopTimestepLoopEvent = VoidEvent<StopTimestepLoopRequest>;
	using ComputeTimestepEvent = VoidEvent<ComputeTimestepRequest>;

	using event_queue_type = EventQueue<StopTimestepLoopEvent, ComputeTimestepEvent>;
	event_queue_type m_eventQueue;

public:
	TimesteppedSimulation();

	void simulateTimestep(double dt);

	std::future<void> stopTimestepLoop();
	std::future<void> requestTimestep(double dt);

	void executeTimestepLoop();
	void processEvents();

private:
	std::atomic<bool> m_continueEventLoop;
	std::atomic<double> m_time;
};