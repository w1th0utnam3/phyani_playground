#include "DrawableFactory.h"

DrawableFactory::DrawableSource DrawableFactory::createCube()
{
	DrawableSource drawable;

	drawable.mode = GL_TRIANGLES;
	drawable.vertices = {
		-0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f, 0.5f,
		-0.5f, 0.5f, 0.5f,

		 0.5f, 0.5f,-0.5f,
		-0.5f,-0.5f,-0.5f,
		-0.5f, 0.5f,-0.5f,

		 0.5f,-0.5f, 0.5f,
		-0.5f,-0.5f,-0.5f,
		 0.5f,-0.5f,-0.5f,

		 0.5f, 0.5f,-0.5f,
		 0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,-0.5f,

		-0.5f,-0.5f,-0.5f,
		-0.5f, 0.5f, 0.5f,
		-0.5f, 0.5f,-0.5f,

		 0.5f,-0.5f, 0.5f,
		-0.5f,-0.5f, 0.5f,
		-0.5f,-0.5f,-0.5f,

		-0.5f, 0.5f, 0.5f,
		-0.5f,-0.5f, 0.5f,
		 0.5f,-0.5f, 0.5f,

		 0.5f, 0.5f, 0.5f,
		 0.5f,-0.5f,-0.5f,
		 0.5f, 0.5f,-0.5f,

		 0.5f,-0.5f,-0.5f,
		 0.5f, 0.5f, 0.5f,
		 0.5f,-0.5f, 0.5f,

		 0.5f, 0.5f, 0.5f,
		 0.5f, 0.5f,-0.5f,
		-0.5f, 0.5f,-0.5f,

		 0.5f, 0.5f, 0.5f,
		-0.5f, 0.5f,-0.5f,
		-0.5f, 0.5f, 0.5f,

		 0.5f, 0.5f, 0.5f,
		-0.5f, 0.5f, 0.5f,
		 0.5f,-0.5f, 0.5f,
	};

	drawable.indices = calculateIndices(drawable.vertices);
	drawable.normals = calculateTriangleNormals(drawable.vertices, drawable.indices);

	return drawable;
}

std::vector<DrawableFactory::DrawableSource::IndexT> DrawableFactory::calculateIndices(
		const std::vector<DrawableSource::VertexT>& vertices)
{
	std::vector<DrawableSource::IndexT> indices;
	indices.resize(vertices.size() / 3);

	for (DrawableSource::IndexT i = 0; i < indices.size(); i++)
		indices[i] = i;

	return indices;
}

std::vector<DrawableFactory::DrawableSource::VertexT> DrawableFactory::calculateTriangleNormals(
		const std::vector<DrawableSource::VertexT>& vertices,
		const std::vector<DrawableSource::IndexT>& indices)
{
	// TODO: Check for indexed objects where 'vertices.size() != indices.size()*3'

	std::vector<DrawableSource::VertexT> normals;
	normals.resize(vertices.size());

	for (DrawableSource::IndexT i = 0; i < indices.size(); i+=3) {
		const auto v1 = reinterpret_cast<const glm::fvec3*>(&vertices[3*indices[i + 0]]);
		const auto v2 = reinterpret_cast<const glm::fvec3*>(&vertices[3*indices[i + 1]]);
		const auto v3 = reinterpret_cast<const glm::fvec3*>(&vertices[3*indices[i + 2]]);

		auto normalsPerVertex = reinterpret_cast<glm::fvec3*>(&normals[3*i + 0]);

		glm::fvec3 edge1 = *v2 - *v3;
		glm::fvec3 edge2 = *v3 - *v1;
		glm::fvec3 normal = glm::normalize(glm::cross(edge1, edge2));

		normalsPerVertex[0] = normal;
		normalsPerVertex[1] = normal;
		normalsPerVertex[2] = normal;
	}

	return normals;
}
