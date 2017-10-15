#pragma once

#include <cassert>
#include <limits>
#include <mutex>
#include <shared_mutex>
#include <vector>
#include <utility>
#include <type_traits>
#include <iterator>

#include "CommonOpenGl.h"
#include "DrawableFactory.h"

template <typename InstanceDataT, typename DrawableSourceT = DrawableFactory::DrawableSource>
class DrawableManager
{
private:
	using VertexT = typename DrawableSourceT::VertexT;
	using IndexT = typename DrawableSourceT::IndexT;

public:

	struct DrawableData
	{
		GLenum mode;

		IndexT vertexBufferOffset;
		IndexT vertexCount;

		IndexT normalBufferOffset;
		IndexT normalCount;

		IndexT indexBufferOffset;
		IndexT indexCount;
	};

private:
	using MutexT = std::mutex;
	using SharedMutexT = std::shared_timed_mutex;

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
	class DrawableProxy : private std::unique_lock<MutexT>, public DrawableData
	{
	public:
		DrawableProxy(Drawable& target)
			: std::unique_lock<MutexT>(target.drawableMutex)
			, DrawableData(target.data)
			, m_target(target)
		{
		}

		DrawableProxy(DrawableProxy&& other) = default;
		DrawableProxy& operator=(DrawableProxy&& other) = default;

		IndexT instanceCount() const { return static_cast<IndexT>(m_target.instances.size()); }
		std::size_t instanceDataSize() const { return m_target.instances.size() * sizeof(InstanceDataT); }
		InstanceDataT* instanceData() { return m_target.instances.data(); }
		const InstanceDataT* instanceData() const { return m_target.instances.data(); }

	private:
		Drawable& m_target;
	};

	class DrawableIterator
	{
	public:
		using difference_type = IndexT;
		using value_type = DrawableProxy;
		using pointer = DrawableProxy*;
		using reference = DrawableProxy&;
		using iterator_category = std::forward_iterator_tag;

		DrawableIterator() = default;
		DrawableIterator(const typename ContainerT<Drawable>::iterator& it) : it(it) {}

		bool operator==(const DrawableIterator& other) const { return it == other.it; }
		bool operator!=(const DrawableIterator& other) const { return it != other.it; }
		bool operator<(const DrawableIterator& other) const { return it < other.it; }
		bool operator>(const DrawableIterator& other) const { return it > other.it; }

		DrawableIterator& operator++() { ++it; return *this; }
		DrawableIterator& operator--() { --it; return *this; }

		DrawableIterator& operator++(int) { auto tmp = *this; ++it; return tmp; }
		DrawableIterator& operator--(int) { auto tmp = *this; --it; return tmp; }

		DrawableProxy operator*() { return DrawableProxy(*it); }

	private:
		typename ContainerT<Drawable>::iterator it;
	};

	void clear()
	{
		std::lock_guard<SharedMutexT> lock(m_bufferMutex);

		m_vertexBuffer.clear();
		m_normalBuffer.clear();
		m_indexBuffer.clear();
		m_drawables.clear();
	}

	IndexT registerDrawable(const DrawableSourceT& drawable)
	{
		std::lock_guard<SharedMutexT> lock(m_bufferMutex);

		m_drawables.emplace_back();
		auto& drawableData = m_drawables.back().data;

		drawableData.mode = drawable.mode;
		assert(CommonOpenGl::isValidGlMode(drawable.mode));

		drawableData.vertexBufferOffset = static_cast<IndexT>(m_vertexBuffer.size());
		drawableData.vertexCount = static_cast<IndexT>(drawable.vertices.size());
		m_vertexBuffer.insert(m_vertexBuffer.end(), drawable.vertices.begin(), drawable.vertices.end());

		assert(m_vertexBuffer.size() < std::numeric_limits<IndexT>::max());

		drawableData.normalBufferOffset = static_cast<IndexT>(m_normalBuffer.size());
		drawableData.normalCount = static_cast<IndexT>(drawable.normals.size());
		m_normalBuffer.insert(m_normalBuffer.end(), drawable.normals.begin(), drawable.normals.end());

		drawableData.indexBufferOffset = static_cast<IndexT>(m_indexBuffer.size());
		drawableData.indexCount = static_cast<IndexT>(drawable.indices.size());
		m_indexBuffer.insert(m_indexBuffer.end(), drawable.indices.begin(), drawable.indices.end());

		for (IndexT i = drawableData.indexBufferOffset; i < drawableData.indexBufferOffset + drawableData.indexCount; i++) {
			m_indexBuffer[i] += drawableData.vertexBufferOffset;
		}

		// Return drawable id
		return static_cast<IndexT>(m_drawables.size() - 1);
	}

	IndexT storeInstance(IndexT drawableId, const InstanceDataT& instanceData)
	{
		std::shared_lock<SharedMutexT> bufferLock(m_bufferMutex);

		auto& drawable = m_drawables[drawableId];
		std::lock_guard<MutexT> lock(drawable.drawableMutex);

		const std::size_t instanceOffset = drawable.instances.size();
		assert(instanceOffset + 1 < std::numeric_limits<IndexT>::max());

		drawable.instances.push_back(instanceData);

		// Return instance id
		return instanceOffset;
	}

	IndexT createInstances(IndexT drawableId, IndexT instanceCount)
	{
		std::shared_lock<SharedMutexT> bufferLock(m_bufferMutex);

		auto& drawable = m_drawables[drawableId];
		std::lock_guard<MutexT> lock(drawable.drawableMutex);

		const std::size_t instanceOffset = drawable.instances.size();
		assert(instanceOffset + static_cast<std::size_t>(instanceCount) < std::numeric_limits<IndexT>::max());

		drawable.instances.resize(instanceOffset + instanceCount);

		// Return id of first created instance
		return static_cast<IndexT>(instanceOffset);
	}

	void destroyInstance(IndexT drawableId, IndexT instanceId)
	{
		std::shared_lock<SharedMutexT> bufferLock(m_bufferMutex);

		auto& drawable = m_drawables[drawableId];
		std::lock_guard<MutexT> lock(drawable.drawableMutex);

		drawable.instances.erase(instanceId);
	}

	std::shared_lock<SharedMutexT> shared_lock() { return std::shared_lock<SharedMutexT>(m_bufferMutex); }

	IndexT vertexCount() const { return static_cast<IndexT>(m_vertexBuffer.size()); }
	IndexT normalCount() const { return static_cast<IndexT>(m_normalBuffer.size()); }
	IndexT indexCount() const { return static_cast<IndexT>(m_indexBuffer.size()); }

	std::size_t vertexBufferSize() const { return m_vertexBuffer.size() * sizeof(VertexT); }
	std::size_t normalBufferSize() const { return m_normalBuffer.size() * sizeof(VertexT); }
	std::size_t indexBufferSize() const { return m_indexBuffer.size() * sizeof(IndexT); }

	VertexT* vertexBufferData() { return m_vertexBuffer.data(); }
	VertexT* normalBufferData() { return m_normalBuffer.data(); }
	IndexT* indexBufferData() { return m_indexBuffer.data(); }

	DrawableProxy drawable(IndexT drawableId) { return DrawableProxy(m_drawables[drawableId]); }
	DrawableIterator drawablesBegin() { return DrawableIterator(m_drawables.begin()); }
	DrawableIterator drawablesEnd() { return DrawableIterator(m_drawables.end()); }

private:
	SharedMutexT m_bufferMutex;

	ContainerT<Drawable> m_drawables;

	ContainerT<VertexT> m_vertexBuffer;
	ContainerT<VertexT> m_normalBuffer;
	ContainerT<IndexT> m_indexBuffer;
};
