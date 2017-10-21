#pragma once

#include <cassert>
#include <limits>
#include <mutex>
#include <shared_mutex>
#include <vector>
#include <utility>
#include <type_traits>
#include <iterator>

#include <noname_tools/range_tools.h>

#include "CommonOpenGl.h"
#include "DrawableFactory.h"

template <typename DrawableManagerT>
class DrawableProxy;

template <typename DrawableManagerT>
class DrawableIterator;

template <typename InstanceDataT_, typename DrawableSourceT_ = DrawableFactory::DrawableSource>
class DrawableManager
{
public:
	using InstanceDataT = InstanceDataT_;
	using DrawableSourceT = DrawableSourceT_;

	using DrawableManagerT = DrawableManager<InstanceDataT, DrawableSourceT>;

	using DrawableProxyT = DrawableProxy<DrawableManagerT>;
	using DrawableIteratorT = DrawableIterator<DrawableManagerT>;

	template <typename T>
	friend class DrawableProxy;

	template <typename T>
	friend class DrawableIterator;

	using VertexT = typename DrawableSourceT::VertexT;
	using IndexT = typename DrawableSourceT::IndexT;

	struct DrawableData
	{
		static constexpr GLenum glIndexType = DrawableSourceT::glIndexType;

		GLenum glMode;

		GLint vertexBufferOffset;
		GLsizei vertexCount;

		GLint normalBufferOffset;
		GLsizei normalCount;

		GLint indexBufferOffset;
		GLsizei indexCount;

		GLint baseVertex;
		GLvoid* indexPtrOffset;
	};

// Required for DrawableProxy base class definition in GCC
//		See: https://stackoverflow.com/questions/46819854/access-private-definitions-in-base-clause-of-friend-class-template
#ifdef __GNUC__
	using MutexT = std::mutex;
#endif

private:

#ifndef __GNUC__
	using MutexT = std::mutex;
#endif
	using SharedMutexT = std::shared_timed_mutex;

	//! Number of VertexT instances in the buffer that represent a single vertex
	static constexpr std::size_t BufferEntriesPerVertex = DrawableSourceT::BufferEntriesPerVertex;

	//! Container type used for the buffers
	template<typename T>
	using ContainerT = std::vector<T>;

	struct Drawable
	{
		MutexT drawableMutex;
		DrawableData data;
		std::vector<InstanceDataT> instances;

		Drawable() = default;

		//! Copy constructor, not thread safe!
		Drawable(const Drawable& other)
			: data(other.data)
			, instances(other.instances)
		{
		}

		//! Move constructor, not thread safe!
		Drawable(Drawable&& other) noexcept(std::is_nothrow_move_constructible<decltype(instances)>::value)
			: data(std::move(other.data))
			, instances(std::move(other.instances))
		{
		}
	};

public:
	//! Clears all data buffers and removes all drawables
	/*
	 * Before this method can clear the container, an exclusive lock over the manager has to be acquired.
	 * Therefore, it may block until this was successful.
	 */
	void clear()
	{
		// Require exclusive access to the central lock
		std::lock_guard<SharedMutexT> lock(m_sharedManagerMutex);

		// Clear all data containers
		m_vertexBuffer.clear();
		m_normalBuffer.clear();
		m_indexBuffer.clear();
		m_drawables.clear();
	}

	//! Adds a new drawable to the manager and returns its index
	/*
	 * Copies the supplied vertex/normal/index data to the internal buffers and registers the drawable
	 * in order to allow creation of corresponding instances.
	 * Before this method can add a drawable to the container, an exclusive lock over the manager has to
	 * be acquired (because of potential reallocations). Therefore, it may block until this was successful.
	 */
	GLsizei registerDrawable(const DrawableSourceT& drawable)
	{
		// Require exclusive access to the central lock (registration might cause reallocations)
		std::lock_guard<SharedMutexT> lock(m_sharedManagerMutex);

		// Create a new drawable object
		m_drawables.emplace_back();
		DrawableData& drawableData = m_drawables.back().data;

		// Set drawing mode and check whether it is valid
		drawableData.glMode = drawable.glMode;
		assert(CommonOpenGl::isValidGlMode(drawable.glMode));

		// Make sure that all vertices of the new drawable are indexable using the index type
		assert(drawable.vertices.size()/BufferEntriesPerVertex < std::numeric_limits<IndexT>::max());

		// Copy vertices to normal buffer
		drawableData.vertexBufferOffset = static_cast<GLint>(m_vertexBuffer.size());
		drawableData.vertexCount = static_cast<GLsizei>(drawable.vertices.size()/BufferEntriesPerVertex);
		m_vertexBuffer.insert(m_vertexBuffer.end(), drawable.vertices.begin(), drawable.vertices.end());

		// Make sure that all vertices are indexable by OpenGL
		assert((m_vertexBuffer.size()/BufferEntriesPerVertex) < std::numeric_limits<GLint>::max());

		// Copy normals to normal buffer
		drawableData.normalBufferOffset = static_cast<GLint>(m_normalBuffer.size());
		drawableData.normalCount = static_cast<GLsizei>(drawable.normals.size()/BufferEntriesPerVertex);
		m_normalBuffer.insert(m_normalBuffer.end(), drawable.normals.begin(), drawable.normals.end());

		// Copy indices to index buffer
		drawableData.indexBufferOffset = static_cast<GLint>(m_indexBuffer.size());
		drawableData.indexCount = static_cast<GLsizei>(drawable.indices.size());
		m_indexBuffer.insert(m_indexBuffer.end(), drawable.indices.begin(), drawable.indices.end());

		// Store the index of the drawable's base vertex
		drawableData.baseVertex = drawableData.vertexBufferOffset/BufferEntriesPerVertex;
		drawableData.indexPtrOffset = (GLvoid*)(drawableData.indexBufferOffset * sizeof(typename DrawableSourceT::IndexT));

		// Return drawable id
		return static_cast<GLsizei>(m_drawables.size() - 1);
	}

	//! Creates a new instance of the specified drawable and stores the supplied instance data
	GLsizei storeInstance(GLsizei drawableId, const InstanceDataT& instanceData)
	{
		// Lock the drawable manager against clearing and reallocation
		std::shared_lock<SharedMutexT> bufferLock(m_sharedManagerMutex);

		// Make sure that drawable exists
		assert(drawableId < m_drawables.size());

		// Get the corresponding drawable and lock it against simultaneous modification
		auto& drawable = m_drawables[drawableId];
		std::lock_guard<MutexT> lock(drawable.drawableMutex);

		// Calculate index of the new instance
		const std::size_t instanceOffset = drawable.instances.size();
		// Make sure that the new instance is idexable by OpenGL
		assert(instanceOffset + 1 < std::numeric_limits<GLsizei>::max());

		// Create the instance
		drawable.instances.push_back(instanceData);

		// Return instance id
		return static_cast<GLsizei>(instanceOffset);
	}

	//! Creates the specified number of instances of the drawable without assinging any instance data
	GLsizei createInstances(GLsizei drawableId, GLsizei instanceCount)
	{
		// Lock the drawable manager against clearing and reallocation
		std::shared_lock<SharedMutexT> bufferLock(m_sharedManagerMutex);

		// Make sure that drawable exists
		assert(drawableId < m_drawables.size());

		// Get the corresponding drawable and lock it against simultaneous modification
		auto& drawable = m_drawables[drawableId];
		std::lock_guard<MutexT> lock(drawable.drawableMutex);

		// Calculate index of first new instance
		const std::size_t instanceOffset = drawable.instances.size();
		// Make sure that all instances are idexable by OpenGL
		assert(instanceOffset + static_cast<std::size_t>(instanceCount) < std::numeric_limits<GLsizei>::max());

		// Create the specified number of instances
		drawable.instances.resize(instanceOffset + instanceCount);

		// Return id of first created instance
		return static_cast<GLsizei>(instanceOffset);
	}

	//! Removes the instance with the supplied id of the specified drawable
	void destroyInstance(GLsizei drawableId, GLsizei instanceId)
	{
		// Lock the drawable manager against clearing and reallocation
		std::shared_lock<SharedMutexT> bufferLock(m_sharedManagerMutex);

		// Make sure that drawable exists
		assert(drawableId < m_drawables.size());

		// Get the corresponding drawable and lock it against simultaneous modification
		auto& drawable = m_drawables[drawableId];
		std::lock_guard<MutexT> lock(drawable.drawableMutex);

		// Make sure that the instance exists
		assert(instanceId < drawable.instances.size());

		// Erase the specified instance
		drawable.instances.erase(drawable.instances.begin() + instanceId);
	}

	//! Returns a shared lock for the manager
	/*
	 * Manually requesting a shared lock for the manager is (only) required when reading drawable
	 * data using the drawable() method or the iterators in order to prevent data races. All other
	 * methods automatically lock the required mutexes.
	 */
	std::shared_lock<SharedMutexT> shared_lock() { return std::shared_lock<SharedMutexT>(m_sharedManagerMutex); }

	//! Returns the number of vertices stored in the vertex buffer
	GLsizei vertexCount() const { return static_cast<GLsizei>(m_vertexBuffer.size()); }
	//! Returns the number of normals stored in the normal buffer
	GLsizei normalCount() const { return static_cast<GLsizei>(m_normalBuffer.size()); }
	//! Returns the number of indices stored in the index buffer
	GLsizei indexCount() const { return static_cast<GLsizei>(m_indexBuffer.size()); }

	//! Returns the size in bytes of the vertex buffer
	std::size_t vertexBufferSize() const { return m_vertexBuffer.size() * sizeof(VertexT); }
	//! Returns the size in bytes of the normal buffer
	std::size_t normalBufferSize() const { return m_normalBuffer.size() * sizeof(VertexT); }
	//! Returns the size in bytes of the index buffer
	std::size_t indexBufferSize() const { return m_indexBuffer.size() * sizeof(IndexT); }

	//! Returns a pointer to the vertex buffer
	VertexT* vertexBufferData() { return m_vertexBuffer.data(); }
	//! Returns a pointer to the normal buffer
	VertexT* normalBufferData() { return m_normalBuffer.data(); }
	//! Returns a pointer to the index buffer
	IndexT* indexBufferData() { return m_indexBuffer.data(); }

	//! Returns an object with data of the specified drawable
	/*
	 * Creates a DrawableProxy object for the specified drawable which locks the drawable against
	 * modification during the lifetime of the proxy object. The proxy object gives access to the
	 * instance data of the drawable and indices/offsets in order to acces the vertex/normal/index
	 * data of the drawable.
	 */
	DrawableProxyT drawable(GLsizei drawableId) { return DrawableProxyT(m_drawables[drawableId]); }
	//! Returns an iterator to the beginning of the drawable container
	/*
	 * The iterator can be used to loop over all drawables and dereferencing it yields a DrawableProxy
	 * corresponding to the current drawable.
	 */
	DrawableIteratorT drawablesBegin() { return DrawableIteratorT(m_drawables.begin()); }
	//! Returns an iterator behind the end of the drawable container
	/*
	 * The iterator can be used to loop over all drawables and dereferencing it yields a DrawableProxy
	 * corresponding to the current drawable.
	 */
	DrawableIteratorT drawablesEnd() { return DrawableIteratorT(m_drawables.end()); }

	//! Returns a range object to loop over all drawables in a range-based for loop
	auto drawableRange()
	{
		return noname::tools::make_range(
					DrawableIteratorT(m_drawables.begin()),
					DrawableIteratorT(m_drawables.end()));
	}

private:
	//! Mutex to protect against data races caused by drawable access and reallocation/clear calls
	SharedMutexT m_sharedManagerMutex;

	//! Container storing the drawables with buffer offsets and instance data
	ContainerT<Drawable> m_drawables;

	//! Buffer of vertices
	ContainerT<VertexT> m_vertexBuffer;
	//! Buffer of normals
	ContainerT<VertexT> m_normalBuffer;
	//! Buffer of indices
	ContainerT<IndexT> m_indexBuffer;
};

template <typename DrawableManagerT>
class DrawableProxy : private std::unique_lock<typename DrawableManagerT::MutexT>, public DrawableManagerT::DrawableData
{
public:
	DrawableProxy(typename DrawableManagerT::Drawable& target)
		: std::unique_lock<typename DrawableManagerT::MutexT>(target.drawableMutex)
		, DrawableManagerT::DrawableData(target.data)
		, m_target(target)
	{
	}

	DrawableProxy(DrawableProxy&& other) = default;
	DrawableProxy& operator=(DrawableProxy&& other) = default;

	GLsizei instanceCount() const { return static_cast<GLsizei>(m_target.instances.size()); }
	std::size_t instanceDataSize() const { return m_target.instances.size() * sizeof(typename DrawableManagerT::InstanceDataT); }
	typename DrawableManagerT::InstanceDataT* instanceData() { return m_target.instances.data(); }
	const typename DrawableManagerT::InstanceDataT* instanceData() const { return m_target.instances.data(); }

private:
	typename DrawableManagerT::Drawable& m_target;
};

template <typename DrawableManagerT>
class DrawableIterator
{
public:
	using difference_type = GLsizei;
	using value_type = typename DrawableManagerT::DrawableProxyT;
	using pointer = typename DrawableManagerT::DrawableProxyT*;
	using reference = typename DrawableManagerT::DrawableProxyT&;
	using iterator_category = std::forward_iterator_tag;

	DrawableIterator() = default;
	DrawableIterator(const typename DrawableManagerT::template ContainerT<typename DrawableManagerT::Drawable>::iterator& it) : it(it) {}

	bool operator==(const DrawableIterator& other) const { return it == other.it; }
	bool operator!=(const DrawableIterator& other) const { return it != other.it; }
	bool operator<(const DrawableIterator& other) const { return it < other.it; }
	bool operator>(const DrawableIterator& other) const { return it > other.it; }

	DrawableIterator& operator++() { ++it; return *this; }
	DrawableIterator& operator--() { --it; return *this; }

	DrawableIterator& operator++(int) { auto tmp = *this; ++it; return tmp; }
	DrawableIterator& operator--(int) { auto tmp = *this; --it; return tmp; }

	typename DrawableManagerT::DrawableProxyT operator*() { return typename DrawableManagerT::DrawableProxyT(*it); }

private:
	typename DrawableManagerT::template ContainerT<typename DrawableManagerT::Drawable>::iterator it;
};

namespace std {
	template <class DrawableManagerT>
	struct iterator_traits<DrawableIterator<DrawableManagerT>>
	{
		using difference_type	= typename DrawableIterator<DrawableManagerT>::difference_type;
		using value_type		= typename DrawableIterator<DrawableManagerT>::value_type;
		using pointer			= typename DrawableIterator<DrawableManagerT>::pointer;
		using reference			= typename DrawableIterator<DrawableManagerT>::reference;
		using iterator_category	= typename DrawableIterator<DrawableManagerT>::iterator_category;
	};
}
