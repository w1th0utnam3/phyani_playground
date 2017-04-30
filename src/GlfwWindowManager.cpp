#include "GlfwWindowManager.h"

#include <iostream>
#include <stdexcept>
#include <string>

std::atomic<bool> GlfwWindowManager::m_initialized(false);
std::atomic<bool> GlfwWindowManager::m_continueEventLoop(false);
const std::thread::id GlfwWindowManager::m_mainThreadId = std::this_thread::get_id();

std::mutex GlfwWindowManager::m_eventQueueMutex;
std::queue<GlfwWindowManager::EventRequestVariant> GlfwWindowManager::m_eventQueue;

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

void GlfwWindowManager::processEvents()
{
	// Make sure that this function is only called in the main thread
	if (!isMainThread()) {
		const std::string error = "GLFW windows can only be created in the main thread!";
		std::cerr << error << "\n";
		return;
	}

	while (!m_eventQueue.empty()) {
		// Define the visitor which processes all possible event types
		struct
		{
			void operator()(ExitEventRequest& request)
			{
				// Change the flag to break event loop
				GlfwWindowManager::m_continueEventLoop = false;
				request.promise.set_value();
			}

			void operator()(CreateWindowEventRequest& request)
			{
				auto& event = request.event;
				// Try to create a new window
				auto window = glfwCreateWindow(event.width, event.height, event.title, event.monitor, event.share);
				if (!window) {
					const std::string error = "GLFW window could not be created!";
					std::cerr << error << "\n";
				}
				request.promise.set_value(window);
			}

			// Destroy a window
			void operator()(DestroyWindowEventRequest& request)
			{
				auto& event = request.event;
				glfwDestroyWindow(event.window);
				request.promise.set_value();
			}
		} eventVisitor;

		// Apply the visitor to the oldest event
		std::visit(eventVisitor, m_eventQueue.front());
		m_eventQueue.pop();

		// Loop through queue if no exit event was encountered
		if (!m_continueEventLoop) break;
	}
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
		glfwWaitEvents(); {
			// Lock queue mutex, as processEvents() is not thread safe
			std::lock_guard<std::mutex> lock(m_eventQueueMutex);
			processEvents();
		}
	}
}

std::future<void> GlfwWindowManager::exitEventLoop()
{
	std::promise<void>* promise; {
		std::lock_guard<std::mutex> lock(m_eventQueueMutex);
		m_eventQueue.emplace(ExitEventRequest{std::promise<void>()});
		promise = &(std::get<ExitEventRequest>(m_eventQueue.back())).promise;
		glfwPostEmptyEvent();
	}

	return promise->get_future();
}

std::future<GLFWwindow*> GlfwWindowManager::requestWindow(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share)
{
	std::promise<GLFWwindow*>* promise; {
		std::lock_guard<std::mutex> lock(m_eventQueueMutex);
		m_eventQueue.emplace(CreateWindowEventRequest{std::promise<GLFWwindow*>(), CreateWindowEvent{width, height, title, monitor, share}});
		promise = &(std::get<CreateWindowEventRequest>(m_eventQueue.back())).promise;
		glfwPostEmptyEvent();
	}

	return promise->get_future();
}

std::future<void> GlfwWindowManager::destroyWindow(GLFWwindow* window)
{
	std::promise<void>* promise; {
		std::lock_guard<std::mutex> lock(m_eventQueueMutex);
		m_eventQueue.emplace(DestroyWindowEventRequest{std::promise<void>(), DestroyWindowEvent{window}});
		promise = &(std::get<DestroyWindowEventRequest>(m_eventQueue.back())).promise;
		glfwPostEmptyEvent();
	}

	return promise->get_future();
}

bool GlfwWindowManager::isMainThread()
{
	return m_mainThreadId == std::this_thread::get_id();
}
