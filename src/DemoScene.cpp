#include "DemoScene.h"

#include <iostream>
#include <array>

#include <glad/glad.h>
#include <Eigen/Geometry>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "RigidBodyComponents.h"

DemoScene::DemoScene(EntityComponentSystem& ecs)
	: m_ecs(ecs)
	, m_animationSystem(ecs)
	, m_renderSystem(ecs)
{
}

DemoScene::~DemoScene()
{
	//if (m_simulation.isEventLoopRunning()) m_simulation.stopTimestepLoop().wait();
	//m_simulationThread.join();
}

void DemoScene::initializeSceneContent()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.95f, 0.95f, 1.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	initializeLight();
	initializeEntities();

	/*
	auto simulationPtr = &m_simulation;
	m_simulationThread = std::move(std::thread([simulationPtr]()
	{
		simulationPtr->executeTimestepLoop();
	}));
	*/
}

void DemoScene::doTimestep(double dt)
{
	m_animationSystem.computeTimestep(dt);
	//m_simulation.requestTimestep(dt);
}

void DemoScene::renderSceneContent()
{
	glMatrixMode(GL_PROJECTION);
	const auto projection = m_camera->projectionMatrix();
	glLoadMatrixd(glm::value_ptr(projection));
	glMatrixMode(GL_MODELVIEW);
	const auto transform = m_camera->modelViewMatrix();
	glLoadMatrixd(glm::value_ptr(transform));

	drawCoordinateSystem(2);
	static const std::array<float, 4> green{ 0.0f, 1.0f, 0.0f, 1.0f };
	static const std::array<float, 4> blue{ 0.0f, 0.0f, 1.0f, 1.0f };

	drawTetrahedron(Eigen::Vector3d::Zero(), Eigen::Vector3d::UnitX(), Eigen::Vector3d::UnitY(), Eigen::Vector3d::UnitZ(), green.data());
}

void DemoScene::initializeEntities()
{
	// Create a rigid body
	auto body = m_ecs.create<DynamicBody, Position, Velocity, Kinetics, RigidBodyProperties, Joints>();
	m_ecs.get<RigidBodyProperties>(body).mass = 1.1;

	// Create a fixed point
	auto particle = m_ecs.create<Position, Joints>();
	m_ecs.get<Position>(particle).translation.y() = 2.0;

	// Create a spring for the connection
	auto spring = m_ecs.create<JointProperties>();
	auto& springProperties = m_ecs.get<JointProperties>(body);
	//springProperties.entityA = body;
	//springProperties.entityB = particle;
	springProperties.restLength = 1.0;
	springProperties.elasticity = 0.1;
	springProperties.damping = 0.05;

	m_ecs.get<Joints>(body).joints.push_back(spring);
	m_ecs.get<Joints>(particle).joints.push_back(spring);
}

void DemoScene::initializeLight()
{
	float t = 0.4f;
	float a = 0.1f;

	float amb0[4] = {a,a,a,1};
	float diff0[4] = {t,t,t,1};
	float spec0[4] = {1,1,1,1};
	float pos0[4] = {-10,10,10,1};
	glLightfv(GL_LIGHT0, GL_AMBIENT, amb0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diff0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, spec0);
	glLightfv(GL_LIGHT0, GL_POSITION, pos0);
	glEnable(GL_LIGHT0);

	float amb1[4] = {a,a,a,1};
	float diff1[4] = {t,t,t,1};
	float spec1[4] = {1,1,1,1};
	float pos1[4] = {10,10,10,1};
	glLightfv(GL_LIGHT1, GL_AMBIENT, amb1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diff1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, spec1);
	glLightfv(GL_LIGHT1, GL_POSITION, pos1);
	glEnable(GL_LIGHT1);

	float amb2[4] = {a,a,a,1};
	float diff2[4] = {t,t,t,1};
	float spec2[4] = {1,1,1,1};
	float pos2[4] = {0,10,10,1};
	glLightfv(GL_LIGHT2, GL_AMBIENT, amb2);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, diff2);
	glLightfv(GL_LIGHT2, GL_SPECULAR, spec2);
	glLightfv(GL_LIGHT2, GL_POSITION, pos2);
	glEnable(GL_LIGHT2);

	glEnable(GL_LIGHTING);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
}

void DemoScene::drawCoordinateSystem(double axisLength)
{
	static const Eigen::Vector3d origin(0, 0, 0);
	const Eigen::Vector3d xAxis(axisLength, 0, 0);
	const Eigen::Vector3d yAxis(0, axisLength, 0);
	const Eigen::Vector3d zAxis(0, 0, axisLength);

	static constexpr float SpecColorWhite[4] = {1,1,1,1};
	static constexpr float diffColorRed[4] = {1,0,0,1};
	static constexpr float diffColorGreen[4] = {0,1,0,1};
	static constexpr float diffColorBlue[4] = {0,0,1,1};

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, diffColorRed);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffColorRed);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, SpecColorWhite);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 100.0);
	glLineWidth(2);

	glBegin(GL_LINES);
	glVertex3dv(&origin[0]);
	glVertex3dv(&xAxis[0]);
	glEnd();

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, diffColorGreen);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffColorGreen);

	glBegin(GL_LINES);
	glVertex3dv(&origin[0]);
	glVertex3dv(&yAxis[0]);
	glEnd();

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, diffColorBlue);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffColorBlue);

	glBegin(GL_LINES);
	glVertex3dv(&origin[0]);
	glVertex3dv(&zAxis[0]);
	glEnd();
	glLineWidth(1);
}

void DemoScene::drawTriangle(const Eigen::Vector3d &a, const Eigen::Vector3d &b, const Eigen::Vector3d &c, const Eigen::Vector3d &norm, const float *color)
{
	float speccolor[4] = { 1.0, 1.0, 1.0, 1.0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, color);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, speccolor);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 100.0);

	glBegin(GL_TRIANGLES);
	glNormal3dv(&norm[0]);
	glVertex3dv(&a[0]);
	glVertex3dv(&b[0]);
	glVertex3dv(&c[0]);
	glEnd();
}

void DemoScene::drawTetrahedron(const Eigen::Vector3d &a, const Eigen::Vector3d &b, const Eigen::Vector3d &c, const Eigen::Vector3d &d, const float *color)
{
	Eigen::Vector3d normal1 = (b - a).cross(c - a);
	Eigen::Vector3d normal2 = (b - a).cross(d - a);
	Eigen::Vector3d normal3 = (c - a).cross(d - a);
	Eigen::Vector3d normal4 = (c - b).cross(d - b);
	drawTriangle(a, b, c, normal1, color);
	drawTriangle(a, b, d, normal2, color);
	drawTriangle(a, c, d, normal3, color);
	drawTriangle(b, c, d, normal4, color);
}
