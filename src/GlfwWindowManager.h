﻿#pragma once

#include <thread>
#include <queue>
#include <future>
#include <mutex>
#include <variant>
#include <memory>

#include <GLFW/glfw3.h>

class GlfwWindowManager
{
public:
	//! Unique pointer type returned by the static create() method.
	using GlfwWindowManagerUniquePtr = std::unique_ptr<GlfwWindowManager, void(*)(GlfwWindowManager*)>;

	//! Creates and returns the GlfwWindowManager singleton.
	/*!
	 * Initializes the GlfwWindowManager singleton and returns a unique pointer to it. When leaving the scope of the pointer, the
	 * GLFW context is deinitialized. The window manager can only be created from the main thread. there can be only one window
	 * manager at a time.
	 * \param throwOnFailure Whether this method should throw a runtime_error when a critical error occurs (e.g. glfwInit() fails).
	 * \return Unique pointer of the singleton. Used to indicate the lifetime of the GLFW context.
	 */
	static GlfwWindowManagerUniquePtr create(bool throwOnFailure = true);

	//! Returns whether the GlfWindowManager is currently initialized.
	static bool isInitialized();
	//! Processes all events that were posted to the event queue of the window manager.
	static void processEvents();
	//! Starts a blocking event loop that waits fro GLFW events.
	static void startEventLoop();

	//! Posts an event to exit the event loop.
	static std::future<void> exitEventLoop();
	//! Posts an event to create a new window using glfwCreateWindow().
	static std::future<GLFWwindow*> requestWindow(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share);
	//! Posts an event to destroy the specified window using glfwDestroyWindow().
	static std::future<void> destroyWindow(GLFWwindow* window);

private:
	//! Constructs a GlfwWindowManager and initializes GLFW. Use the static create() method instead.
	GlfwWindowManager(bool throwOnFailure = true);
	//! Destroys the GlfwWindowManager and terminates GLFW. Lifetime is managed by the unique pointer returned by create().
	~GlfwWindowManager();
	//! Deletes the supplied GlfwWindowManager. Used as a deleter for the unqiue pointer returned by create().
	static void deleter(GlfwWindowManager* manager);
	//! Prints the supplied error. Used as a callback for GLFW errors.
	static void errorCallback(int error, const char* description);

	//! Flag indicating whether GLFW is currently initialized.
	static bool m_initialized;
	//! Flag used to indicate whether the event loop should continue or an exit event was received.
	static bool m_continueEventLoop;

	//! Thread id of the thread that initialized the global, static variables.
	static std::thread::id m_mainThreadId;
	//! Returns whether the current thread id is the same as the id of the thread used to initialize global, static variables.
	static bool isMainThread();

	template <typename PromisedT, typename EventT>
	struct EventTask
	{
		using promised_type = PromisedT;
		using event_type = EventT;

		std::promise<PromisedT> promise;
		EventT event;
	};

	struct ExitEvent { };

	struct CreateWindowEvent
	{
		int width;
		int height;
		const char* title;
		GLFWmonitor* monitor;
		GLFWwindow* share;
	};

	struct DestroyWindowEvent
	{
		GLFWwindow* window;
	};

	using ExitEventRequest = EventTask<void, ExitEvent>;
	using CreateWindowEventRequest = EventTask<GLFWwindow*, CreateWindowEvent>;
	using DestroyWindowEventRequest = EventTask<void, DestroyWindowEvent>;

	using EventRequestVariant = std::variant<ExitEventRequest,
	                                         CreateWindowEventRequest,
	                                         DestroyWindowEventRequest>;

	//! Mutex for the event queue.
	static std::mutex m_eventQueueMutex;
	//! The queue used to store events.
	static std::queue<EventRequestVariant> m_eventQueue;
};
