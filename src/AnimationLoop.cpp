#include "AnimationLoop.h"

#include <iostream>

AnimationLoop::AnimationLoop(AnimationSystem& animationSystem)
	: m_animationSystem(animationSystem)
	, m_continueEventLoop(false)
	, m_automaticTimestepping(false)
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

std::future<void> AnimationLoop::startAutomaticTimestepping()
{
	return m_eventQueue.postEvent(StartAutomaticTimesteppingRequest());
}

std::future<void> AnimationLoop::stopAutomaticTimestepping()
{
	return m_eventQueue.postEvent(StopAutomaticTimesteppingRequest());
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
			auto currentTime = std::chrono::high_resolution_clock::now();
			m_animationSystem.computeTimestep(static_cast<std::chrono::duration<double>>(currentTime - m_lastRender).count());
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
			if (!simulation->m_automaticTimestepping) simulation->m_animationSystem.computeTimestep(request.dt);
			event.promise.set_value();
		}

		void operator()(StartAutomaticTimesteppingEvent& event) const
		{
			if(!simulation->m_automaticTimestepping)
			{
				simulation->m_automaticTimestepping = true;
				simulation->m_lastRender = std::chrono::high_resolution_clock::now();
			}
			event.promise.set_value();
		}

		void operator()(StopAutomaticTimesteppingEvent& event) const
		{
			simulation->m_automaticTimestepping = false;
			event.promise.set_value();
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

