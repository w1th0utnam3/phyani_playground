#pragma once

#include <atomic>
#include <future>
#include <chrono>
#include <utility>

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
	void processEvents();

	std::future<void> stopEventLoop();
	std::future<void> requestTimestep(double dt);
	std::future<bool> toggleAutomaticTimestepping();
	std::future<bool> toggleAutomaticTimestepping(double timeStretch);

	bool isEventLoopRunning() const;
	bool isAutomaticTimesteppingActive() const;

	std::pair<double, double> lastTimestepStats() const;

private:
	AnimationSystem& m_animationSystem;

	std::atomic<bool> m_continueEventLoop;
	std::atomic<bool> m_automaticTimestepping;
	std::atomic<double> m_timeStretch;
	std::atomic<double> m_lastComputationTime;
	std::atomic<double> m_lastTimestepDt;

	std::chrono::time_point<std::chrono::high_resolution_clock> m_lastRender;
};