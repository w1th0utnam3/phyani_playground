#pragma once

#include <vector>

#include "CommonOpenGl.h"

//! Factory for various primitive drawable objects
class DrawableFactory
{
public:

	struct DrawableSource
	{
		//! Type used to represent vertex components
		using VertexT = glm::fvec3;
		//! Type used to represent vertx indices
		using IndexT = GLushort;

		//! The number of instances of VertexT to represent one vertex in OpenGL
		static constexpr GLubyte BufferEntriesPerVertex = 1;
		//! Type of the index values
		static constexpr GLenum glIndexType = GL_UNSIGNED_SHORT;

		//! OpenGL drawing mode for the drawable
		GLenum glMode;
		//! Vertices representing the drawable
		std::vector<VertexT> vertices;
		//! Normals for the drawable
		std::vector<VertexT> normals;
		//! Indices required to draw the drawable in element mode
		std::vector<IndexT> indices;
	};

	//! Returns a drawable with vertices and normals representing a cube.
	static DrawableSource createCube();
	//! Returns a drawable with vertices representing a sphere.
	static DrawableSource createSphere(const int recursionLevel = 2);

private:
	//! Calculates normals of a triangle mesh by using normals averaged over adjacent triangles
	static std::vector<DrawableSource::VertexT> calculateAveragedTriangleNormals(
			const std::vector<DrawableSource::VertexT>& vertices,
			const std::vector<DrawableSource::IndexT>& indices);
};
