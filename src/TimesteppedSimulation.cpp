#include "TimesteppedSimulation.h"

#include <iostream>

TimesteppedSimulation::TimesteppedSimulation()
	: m_continueEventLoop(false)
	, m_time(0.0)
{
}

void TimesteppedSimulation::simulateTimestep(double dt)
{
	m_time = m_time + dt;
	std::cout << m_time << "\n";
}

std::future<void> TimesteppedSimulation::stopTimestepLoop()
{
	return m_eventQueue.postEvent(StopTimestepLoopRequest());
}

std::future<void> TimesteppedSimulation::requestTimestep(double dt)
{
	return m_eventQueue.postEvent(ComputeTimestepRequest{ dt });
}

void TimesteppedSimulation::executeTimestepLoop()
{
	if (m_continueEventLoop) return;

	m_continueEventLoop = true;
	while(m_continueEventLoop) {
		m_eventQueue.waitForEvent();
		processEvents();
	}
}

void TimesteppedSimulation::processEvents()
{
	// Define the visitor which processes all possible event types
	const struct
	{
		TimesteppedSimulation* simulation;

		void operator()(StopTimestepLoopEvent& event) const
		{
			// Change the flag to break event loop
			simulation->m_continueEventLoop = false;
			event.promise.set_value();
		}

		void operator()(ComputeTimestepEvent& event) const
		{
			auto& request = event.request;
			simulation->simulateTimestep(request.dt);
			event.promise.set_value();
		}
	} eventVisitor{ this };

	while (m_continueEventLoop && !m_eventQueue.empty()) {
		// The event queue ensures thread safety
		m_eventQueue.processOldestEvent(eventVisitor);
	}
}