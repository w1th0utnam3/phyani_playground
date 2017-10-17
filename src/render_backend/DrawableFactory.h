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
		using VertexT = GLfloat;
		//! Type used to represent vertx indices
		using IndexT = GLuint;

		//! The OpenGL drawing mode for the drawable
		GLenum mode;
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
	//! Calculates the indices for a vertex array, assuming duplicate vertices for adjacent primitives.
	static std::vector<DrawableSource::IndexT> calculateIndices(
			const std::vector<DrawableSource::VertexT>& vertices);
	//! Calculates triangle normals for a vertex array,assuming duplicate vertices for adjacent primitives.
	static std::vector<DrawableSource::VertexT> calculateTriangleNormalsPerVertex(
			const std::vector<DrawableSource::VertexT>& vertices,
			const std::vector<DrawableSource::IndexT>& indices);
	static std::vector<DrawableSource::VertexT> calculateTriangleNormalsPerTriangle(
			const std::vector<DrawableSource::VertexT>& vertices,
			const std::vector<DrawableSource::IndexT>& indices);
};
