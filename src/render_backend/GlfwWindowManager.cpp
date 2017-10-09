#include "GlfwWindowManager.h"

#include <iostream>
#include <stdexcept>
#include <string>

#include "EventQueue.h"

std::atomic<bool> GlfwWindowManager::m_initialized(false);
std::atomic<bool> GlfwWindowManager::m_continueEventLoop(false);
const std::thread::id GlfwWindowManager::m_mainThreadId = std::this_thread::get_id();

GlfwWindowManager::event_queue_type GlfwWindowManager::m_eventQueue;

auto GlfwWindowManager::create(bool throwOnFailure) -> GlfwWindowManagerUniquePtr
{
	// Get address of the deleter
	constexpr static auto deleter = &GlfwWindowManager::deleter;

	// Make sure that there currently is no other GlfwWindowManager
	if (m_initialized) {
		const std::string error = "GLFW cannot be initialized twice at the same time!";
		std::cerr << error << "\n";

		if (throwOnFailure) throw std::runtime_error(error);

		return GlfwWindowManagerUniquePtr(nullptr, deleter);
	}

	// Make sure that this function is only called from the main thread
	if (!isMainThread()) {
		const std::string error = "GLFW can only be initialized from the main thread!";
		std::cerr << error << "\n";

		if (throwOnFailure) throw std::runtime_error(error);

		return GlfwWindowManagerUniquePtr(nullptr, deleter);
	}

	// Create the GlfwWindowManager
	auto manager = GlfwWindowManagerUniquePtr(new GlfwWindowManager(throwOnFailure), deleter);
	return std::move(manager);
}

void GlfwWindowManager::deleter(GlfwWindowManager* manager)
{
	delete manager;
}

GlfwWindowManager::GlfwWindowManager(bool throwOnFailure)
{
	// Try to initialize GLFW
	if (!glfwInit()) {
		const std::string error = "GLFW could not be initialized!";
		std::cerr << error << "\n";
		m_initialized = false;

		if (throwOnFailure) throw std::runtime_error(error);
	}
	else {
		m_initialized = true;
		glfwSetErrorCallback(GlfwWindowManager::errorCallback);
	}
}

GlfwWindowManager::~GlfwWindowManager()
{
	// Unload GLFW
	if (m_initialized) glfwTerminate();
	m_initialized = false;
}

bool GlfwWindowManager::isInitialized()
{
	return m_initialized;
}

void GlfwWindowManager::errorCallback(int error, const char* description)
{
	fprintf(stderr, "Error %d: %s\n", error, description);
}

void GlfwWindowManager::executeEventLoop()
{
	// Make sure that this function is only called from the main thread
	if (!isMainThread()) {
		const std::string error = "GLFW event loop can only be run in the main thread!";
		std::cerr << error << "\n";
		return;
	}

	m_continueEventLoop = true;
	while (m_continueEventLoop) {
		// Wait for new GLFW events
		glfwWaitEvents();
		processEvents();
	}
}

void GlfwWindowManager::processEvents()
{
	// Make sure that this function is only called in the main thread
	if (!isMainThread()) {
		const std::string error = "GLFW windows can only be created in the main thread!";
		std::cerr << error << "\n";
		return;
	}

	std::atomic<bool> processEvents{true};

	// Define the visitor which processes all possible event types
	const struct
	{
		std::atomic<bool>* processEvents;

		void operator()(StopEventLoopEvent& event) const
		{
			// Change the flag to break event loop
			m_continueEventLoop = false;
			processEvents->store(false);
			event.promise.set_value();
		}

		void operator()(CreateWindowEvent& event) const
		{
			auto& request = event.request;
			// Try to create a new window
			auto window = glfwCreateWindow(request.width, request.height, request.title, request.monitor, request.share);
			if (!window) {
				const std::string error = "GLFW window could not be created!";
				std::cerr << error << "\n";
			}
			event.promise.set_value(window);
		}

		// Destroy a window
		void operator()(DestroyWindowEvent& event) const
		{
			auto& request = event.request;
			glfwDestroyWindow(request.window);
			event.promise.set_value();
		}

		void operator()(SetMouseButtonCallbackEvent& event) const
		{
			auto& request = event.request;
			glfwSetMouseButtonCallback(request.window, request.cbfun);
			event.promise.set_value();
		}

		void operator()(SetCursorPosCallbackEevent& event) const
		{
			auto& request = event.request;
			glfwSetCursorPosCallback(request.window, request.cbfun);
			event.promise.set_value();
		}

		void operator()(SetScrollCallbackEvent& event) const
		{
			auto& request = event.request;
			glfwSetScrollCallback(request.window, request.cbfun);
			event.promise.set_value();
		}

		void operator()(SetKeyCallbackEvent& event) const
		{
			auto& request = event.request;
			glfwSetKeyCallback(request.window, request.cbfun);
			event.promise.set_value();
		}

		void operator()(SetCharCallbackEvent& event) const
		{
			auto& request = event.request;
			glfwSetCharCallback(request.window, request.cbfun);
			event.promise.set_value();
		}

		void operator()(SetCharModsCallbackEvent& event) const
		{
			auto& request = event.request;
			glfwSetCharModsCallback(request.window, request.cbfun);
			event.promise.set_value();
		}

		void operator()(SetWindowSizeCallbackEvent& event) const
		{
			auto& request = event.request;
			glfwSetWindowSizeCallback(request.window, request.cbfun);
			event.promise.set_value();
		}
	} eventVisitor{ &processEvents };

	while (processEvents && !m_eventQueue.empty()) {
		// The event queue ensures thread safety
		m_eventQueue.processOldestEvent(eventVisitor);
	}
}

std::future<void> GlfwWindowManager::stopEventLoop()
{
	return m_eventQueue.postEvent(StopEventLoopRequest());
}

std::future<GLFWwindow*> GlfwWindowManager::requestWindow(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share)
{
	return m_eventQueue.postEvent(CreateWindowRequest{width, height, title, monitor, share});
}

std::future<void> GlfwWindowManager::destroyWindow(GLFWwindow* window)
{
	return m_eventQueue.postEvent(DestroyWindowRequest{window});
}

std::future<void> GlfwWindowManager::setMouseButtonCallback(GLFWwindow* window, GLFWmousebuttonfun cbfun)
{
	return m_eventQueue.postEvent(SetMouseButtonCallbackRequest{ window, cbfun });
}

std::future<void> GlfwWindowManager::setCursorPosCallback(GLFWwindow* window, GLFWcursorposfun cbfun)
{
	return m_eventQueue.postEvent(SetCursorPosCallbackRequest{ window, cbfun });
}

std::future<void> GlfwWindowManager::setScrollCallback(GLFWwindow* window, GLFWscrollfun cbfun)
{
	return m_eventQueue.postEvent(SetScrollCallbackRequest{ window, cbfun });
}

std::future<void> GlfwWindowManager::setKeyCallback(GLFWwindow* window, GLFWkeyfun cbfun)
{
	return m_eventQueue.postEvent(SetKeyCallbackRequest{ window, cbfun });
}

std::future<void> GlfwWindowManager::setCharCallback(GLFWwindow* window, GLFWcharfun cbfun)
{
	return m_eventQueue.postEvent(SetCharCallbackRequest{ window, cbfun });
}

std::future<void> GlfwWindowManager::setCharModsCallback(GLFWwindow* window, GLFWcharmodsfun cbfun)
{
	return m_eventQueue.postEvent(SetCharModsCallbackRequest{ window, cbfun });
}

std::future<void> GlfwWindowManager::setWindowSizeCallback(GLFWwindow* window, GLFWwindowsizefun cbfun)
{
	return m_eventQueue.postEvent(SetWindowSizeCallbackRequest{ window, cbfun });
}

bool GlfwWindowManager::isMainThread()
{
	return m_mainThreadId == std::this_thread::get_id();
}
