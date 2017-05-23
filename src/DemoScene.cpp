﻿#include "DemoScene.h"

#include <iostream>
#include <array>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <Eigen/Geometry>

#include "EntityFactory.h"

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

	m_renderSystem.render();
}

void DemoScene::initializeEntities()
{
	// Create a rigid body
	auto cubeEntity = EntityFactory::createCube(m_ecs, 1, 0.5, Eigen::Vector3d(0.25, 0.0, 0.25));
	// Create a fixed particle
	auto particleEntity = EntityFactory::createParticle(m_ecs, 0.0, Eigen::Vector3d(0.25, 1.5, 0.25));
	// Create a spring as a joint
	EntityFactory::createSpring(m_ecs,
								cubeEntity, Eigen::Vector3d(0.25, 0.25, 0.25),
								particleEntity, Eigen::Vector3d(0.0, 0.0, 0.0),
								Joint::DampedSpring{ 0.6, 11, 0.8 });

	m_animationSystem.initialize();
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
