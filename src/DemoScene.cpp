#include "DemoScene.h"

#include <array>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <Eigen/Geometry>

#include "Simulation.h"
#include "EntityFactory.h"

DemoScene::DemoScene()
	: m_ecs(Simulation::getEntityComponentSystem())
	, m_animationSystem(Simulation::getAnimationSystem())
	, m_renderSystem(Simulation::getRenderSystem())
	, m_animationLoop(m_animationSystem)
{
}

DemoScene::~DemoScene()
{
	// Potentially join the animatino thread
	if (m_animationLoop.isEventLoopRunning()) m_animationLoop.stopEventLoop().wait();
	m_animationThread.join();
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

	initializeEntities();

	// Start the animation event loop
	auto animationLoopPointer = &m_animationLoop;
	m_animationThread = std::move(std::thread([animationLoopPointer]()
	{
		animationLoopPointer->executeTimestepLoop();
	}));
}

void DemoScene::toggleAnimation(double timeStretch)
{
	m_animationLoop.toggleAutomaticTimestepping(timeStretch);
}

void DemoScene::doTimestep(double dt)
{
	// m_animationSystem.computeTimestep(dt);
	m_animationLoop.requestTimestep(dt);
}

void DemoScene::resetScene()
{
	if (m_animationLoop.isEventLoopRunning()) m_animationLoop.stopEventLoop().wait();
	m_animationThread.join();

	m_ecs.reset();
	initializeSceneContent();
}

void DemoScene::renderSceneContent()
{
	glMatrixMode(GL_PROJECTION);
	const auto projection = m_camera->projectionMatrix();
	glLoadMatrixd(glm::value_ptr(projection));
	glMatrixMode(GL_MODELVIEW);
	const auto transform = m_camera->modelViewMatrix();
	glLoadMatrixd(glm::value_ptr(transform));

	initializeLight();
	drawCoordinateSystem(2);

	m_renderSystem.render();
}

void DemoScene::initializeEntities()
{
	// Create a rigid body
	auto cubeEntity1 = EntityFactory::createCube(m_ecs, 1, 0.5, Eigen::Vector3d(2, 0.2, 1));
	// Create a fixed particle
	auto particleEntity = EntityFactory::createParticle(m_ecs, 0.0, Eigen::Vector3d(0.25, 1.5, 0.25));
	// Create a spring as a joint
	EntityFactory::createSpring(m_ecs,
								cubeEntity1, Eigen::Vector3d(0.25, 0.25, 0.25),
								particleEntity, Eigen::Vector3d(0.0, 0.0, 0.0),
								Joint::DampedSpring{ 0.6, 32, 16 });

	auto cubeEntity2 = EntityFactory::createCube(m_ecs, 0.5, 0.25, Eigen::Vector3d(2.2, -0.8, 1.1));

	EntityFactory::createSpring(m_ecs,
								cubeEntity1, Eigen::Vector3d(-0.25, -0.25, -0.25),
								cubeEntity2, Eigen::Vector3d(-0.25, 0.25, -0.25),
								Joint::DampedSpring{ 0.2, 16, 8 });

	auto cubeEntity3 = EntityFactory::createCube(m_ecs, 0.5, 0.25, Eigen::Vector3d(1.8, -1.8, 0.6));

	EntityFactory::createSpring(m_ecs,
								cubeEntity2, Eigen::Vector3d(0.25, -0.25, 0.25),
								cubeEntity3, Eigen::Vector3d(-0.25, 0.25, -0.25),
								Joint::DampedSpring{ 0.2, 16, 8 });

	m_animationSystem.initialize();
}

void DemoScene::initializeLight()
{
	float t = 0.8f;
	float a = 0.1f;

	float amb0[4] = {a,a,a,1};
	float diff0[4] = {t,t,t,1};
	float spec0[4] = {1,1,1,1};
	float pos0[4] = {-1,1,1,1};
	glLightfv(GL_LIGHT0, GL_AMBIENT, amb0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diff0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, spec0);
	glLightfv(GL_LIGHT0, GL_POSITION, pos0);
	//glEnable(GL_LIGHT0);

	float amb1[4] = {a,a,a,1};
	float diff1[4] = {t,t,t,1};
	float spec1[4] = {1,1,1,1};
	float pos1[4] = {10,10,10,1};
	glLightfv(GL_LIGHT1, GL_AMBIENT, amb1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diff1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, spec1);
	glLightfv(GL_LIGHT1, GL_POSITION, pos1);
	//glEnable(GL_LIGHT1);

	float amb2[4] = {a,a,a,1};
	float diff2[4] = {t,t,t,1};
	float spec2[4] = {1,1,1,1};
	float pos2[4] = {0,0,2,1};
	glLightfv(GL_LIGHT2, GL_AMBIENT, amb2);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, diff2);
	glLightfv(GL_LIGHT2, GL_SPECULAR, spec2);
	glLightfv(GL_LIGHT2, GL_POSITION, pos2);
	glEnable(GL_LIGHT2);

	glEnable(GL_LIGHTING);
	//glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	//glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	//glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
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
