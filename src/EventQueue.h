#pragma once

#include <mutex>
#include <future>
#include <queue>
#include <variant>

#include "noname_tools\utility_tools.h"

//! Element type for the 'EventQueue'.
/*
 * Stores a single request and the associated promise. The future of the promise is returned by the
 * EventQueue when a request of the associated request type was posted to the queue.
 */
template <typename RequestT, typename PromisedT>
struct Event
{
	//! Data type of the request.
	using request_type = RequestT;
	//! Data type of the promised response.
	using promised_type = PromisedT;

	//! Request input data.
	RequestT request;
	//! Promise to return response data.
	std::promise<PromisedT> promise;
};

//! An event queue which stores requests of the user and returns future objects to await a result.
template <typename... EventTs>
class EventQueue : protected std::queue<std::variant<Event<typename EventTs::request_type, typename EventTs::promised_type>...>>
{
	// Make sure that we have more than one event type
	static_assert(sizeof...(EventTs) > 0, "The number of event types has to be larger than zero.");
	// Make sure that request types are unique
	static_assert(noname::tools::unique_types_v<typename EventTs::request_type...>, "The request types of the supplied events have to be unique.");

	//! Alias for the promised result type associated to the 'RequestT'.
	template <typename RequestT>
	using promised_type = noname::tools::nth_element_t<noname::tools::element_index_v<RequestT, typename EventTs::request_type...>, typename EventTs::promised_type...>;
	//! The underlying container type of the queue.
	using container_type = std::queue<std::variant<Event<typename EventTs::request_type, typename EventTs::promised_type>...>>;

public:
	//! Creates an event associated to the specified request and returns a future to wait for a response.
	/*
	 * \param request A request which has to be of a supported type, i.e. one event supported by the 
	 *		queue has to specify it as a 'request_type'.
	 * \return A future which can  be used to wait for the response of the request.
	 */
	template <typename RequestT>
	std::future<promised_type<RequestT>> postEvent(RequestT request)
	// We can specialize on return type and still never have to explicitely specify it, because the 'RequestT' types have to be unique
	{
		static_assert(noname::tools::count_element_v<RequestT, typename EventTs::request_type...> > 0, "The request type has to be a request type of the compatible events.");
		using PromisedT = promised_type<RequestT>;
		using EventT = Event<RequestT, PromisedT>;

		std::promise<promised_type<RequestT>>* promise; {
			std::lock_guard<std::mutex> lock(this->m_queueMutex);
			// Store the event with a promise associated to the request
			this->emplace(EventT{request});
			promise = &std::get<EventT>(this->back()).promise;
		}
		return promise->get_future();
	}

	//! Uses a visitor to process the oldest event in the queue.
	/*
	 * The specified visitor is called with the oldest event of type Event<RequestT, PromisedT>. 
	 * It is the task visitor to update the promise with a value to allow the user to obtain it
	 * via the associated future.
	 * \param visitor The visitor to apply to the event. Has to provide a function call operator
	 *		for every supported event type of the queue.
	 */
	template <typename VisitorT>
	void processOldestEvent(VisitorT visitor)
	{
		std::lock_guard<std::mutex> lock(m_queueMutex);
		// Apply visitor to process the event
		std::visit(visitor, this->front());
		// Remove the event from the queue
		this->pop();
	}

	//! Returns whether the queue is currently empty
	using container_type::empty;
	//! Returns the number of events in the queue
	using container_type::size;

protected:
	std::mutex m_queueMutex;
};
