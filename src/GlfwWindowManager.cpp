#include "GlfwWindowManager.h"

#include <iostream>
#include <stdexcept>
#include <string>

bool GlfwWindowManager::initialized = false;
bool GlfwWindowManager::continueEventLoop = false;
std::thread::id GlfwWindowManager::mainThreadId = std::this_thread::get_id();

std::mutex GlfwWindowManager::eventQueueMutex;
std::queue<GlfwWindowManager::EventRequestVariant> GlfwWindowManager::eventQueue;

GlfwWindowManager::GlfwWindowManager(bool throwOnFailure)
{
	// Make sure that this object is only created in the main thread
	if (!isMainThread()) {
		const std::string error = "GLFW can only be initialized from the main thread!";
		std::cerr << error << "\n";

		if (throwOnFailure) throw std::runtime_error(error);
	}

	if (initialized) return;

	// Try to initialize GLFW
	if (!glfwInit()) {
		const std::string error = "GLFW could not be initialized!";
		std::cerr << error << "\n";
		initialized = false;

		if (throwOnFailure) throw std::runtime_error(error);
	}
	else {
		initialized = true;
		glfwSetErrorCallback(GlfwWindowManager::errorCallback);
	}
}

GlfwWindowManager::~GlfwWindowManager()
{
	// Unload GLFW
	if (initialized) glfwTerminate();
	initialized = false;
}

bool GlfwWindowManager::isInitialized()
{
	return initialized;
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

	while (!eventQueue.empty()) {
		// Define the unnamed visitor which processes all event types
		struct
		{
			void operator()(ExitEventRequest& request)
			{
				GlfwWindowManager::continueEventLoop = false;
				request.promise.set_value();
			}

			// Create a new window
			void operator()(CreateWindowEventRequest& request)
			{
				auto& event = request.event;
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

		std::visit(eventVisitor, eventQueue.front());
		eventQueue.pop();

		if (!continueEventLoop) break;
	}
}

void GlfwWindowManager::startEventLoop()
{
	// Make sure that this function is only called in the main thread
	if (!isMainThread()) {
		const std::string error = "GLFW event loop can only be run in the main thread!";
		std::cerr << error << "\n";
		return;
	}

	continueEventLoop = true;
	while (continueEventLoop) {
		glfwWaitEvents(); {
			std::lock_guard<std::mutex> lock(eventQueueMutex);
			processEvents();
		}
	}
}

std::future<void> GlfwWindowManager::exitEventLoop()
{
	std::promise<void>* promise; {
		std::lock_guard<std::mutex> lock(eventQueueMutex);
		eventQueue.emplace(ExitEventRequest{ std::promise<void>() });
		promise = &(std::get<ExitEventRequest>(eventQueue.back())).promise;
		glfwPostEmptyEvent();
	}

	return promise->get_future();
}

std::future<GLFWwindow*> GlfwWindowManager::requestWindow(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share)
{
	std::promise<GLFWwindow*>* promise; {
		std::lock_guard<std::mutex> lock(eventQueueMutex);
		eventQueue.emplace(CreateWindowEventRequest{ std::promise<GLFWwindow*>(), CreateWindowEvent{width, height, title, monitor, share} });
		promise = &(std::get<CreateWindowEventRequest>(eventQueue.back())).promise;
		glfwPostEmptyEvent();
	}

	return promise->get_future();
}

std::future<void> GlfwWindowManager::destroyWindow(GLFWwindow* window)
{
	std::promise<void>* promise; {
		std::lock_guard<std::mutex> lock(eventQueueMutex);
		eventQueue.emplace(DestroyWindowEventRequest{ std::promise<void>(), DestroyWindowEvent{ window } });
		promise = &(std::get<DestroyWindowEventRequest>(eventQueue.back())).promise;
		glfwPostEmptyEvent();
	}

	return promise->get_future();
}

bool GlfwWindowManager::isMainThread()
{
	return mainThreadId == std::this_thread::get_id();
}
