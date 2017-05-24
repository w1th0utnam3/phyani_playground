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
	struct StartAutomaticTimesteppingRequest {};
	struct StopAutomaticTimesteppingRequest {};

	using StopEventLoopEvent = VoidEvent<StopEventLoopRequest>;
	using ComputeTimestepEvent = VoidEvent<ComputeTimestepRequest>;
	using StartAutomaticTimesteppingEvent = VoidEvent<StartAutomaticTimesteppingRequest>;
	using StopAutomaticTimesteppingEvent = VoidEvent<StopAutomaticTimesteppingRequest>;

	using event_queue_type = EventQueue<StopEventLoopEvent, 
										ComputeTimestepEvent, 
										StartAutomaticTimesteppingEvent, 
										StopAutomaticTimesteppingEvent>;
	event_queue_type m_eventQueue;

public:
	AnimationLoop(AnimationSystem& animationSystem);
	void executeTimestepLoop();

	std::future<void> stopEventLoop();
	std::future<void> requestTimestep(double dt);
	std::future<void> startAutomaticTimestepping();
	std::future<void> stopAutomaticTimestepping();

	void processEvents();

	bool isEventLoopRunning() const;
	bool isAutomaticTimesteppingActive() const;

private:
	AnimationSystem& m_animationSystem;

	std::atomic<bool> m_continueEventLoop;
	std::atomic<bool> m_automaticTimestepping;

	std::chrono::time_point<std::chrono::high_resolution_clock> m_lastRender;
};