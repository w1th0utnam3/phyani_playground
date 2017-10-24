#include "DrawableFactory.h"

#include <numeric>
#include <cstring>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

DrawableFactory::DrawableSource DrawableFactory::createLine()
{
	DrawableSource drawable;
	drawable.glMode = GL_LINES;

	// Use x-axis as base line
	drawable.vertices = {glm::fvec3(0.0f, 0.0f, 0.0f), glm::fvec3(1.0f, 0.0f, 0.0f)};
	drawable.normals = {glm::fvec3(0.0f, 1.0f, 0.0f), glm::fvec3(0.0f, 1.0f, 0.0f)};
	drawable.indices = {0, 1};

	return drawable;
}

glm::fmat4 DrawableFactory::transformLine(const glm::fvec3& pStart, const glm::fvec3& pEnd)
{
	glm::fmat4 transform(1.0f);

	// Translate start point into coordinate origin
	glm::fvec3 line = pEnd - pStart;
	float length = glm::length(line);
	glm::fvec3 normalizedLineDirection = (1.0f / length) * line;

	// Rotation axis: cross product of x-axis and the normalized line
	glm::fvec3 axis(0.0f, normalizedLineDirection.z, -normalizedLineDirection.y);
	// Rotation angle: dot product of x-axis with the normalized line
	float angle = std::acos(normalizedLineDirection.x);

	// Calculate and compose transformation matrices
	transform = glm::translate(transform, pStart);
	transform = glm::rotate(transform, angle, axis);
	transform = glm::scale(transform, glm::fvec3(length, 1.0f, 1.0f));

	return transform;
}

DrawableFactory::DrawableSource DrawableFactory::createCube()
{
	DrawableSource drawable;
	drawable.glMode = GL_TRIANGLES;

	static const GLfloat vertices[] = {
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

	// Copy the vertex array into the vertex vector
	drawable.vertices.resize(sizeof(vertices)/(sizeof(GLfloat)*3));
	std::memcpy(drawable.vertices.data(), &vertices[0], sizeof(vertices));

	// Fill index array with incrementing ints
	drawable.indices.resize(drawable.vertices.size());
	std::iota(drawable.indices.begin(), drawable.indices.end(), 0);

	drawable.normals = calculateAveragedTriangleNormals(drawable.vertices, drawable.indices);

	return drawable;
}

std::vector<DrawableFactory::DrawableSource::VertexT> DrawableFactory::calculateAveragedTriangleNormals(
		const std::vector<DrawableSource::VertexT>& vertices,
		const std::vector<DrawableSource::IndexT>& indices)
{
	std::vector<DrawableSource::VertexT> normals;
	normals.resize(vertices.size(), glm::fvec3(0.0f, 0.0f, 0.0f));

	// Make sure that we can index all vertices
	assert(indices.size() < std::numeric_limits<unsigned int>::max());

	// Keep track of the number of accumulated normals per vertex
	std::vector<int> normalsPerVertex(vertices.size(), 0);

	for (unsigned int i = 0; i < indices.size(); i+=3) {
		// Get all vertices of the triangle
		const glm::fvec3* triVerts[3] = {
			&vertices[indices[i + 0]],
			&vertices[indices[i + 1]],
			&vertices[indices[i + 2]]
		};

		// Calculate the normal of the triangle
		glm::fvec3 edge1 = *triVerts[1] - *triVerts[2];
		glm::fvec3 edge2 = *triVerts[2] - *triVerts[0];
		glm::fvec3 normal = glm::normalize(glm::cross(edge1, edge2));

		// Get the normals of the triangle's vertices
		glm::fvec3* triNormals[3] = {
			&normals[indices[i + 0]],
			&normals[indices[i + 1]],
			&normals[indices[i + 2]]
		};

		// Add the new normal to the rolling averge of each vertex
		for (int j = 0; j < 3; j++) {
			glm::fvec3& n = *triNormals[j];

			// Remove previous averaging factor
			n *= normalsPerVertex[indices[i + j]];
			// Add newly calculated normal
			n += normal;
			// Average again with new normal count
			n /= ++normalsPerVertex[indices[i + j]];
			// Normalize
			n = glm::normalize(n);
		}
	}

	return normals;
}

#include <map>
#include <cmath>
#include <utility>

class IcoSphereCreator
{
	// Code adapted from: http://blog.andreaskahler.com/2009/06/creating-icosphere-mesh-in-code.html

	using Point3D = glm::fvec3;
	using Int64 = std::size_t;

	struct TriangleIndices
	{
		int v1;
		int v2;
		int v3;

		TriangleIndices() = default;
		TriangleIndices(int v1, int v2, int v3) : v1(v1), v2(v2), v3(v3) {}
	};

public:
	struct MeshGeometry3D
	{
		std::vector<Point3D> positions;
		std::vector<int> triangleIndices;
	};

private:
	MeshGeometry3D geometry;
	int index;
	std::map<Int64, int> middlePointIndexCache;

	// add vertex to mesh, fix position to be on unit sphere, return index
	int addVertex(Point3D p)
	{
		double length = std::sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
		geometry.positions.emplace_back(p.x / length, p.y / length, p.z / length);
		return index++;
	}

	// return index of point in the middle of p1 and p2
	int getMiddlePoint(int p1, int p2)
	{
		// first check if we have it already
		bool firstIsSmaller = p1 < p2;
		Int64 smallerIndex = firstIsSmaller ? p1 : p2;
		Int64 greaterIndex = firstIsSmaller ? p2 : p1;
		Int64 key = (smallerIndex << 32) + greaterIndex;

		{
			auto it = middlePointIndexCache.find(key);
			if (it != middlePointIndexCache.end()) {
				return it->second;
			}
		}

		// not in cache, calculate it
		Point3D point1 = geometry.positions[p1];
		Point3D point2 = geometry.positions[p2];
		Point3D middle = Point3D(
			(point1.x + point2.x) / 2.0,
			(point1.y + point2.y) / 2.0,
			(point1.z + point2.z) / 2.0);

		// add vertex makes sure point is on unit sphere
		int i = addVertex(middle);

		// store it, return index
		middlePointIndexCache.insert(std::make_pair(key, i));
		return i;
	}

public:
	MeshGeometry3D create(int recursionLevel)
	{
		geometry = MeshGeometry3D();
		middlePointIndexCache.clear();
		index = 0;

		// create 12 vertices of a icosahedron
		double t = (1.0 + std::sqrt(5.0)) / 2.0;

		addVertex(Point3D(-1, t, 0));
		addVertex(Point3D(1, t, 0));
		addVertex(Point3D(-1, -t, 0));
		addVertex(Point3D(1, -t, 0));

		addVertex(Point3D(0, -1, t));
		addVertex(Point3D(0, 1, t));
		addVertex(Point3D(0, -1, -t));
		addVertex(Point3D(0, 1, -t));

		addVertex(Point3D(t, 0, -1));
		addVertex(Point3D(t, 0, 1));
		addVertex(Point3D(-t, 0, -1));
		addVertex(Point3D(-t, 0, 1));

		// create 20 triangles of the icosahedron
		auto faces = std::vector<TriangleIndices>();
		faces.reserve(20);

		// 5 faces around point 0
		faces.emplace_back(0, 11, 5);
		faces.emplace_back(0, 5, 1);
		faces.emplace_back(0, 1, 7);
		faces.emplace_back(0, 7, 10);
		faces.emplace_back(0, 10, 11);

		// 5 adjacent faces 
		faces.emplace_back(1, 5, 9);
		faces.emplace_back(5, 11, 4);
		faces.emplace_back(11, 10, 2);
		faces.emplace_back(10, 7, 6);
		faces.emplace_back(7, 1, 8);

		// 5 faces around point 3
		faces.emplace_back(3, 9, 4);
		faces.emplace_back(3, 4, 2);
		faces.emplace_back(3, 2, 6);
		faces.emplace_back(3, 6, 8);
		faces.emplace_back(3, 8, 9);

		// 5 adjacent faces 
		faces.emplace_back(4, 9, 5);
		faces.emplace_back(2, 4, 11);
		faces.emplace_back(6, 2, 10);
		faces.emplace_back(8, 6, 7);
		faces.emplace_back(9, 8, 1);

		// refine triangles
		for (int i = 0; i < recursionLevel; i++)
		{
			auto faces2 = std::vector<TriangleIndices>();
			faces2.reserve(faces.size() * 4);

			for(const auto& tri : faces)
			{
				// replace triangle by 4 triangles
				int a = getMiddlePoint(tri.v1, tri.v2);
				int b = getMiddlePoint(tri.v2, tri.v3);
				int c = getMiddlePoint(tri.v3, tri.v1);

				faces2.emplace_back(tri.v1, a, c);
				faces2.emplace_back(tri.v2, b, a);
				faces2.emplace_back(tri.v3, c, b);
				faces2.emplace_back(a, b, c);
			}
			faces = faces2;
		}

		// done, now add triangles to mesh
		geometry.triangleIndices.reserve(faces.size() * 3);
		for(const auto& tri : faces)
		{
			geometry.triangleIndices.push_back(tri.v1);
			geometry.triangleIndices.push_back(tri.v2);
			geometry.triangleIndices.push_back(tri.v3);
		}

		return geometry;
	}
};

DrawableFactory::DrawableSource DrawableFactory::createSphere(const int recursionLevel)
{
	DrawableSource drawable;
	drawable.glMode = GL_TRIANGLES;

	{
		auto mesh = IcoSphereCreator().create(recursionLevel);

		// Make sure that all vertices can be indexed with the specified index type
		assert(mesh.positions.size() < std::numeric_limits<DrawableSource::IndexT>::max());

		drawable.vertices = mesh.positions;

		drawable.indices.resize(mesh.triangleIndices.size());
		for (std::size_t i = 0; i < mesh.triangleIndices.size(); i++) {
			drawable.indices[i] = mesh.triangleIndices[i];
		}
	}

	drawable.normals = calculateAveragedTriangleNormals(drawable.vertices, drawable.indices);

	return drawable;
}

DrawableFactory::DrawableSource DrawableFactory::createFromObj(const std::string& objFilename)
{
	DrawableSource drawable;
	drawable.glMode = GL_TRIANGLES;

	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;
	std::string base_dir = "";

	static_assert(std::is_same<decltype(attrib.vertices)::value_type, float>::value, "Vertex type has to be float!");
	static_assert(std::is_same<decltype(attrib.normals)::value_type, float>::value, "Normal type has to be float!");

	bool result = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, objFilename.c_str(), base_dir.c_str(), false);

	if (!result) {
		std::cerr << err;
	} else {
		// Make sure that all vertices can be indexed with the specified index type
		assert(attrib.vertices.size() / 3 < std::numeric_limits<DrawableSource::IndexT>::max());
		assert(shapes.size() > 0);

		// Make room for vertices and normals
		drawable.vertices.resize(attrib.vertices.size() / 3, glm::fvec3{1.0f, 0.0f, 0.0f});
		drawable.normals.resize(attrib.vertices.size() / 3, glm::fvec3{1.0f, 0.0f, 0.0f});

		// Copy vertices to drawable
		std::memcpy(drawable.vertices.data(), attrib.vertices.data(), attrib.vertices.size() * sizeof(float));

		const auto& mesh = shapes[0].mesh;
		drawable.indices.resize(mesh.indices.size(), 0);
		for (std::size_t i = 0; i < mesh.indices.size(); i++) {
			const auto vi = mesh.indices[i].vertex_index;
			const auto ni = mesh.indices[i].normal_index;

			// Store the vertex index
			drawable.indices[i] = vi;
			// Fix ordering of normals to correspond to vertex ordering
			std::memcpy(&drawable.normals[vi], &attrib.normals[3*ni], 3 * sizeof(float));
		}
	}

	return drawable;
}
