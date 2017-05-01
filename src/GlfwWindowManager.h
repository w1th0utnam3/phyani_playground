#pragma once

#include <atomic>
#include <thread>
#include <future>
#include <memory>

#include <GLFW/glfw3.h>

#include "EventQueue.h"

struct ExitRequest {};

struct CreateWindowRequest
{
	int width;
	int height;
	const char* title;
	GLFWmonitor* monitor;
	GLFWwindow* share;
};

struct DestroyWindowRequest
{
	GLFWwindow* window;
};

template <typename CallbackT, std::size_t id = 0>
struct SetCallbackRequest
{
	GLFWwindow* window;
	CallbackT cbfun;
};

using SetMouseButtonCallbackRequest = SetCallbackRequest<GLFWmousebuttonfun>;
using SetCursorPosCallbackRequest = SetCallbackRequest<GLFWcursorposfun>;
using SetScrollCallbackRequest = SetCallbackRequest<GLFWscrollfun, 1>;
using SetKeyCallbackRequest = SetCallbackRequest<GLFWkeyfun>;
using SetCharCallbackRequest = SetCallbackRequest<GLFWcharfun>;
using SetWindowSizeCallbackRequest = SetCallbackRequest<GLFWwindowsizefun>;

using ExitEvent = VoidEvent<ExitRequest>;
using CreateWindowEvent = Event<CreateWindowRequest, GLFWwindow*>;
using DestroyWindowEvent = VoidEvent<DestroyWindowRequest>;
using SetMouseButtonCallbackEvent = VoidEvent<SetMouseButtonCallbackRequest>;
using SetCursorPosCallbackEevent = VoidEvent<SetCursorPosCallbackRequest>;
using SetScrollCallbackEvent = VoidEvent<SetScrollCallbackRequest>;
using SetKeyCallbackEvent = VoidEvent<SetKeyCallbackRequest>;
using SetCharCallbackEvent = VoidEvent<SetCharCallbackRequest>;
using SetWindowSizeCallbackEvent = VoidEvent<SetWindowSizeCallbackRequest>;

//! Global window and GLFW manager singleton class.
/*
 * This class can be used as a scoped GLFW manager which initializes GLFW and runs the event loop
 * for all opened GLFW windows. When the manager leaves scope, GLFW is terminated cleanly. Furthermore, 
 * the manager offers asynchronous event methods, allowing GLFW functions which 
 * can be only called safely from the main thread, to be called indirectly from any thread.
 */
class GlfwWindowManager
{
	using event_queue_type = EventQueue<ExitEvent, 
										CreateWindowEvent, 
										DestroyWindowEvent, 
										SetMouseButtonCallbackEvent, 
										SetCursorPosCallbackEevent, 
										SetScrollCallbackEvent, 
										SetKeyCallbackEvent, 
										SetCharCallbackEvent,
										SetWindowSizeCallbackEvent>;
	static event_queue_type m_eventQueue;

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
	static void executeEventLoop();

	//! Posts an event to exit the event loop.
	static std::future<void> exitEventLoop();
	//! Posts an event to create a new window using glfwCreateWindow().
	static std::future<GLFWwindow*> requestWindow(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share);
	//! Posts an event to destroy the specified window using glfwDestroyWindow().
	static std::future<void> destroyWindow(GLFWwindow* window);
	//! Posts an event to set the keyboard callback of the specified window using glfwSetMouseButtonCallback().
	static std::future<void> setMouseButtonCallback(GLFWwindow* window, GLFWmousebuttonfun cbfun);
	//! Posts an event to set the keyboard callback of the specified window using glfwSetCursorPosCallback().
	static std::future<void> setCursorPosCallback(GLFWwindow* window, GLFWcursorposfun cbfun);
	//! Posts an event to set the keyboard callback of the specified window using glfwSetScrollCallback().
	static std::future<void> setScrollCallback(GLFWwindow* window, GLFWscrollfun cbfun);
	//! Posts an event to set the keyboard callback of the specified window using glfwSetKeyCallback().
	static std::future<void> setKeyCallback(GLFWwindow* window, GLFWkeyfun cbfun);
	//! Posts an event to set the keyboard callback of the specified window using glfwSetCharCallback().
	static std::future<void> setCharCallback(GLFWwindow* window, GLFWcharfun cbfun);
	//! Posts an event to set the keyboard callback of the specified window using glfwSetWindowSizeCallback().
	static std::future<void> setWindowSizeCallback(GLFWwindow* window, GLFWwindowsizefun cbfun);

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
	static std::atomic<bool> m_initialized;
	//! Flag used to indicate whether the event loop should continue or an exit event was received.
	static std::atomic<bool> m_continueEventLoop;

	//! Thread id of the thread that initialized the global, static variables.
	const static std::thread::id m_mainThreadId;
	//! Returns whether the current thread id is the same as the id of the thread used to initialize global, static variables.
	static bool isMainThread();
};
