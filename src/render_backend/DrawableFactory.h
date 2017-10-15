#pragma once

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

private:
	//! Calculates the indices for a vertex array, assuming duplicate vertices for adjacent primitives.
	static std::vector<DrawableSource::IndexT> calculateIndices(
			const std::vector<DrawableSource::VertexT>& vertices);
	//! Calculates triangle normals for a vertex array,assuming duplicate vertices for adjacent primitives.
	static std::vector<DrawableSource::VertexT> calculateTriangleNormals(
			const std::vector<DrawableSource::VertexT>& vertices,
			const std::vector<DrawableSource::IndexT>& indices);
};
