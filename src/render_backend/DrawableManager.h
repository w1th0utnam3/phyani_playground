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

// TODO: Shared access to drawable?
// TODO: Instance data buffer swapping?
// TODO: Iterator distance

//! Container that stores drawable objects and their data and controls multithreaded acces to it
template <typename InstanceDataT, typename DrawableSourceT = DrawableFactory::DrawableSource>
class DrawableManager;

//! Proxy type returned when accessing specific drawables in the DrawableManager
template <typename DrawableManagerT>
class DrawableProxy;

//! Iterator type used when iterating over drawable objects stored in the DrawableManager
template <typename DrawableManagerT>
class DrawableIterator;

template <typename InstanceDataT_, typename DrawableSourceT_>
class DrawableManager
{
public:
	//! Type associated to specific instances of drawables, e.g. colors, materials...
	using InstanceDataT = InstanceDataT_;
	//! Type used as input when registering new drawables to the manager
	using DrawableSourceT = DrawableSourceT_;

	//! Concrete type of this class
	using DrawableManagerT = DrawableManager<InstanceDataT, DrawableSourceT>;

	//! Drawable proxy type associated with this class, returned when accessing drawables using the public interface
	using DrawableProxyT = DrawableProxy<DrawableManagerT>;
	//! Iterator type associated with this class, used when iterating over drawables using the public interface
	using DrawableIteratorT = DrawableIterator<DrawableManagerT>;

	//! Vertex type of the drawables which is stored in the vertex buffer (also used for normals)
	using VertexT = typename DrawableSourceT::VertexT;
	//! Index type of the drawables which is stored in the index buffer
	using IndexT = typename DrawableSourceT::IndexT;

	//! Information required to render a drawable, base class for DrawableProxy
	struct DrawableInformation
	{
		//! GLenum encoding of the drawable's index type, e.g. GL_UNSIGNED_SHORT
		static constexpr GLenum glIndexType = DrawableSourceT::glIndexType;
		//! OpenGL drawing mode required to render this drawable
		GLenum glMode;

		//! Index in the vertex buffer where this drawable starts
		GLint vertexBufferOffset;
		//! Number of VertexT instances belonging to this drawable in the vertex buffer
		GLsizei vertexCount;

		//! Index in the normal buffer where this drawable starts
		GLint normalBufferOffset;
		//! Number of VertexT instances belonging to this drawable in the normal buffer
		GLsizei normalCount;

		//! Index in the index buffer where this drawable starts
		GLint indexBufferOffset;
		//! Number of IndexT instances belonging to this drawable in the index buffer
		GLsizei indexCount;

		//! The base vertex parameter for this drawable
		GLint baseVertex;
		//! Pointer offset of this drawable in the index buffer
		GLvoid* indexPtrOffset;
	};

// Required for DrawableProxy base class definition in GCC
//		See: https://stackoverflow.com/questions/46819854/access-private-definitions-in-base-clause-of-friend-class-template
#ifdef __GNUC__
	using DrawableMutexT = std::mutex;
#endif

private:
	// Friend the associated proxy and iterator types
	friend DrawableProxyT;
	friend DrawableIteratorT;

#ifndef __GNUC__
	//! Mutex type used for drawable access control
	using DrawableMutexT = std::mutex;
#endif
	//! Mutex type used for shared access to the whole DrawableManager
	using ManagerMutexT = std::shared_timed_mutex;

	//! Number of VertexT instances in the buffer that represent a single vertex
	static constexpr std::size_t bufferEntriesPerVertex = DrawableSourceT::bufferEntriesPerVertex;

	//! Container type used for the buffers
	template<typename T>
	using ContainerT = std::vector<T>;

	//! Internal representation of the registered drawables
	struct DrawableInternalData
	{
		//! Mutex for exclusive access control, to prevent accidental reallocations
		DrawableMutexT drawableMutex;
		//! Information like offsets associated to this drawable
		DrawableInformation data;
		//! Container of specific instances of this drawable
		ContainerT<InstanceDataT> instances;

		DrawableInternalData() = default;

		//! Copy constructor, not thread safe! It has to be made sure that the DrawableManager is locked exclusively before copying drawables.
		DrawableInternalData(const DrawableInternalData& other)
			: data(other.data)
			, instances(other.instances)
		{
		}

		//! Move constructor, not thread safe! It has to be made sure that the DrawableManager is locked exclusively before moving drawables.
		DrawableInternalData(DrawableInternalData&& other) noexcept(std::is_nothrow_move_constructible<decltype(instances)>::value)
			: data(std::move(other.data))
			, instances(std::move(other.instances))
		{
		}
	};

	//! Alias for the internal reporesentation of drawables
	using InternalDrawableT = DrawableInternalData;
	//! Alias for the container used to store the drawables
	using InternalDrawableContainerT = ContainerT<DrawableInternalData>;

public:
	//! Clears all data buffers and removes all drawables
	/*
	 * Before this method can clear the container, an exclusive lock over the manager has to be acquired.
	 * Therefore, it may block until this was successful.
	 */
	void clear()
	{
		// Require exclusive access to the central mutex
		std::lock_guard<ManagerMutexT> lock(m_sharedManagerMutex);

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
		std::lock_guard<ManagerMutexT> lock(m_sharedManagerMutex);

		// Create a new drawable object
		m_drawables.emplace_back();
		DrawableInformation& drawableData = m_drawables.back().data;

		// Set drawing mode and check whether it is valid
		drawableData.glMode = drawable.glMode;
		assert(CommonOpenGl::isValidGlMode(drawable.glMode));

		// Make sure that all vertices of the new drawable are indexable using the index type
		assert(drawable.vertices.size()/bufferEntriesPerVertex < std::numeric_limits<IndexT>::max());

		// Copy vertices to normal buffer
		drawableData.vertexBufferOffset = static_cast<GLint>(m_vertexBuffer.size());
		drawableData.vertexCount = static_cast<GLsizei>(drawable.vertices.size());
		m_vertexBuffer.insert(m_vertexBuffer.end(), drawable.vertices.begin(), drawable.vertices.end());

		// Make sure that all vertices are indexable by OpenGL
		assert((m_vertexBuffer.size()/bufferEntriesPerVertex) < std::numeric_limits<GLint>::max());

		// Copy normals to normal buffer
		drawableData.normalBufferOffset = static_cast<GLint>(m_normalBuffer.size());
		drawableData.normalCount = static_cast<GLsizei>(drawable.normals.size());
		m_normalBuffer.insert(m_normalBuffer.end(), drawable.normals.begin(), drawable.normals.end());

		// Copy indices to index buffer
		drawableData.indexBufferOffset = static_cast<GLint>(m_indexBuffer.size());
		drawableData.indexCount = static_cast<GLsizei>(drawable.indices.size());
		m_indexBuffer.insert(m_indexBuffer.end(), drawable.indices.begin(), drawable.indices.end());

		// Store the index of the drawable's base vertex
		drawableData.baseVertex = drawableData.vertexBufferOffset/bufferEntriesPerVertex;
		drawableData.indexPtrOffset = (GLvoid*)(drawableData.indexBufferOffset * sizeof(typename DrawableSourceT::IndexT));

		// Return drawable id
		return static_cast<GLsizei>(m_drawables.size() - 1);
	}

	//! Creates a new instance of the specified drawable and stores the supplied instance data
	GLsizei storeInstance(GLsizei drawableId, const InstanceDataT& instanceData)
	{
		// Lock the drawable manager against clearing and reallocation
		std::shared_lock<ManagerMutexT> bufferLock(m_sharedManagerMutex);

		// Make sure that drawable exists
		assert(drawableId < m_drawables.size());

		// Get the corresponding drawable and lock it against simultaneous modification
		auto& drawable = m_drawables[drawableId];
		std::lock_guard<DrawableMutexT> lock(drawable.drawableMutex);

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
		std::shared_lock<ManagerMutexT> bufferLock(m_sharedManagerMutex);

		// Make sure that drawable exists
		assert(drawableId < m_drawables.size());

		// Get the corresponding drawable and lock it against simultaneous modification
		auto& drawable = m_drawables[drawableId];
		std::lock_guard<DrawableMutexT> lock(drawable.drawableMutex);

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
		std::shared_lock<ManagerMutexT> bufferLock(m_sharedManagerMutex);

		// Make sure that drawable exists
		assert(drawableId < m_drawables.size());

		// Get the corresponding drawable and lock it against simultaneous modification
		auto& drawable = m_drawables[drawableId];
		std::lock_guard<DrawableMutexT> lock(drawable.drawableMutex);

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
	std::shared_lock<ManagerMutexT> shared_lock() { return std::shared_lock<ManagerMutexT>(m_sharedManagerMutex); }

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
	 * corresponding to the drawable the iterator points to.
	 */
	DrawableIteratorT drawablesBegin() { return DrawableIteratorT(m_drawables.begin()); }
	//! Returns an iterator behind the end of the drawable container
	/*
	 * The iterator can be used to loop over all drawables. Marks the end of the container
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
	ManagerMutexT m_sharedManagerMutex;

	//! Container storing the drawables with buffer offsets and instance data
	InternalDrawableContainerT m_drawables;

	//! Buffer of vertices
	ContainerT<VertexT> m_vertexBuffer;
	//! Buffer of normals
	ContainerT<VertexT> m_normalBuffer;
	//! Buffer of indices
	ContainerT<IndexT> m_indexBuffer;
};

//! Proxy to give controlled access to drawables and their instace data buffers
/*
 * Instances of this proxy class can be obtained using the DrawableManager::drawable() method or by
 * dereferencing iterators obtained from the DrawableManager. The proxy provides access to instance
 * data and inherits from DrawableManager::DrawableInformation and therefore also provides buffer
 * offsets, etc.
 * When a proxy object for a specific drawable gets created, it automatically locks the drawable
 * against concurrent access. When the proxy instance is destroyed, the drawable gets unlocked. As
 * the proxy contains a mutex lock, it cannot be copied, etc. but only moved.
 */
template <typename DrawableManagerT>
class DrawableProxy :
		private std::unique_lock<typename DrawableManagerT::DrawableMutexT>,
		public DrawableManagerT::DrawableInformation
{
	// Friend associated classes in order to allow access to private constructor
	friend DrawableManagerT;
	friend DrawableIterator<DrawableManagerT>;

public:
	//! Default move constructor
	DrawableProxy(DrawableProxy&& other) = default;
	//! Default move assignment
	DrawableProxy& operator=(DrawableProxy&& other) = default;

	//! Returns the number of instances of this drawable
	GLsizei instanceCount() const { return static_cast<GLsizei>(m_target.instances.size()); }
	//! Returns the total size in bytes of the instance data buffer of this drawable
	std::size_t instanceDataSize() const { return m_target.instances.size() * sizeof(typename DrawableManagerT::InstanceDataT); }
	//! Returns a pointer to the instance data buffer of this drawable
	typename DrawableManagerT::InstanceDataT* instanceData() { return m_target.instances.data(); }
	//! Returns a pointer to the const instance data buffer of this drawable
	const typename DrawableManagerT::InstanceDataT* instanceData() const { return m_target.instances.data(); }

private:
	//! Private constructor, automatically locks the drawable's mutex
	DrawableProxy(typename DrawableManagerT::InternalDrawableT& target)
		: std::unique_lock<typename DrawableManagerT::DrawableMutexT>(target.drawableMutex)
		, DrawableManagerT::DrawableInformation(target.data)
		, m_target(target)
	{
	}

	//! Reference of the internal drawable associated to this proxy
	typename DrawableManagerT::InternalDrawableT& m_target;
};

//! Iterator used to iterator over drawables of a DrawableManager, dereferences to DrawableProxy instances
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
	DrawableIterator(const typename DrawableManagerT::InternalDrawableContainerT::iterator& it) : it(it) {}

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
	typename DrawableManagerT::InternalDrawableContainerT::iterator it;
};

// Iterator trait specialization for the DrawableIterator
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
