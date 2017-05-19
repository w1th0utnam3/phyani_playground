#pragma once

#include <Eigen/Geometry>

#include "Scene.h"
#include "TimeSteppedSimulation.h"

class DemoScene : public Scene
{
public:
	DemoScene() = default;
	virtual ~DemoScene();

	void doTimestep(double dt);

protected:
	virtual void initializeSceneContent();
	virtual void renderSceneContent();

private:
	static void initializeLight();
	static void drawCoordinateSystem(double axisLength);
	static void drawTriangle(const Eigen::Vector3d &a, const Eigen::Vector3d &b, const Eigen::Vector3d &c, const Eigen::Vector3d &norm, const float *color);
	static void drawTetrahedron(const Eigen::Vector3d &a, const Eigen::Vector3d &b, const Eigen::Vector3d &c, const Eigen::Vector3d &d, const float *color);

	TimesteppedSimulation m_simulation;
	std::thread m_simulationThread;
};
