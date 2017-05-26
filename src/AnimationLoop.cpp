#include "AnimationLoop.h"

#include <iostream>

AnimationLoop::AnimationLoop(AnimationSystem& animationSystem)
	: m_animationSystem(animationSystem)
	, m_continueEventLoop(false)
	, m_automaticTimestepping(false)
	, m_timeStretch(1.0)
	, m_lastComputationTime(0.0)
	, m_lastTimestepDt(0.0)
{
}

std::future<void> AnimationLoop::stopEventLoop()
{
	return m_eventQueue.postEvent(StopEventLoopRequest());
}

std::future<void> AnimationLoop::requestTimestep(double dt)
{
	return m_eventQueue.postEvent(ComputeTimestepRequest{ dt });
}

std::future<bool> AnimationLoop::toggleAutomaticTimestepping()
{
	return m_eventQueue.postEvent(ToggleAutomaticTimesteppingRequest{ 1.0 });
}

std::future<bool> AnimationLoop::toggleAutomaticTimestepping(double timeStretch)
{
	return m_eventQueue.postEvent(ToggleAutomaticTimesteppingRequest{ timeStretch });
}

void AnimationLoop::executeTimestepLoop()
{
	if (m_continueEventLoop) return;

	std::cout << "(sim) Timestep loop started." << "\n";
	m_continueEventLoop = true;
	while(m_continueEventLoop) {
		if (!m_automaticTimestepping) {
			m_eventQueue.waitForEvent();
		}

		processEvents();
		if (m_automaticTimestepping) {
			processEvents();
			const auto currentTime = std::chrono::high_resolution_clock::now();
			m_lastComputationTime = static_cast<std::chrono::duration<double>>(currentTime - m_lastRender).count();
			m_lastTimestepDt = m_timeStretch*m_lastComputationTime;
			m_animationSystem.computeTimestep(m_lastTimestepDt);
			m_lastRender = currentTime;
		}
	}
	std::cout << "(sim) Timestep loop stopped." << "\n";
}

void AnimationLoop::processEvents()
{
	// Define the visitor which processes all possible event types
	const struct
	{
		AnimationLoop* simulation;

		void operator()(StopEventLoopEvent& event) const
		{
			// Change the flag to break event loop
			simulation->m_continueEventLoop = false;
			event.promise.set_value();
		}

		void operator()(ComputeTimestepEvent& event) const
		{
			auto& request = event.request;
			if (!simulation->m_automaticTimestepping) {
				simulation->m_lastRender = std::chrono::high_resolution_clock::now();
				simulation->m_lastTimestepDt = request.dt;
				simulation->m_animationSystem.computeTimestep(request.dt);
				const auto currentTime = std::chrono::high_resolution_clock::now();
				simulation->m_lastComputationTime = static_cast<std::chrono::duration<double>>(currentTime - simulation->m_lastRender).count();
			}
			event.promise.set_value();
		}

		void operator()(ToggleAutomaticTimesteppingEvent& event) const
		{
			if(!simulation->m_automaticTimestepping) {
				auto& request = event.request;
				simulation->m_automaticTimestepping = true;
				simulation->m_timeStretch = request.timeStretch;
				simulation->m_lastRender = std::chrono::high_resolution_clock::now();
				event.promise.set_value(true);
			} else {
				simulation->m_automaticTimestepping = false;
				event.promise.set_value(false);
			}
		}
	} eventVisitor{ this };

	while (m_continueEventLoop && !m_eventQueue.empty()) {
		// The event queue ensures thread safety
		m_eventQueue.processOldestEvent(eventVisitor);
	}
}

bool AnimationLoop::isEventLoopRunning() const
{
	return m_continueEventLoop;
}

bool AnimationLoop::isAutomaticTimesteppingActive() const
{
	return m_automaticTimestepping;
}

std::pair<double, double> AnimationLoop::lastTimestepStats() const
{
	return { m_lastComputationTime , m_lastTimestepDt };
}
