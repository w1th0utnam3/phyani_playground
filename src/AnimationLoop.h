#pragma once

#include <atomic>
#include <future>
#include <chrono>

#include "EventQueue.h"
#include "AnimationSystem.h"

class AnimationLoop
{
	struct StopEventLoopRequest {};
	struct ComputeTimestepRequest { double dt; };
	struct ToggleAutomaticTimesteppingRequest { double timeStretch; };

	using StopEventLoopEvent = VoidEvent<StopEventLoopRequest>;
	using ComputeTimestepEvent = VoidEvent<ComputeTimestepRequest>;
	using ToggleAutomaticTimesteppingEvent = Event<ToggleAutomaticTimesteppingRequest, bool>;

	using event_queue_type = EventQueue<StopEventLoopEvent, 
										ComputeTimestepEvent, 
										ToggleAutomaticTimesteppingEvent>;
	event_queue_type m_eventQueue;

public:
	AnimationLoop(AnimationSystem& animationSystem);
	void executeTimestepLoop();

	std::future<void> stopEventLoop();
	std::future<void> requestTimestep(double dt);
	std::future<bool> toggleAutomaticTimestepping();
	std::future<bool> toggleAutomaticTimestepping(double timeStretch);

	void processEvents();

	bool isEventLoopRunning() const;
	bool isAutomaticTimesteppingActive() const;

private:
	AnimationSystem& m_animationSystem;

	std::atomic<bool> m_continueEventLoop;
	std::atomic<bool> m_automaticTimestepping;
	std::atomic<double> m_timeStretch;

	std::chrono::time_point<std::chrono::high_resolution_clock> m_lastRender;
};