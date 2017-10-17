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
	drawable.normals = calculateTriangleNormalsPerVertex(drawable.vertices, drawable.indices);

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

std::vector<DrawableFactory::DrawableSource::VertexT> DrawableFactory::calculateTriangleNormalsPerVertex(
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

std::vector<DrawableFactory::DrawableSource::VertexT> DrawableFactory::calculateTriangleNormalsPerTriangle(
		const std::vector<DrawableSource::VertexT>& vertices,
		const std::vector<DrawableSource::IndexT>& indices)
{
	// TODO: Check for indexed objects where 'vertices.size() != indices.size()*3'

	std::vector<DrawableSource::VertexT> normals;
	normals.resize(indices.size() / 3);

	for (DrawableSource::IndexT i = 0; i < indices.size(); i+=3) {
		const auto v1 = reinterpret_cast<const glm::fvec3*>(&vertices[3*indices[i + 0]]);
		const auto v2 = reinterpret_cast<const glm::fvec3*>(&vertices[3*indices[i + 1]]);
		const auto v3 = reinterpret_cast<const glm::fvec3*>(&vertices[3*indices[i + 2]]);

		auto normal = reinterpret_cast<glm::fvec3*>(&normals[i]);

		glm::fvec3 edge1 = *v2 - *v3;
		glm::fvec3 edge2 = *v3 - *v1;
		*normal = glm::normalize(glm::cross(edge1, edge2));
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

DrawableFactory::DrawableSource DrawableFactory::createSphere()
{
	DrawableSource drawable;

	IcoSphereCreator generator;
	auto mesh = generator.create(2);

	drawable.vertices.resize(mesh.positions.size() * 3);
	std::memcpy(drawable.vertices.data(), mesh.positions.data(), sizeof(GLfloat) * drawable.vertices.size());

	drawable.indices.resize(mesh.triangleIndices.size());
	for (std::size_t i = 0; i < mesh.triangleIndices.size(); i++) {
		drawable.indices[i] = mesh.triangleIndices[i];
	}

	drawable.normals = calculateTriangleNormalsPerTriangle(drawable.vertices, drawable.indices);

	return drawable;
}
