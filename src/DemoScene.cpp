#include "DemoScene.h"

#include <glad/glad.h>
#include <Eigen/Geometry>

void DemoScene::initializeSceneContent()
{
	initializeLight();
}

void DemoScene::renderSceneContent()
{
	glMatrixMode(GL_MODELVIEW);
	auto tranform = m_camera->toModelViewMatrix();
	glLoadMatrixd(tranform.data());

	drawCoordinateSystem(2);
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
