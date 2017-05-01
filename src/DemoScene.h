#pragma once

#include "Scene.h"

#include <Eigen/Geometry>

class DemoScene : public Scene
{
public:
	DemoScene() = default;
	~DemoScene() = default;

protected:
	virtual void initializeSceneContent();
	virtual void renderSceneContent();

private:
	static void initializeLight();
	static void drawCoordinateSystem(double axisLength);
	static void drawTriangle(const Eigen::Vector3d &a, const Eigen::Vector3d &b, const Eigen::Vector3d &c, const Eigen::Vector3d &norm, const float *color);
	static void drawTetrahedron(const Eigen::Vector3d &a, const Eigen::Vector3d &b, const Eigen::Vector3d &c, const Eigen::Vector3d &d, const float *color);
};
