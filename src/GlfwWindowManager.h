#pragma once

#include <thread>
#include <queue>
#include <future>
#include <mutex>
#include <variant>

#include <GLFW/glfw3.h>

class GlfwWindowManager
{
public:
	GlfwWindowManager(bool throwOnFailure = true);
	~GlfwWindowManager();

	static bool isInitialized();
	static void errorCallback(int error, const char* description);
	static void processEvents();
	static void startEventLoop();

	static std::future<void> exitEventLoop();
	static std::future<GLFWwindow*> requestWindow(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share);
	static std::future<void> destroyWindow(GLFWwindow*);

private:
	static bool initialized;
	static bool continueEventLoop;

	static std::thread::id mainThreadId;
	static bool isMainThread();

	template <typename PromisedT, typename EventT>
	struct EventTask
	{
		using promised_type = PromisedT;
		using event_type = EventT;

		std::promise<PromisedT> promise;
		EventT event;
	};

	struct ExitEvent
	{
	};

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

	static std::mutex eventQueueMutex;
	static std::queue<EventRequestVariant> eventQueue;
};
