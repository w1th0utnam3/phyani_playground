#pragma once

#include <Eigen/Geometry>

#include "EntityComponentSystem.h"

class RenderSystem
{
public:
	RenderSystem(EntityComponentSystem& ecs);

	void render();

private:
	EntityComponentSystem& m_ecs;

	static void drawTriangle(const Eigen::Vector3d &a, const Eigen::Vector3d &b, const Eigen::Vector3d &c, const Eigen::Vector3d &norm, const float *color);
	static void drawTetrahedron(const Eigen::Vector3d &a, const Eigen::Vector3d &b, const Eigen::Vector3d &c, const Eigen::Vector3d &d, const float *color);
	static void drawCube(const double edgeLength, const float *color);
};
