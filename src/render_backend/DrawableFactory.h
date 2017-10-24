#pragma once

#include <vector>

#include "CommonOpenGl.h"

//! Factory for various primitive drawable objects
class DrawableFactory
{
public:
	//! Type returned by the factory functions containing the computed values and type information
	struct DrawableSource
	{
		//! Type used to represent vertex components
		using VertexT = glm::fvec3;
		//! Type used to represent vertx indices
		using IndexT = GLuint;

		//! The number of instances of VertexT to represent one vertex in OpenGL
		static constexpr GLubyte bufferEntriesPerVertex = 1;
		//! Type of the index values
		static constexpr GLenum glIndexType = GL_UNSIGNED_INT;

		//! OpenGL drawing mode for the drawable
		GLenum glMode;
		//! Vertices representing the drawable
		std::vector<VertexT> vertices;
		//! Normals for the drawable
		std::vector<VertexT> normals;
		//! Indices required to draw the drawable in element mode
		std::vector<IndexT> indices;
	};

	//! Returns a drawable that represents a simple line.
	static DrawableSource createLine();
	//! Returns a drawable with vertices and normals representing a cube.
	static DrawableSource createCube();
	//! Returns a drawable with vertices representing a sphere.
	static DrawableSource createSphere(const int recursionLevel = 2);
	//! Returns a drawable created from an obj file.
	static DrawableSource createFromObj(const std::string& objFilename);

	//! Returns a model matrix to transform the simple line drawable into a line connecting arbitrary points.
	static glm::fmat4 transformLine(const glm::fvec3& pStart, const glm::fvec3& pEnd);

private:
	//! Calculates normals of a triangle mesh by using normals averaged over adjacent triangles.
	static std::vector<DrawableSource::VertexT> calculateAveragedTriangleNormals(
			const std::vector<DrawableSource::VertexT>& vertices,
			const std::vector<DrawableSource::IndexT>& indices);
};
