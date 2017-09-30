#include "RenderSystem.h"

#include "Common.h"
#include "CommonOpenGL.h"

RenderSystem::RenderSystem(EntityComponentSystem& ecs)
	: m_ecs(ecs)
{
}

void RenderSystem::render()
{
	for (auto renderEntity : m_ecs.view<RenderData>()) {
		m_renderer.renderData = &m_ecs.get<RenderData>(renderEntity);
		common::variant::visit(m_renderer, m_renderer.renderData->properties);
	}
}

void RenderSystem::Renderer::operator()(const RenderData::Cuboid& cuboidData) const
{
	const auto quat = glm::make_quat<float>(cuboidData.rotation.coeffs().data());
	const auto rotationMatrix = glm::rotate(glm::fmat4(), glm::angle(quat), glm::axis(quat));

	glPushMatrix();
	glTranslatef(cuboidData.position.x(), cuboidData.position.y(), cuboidData.position.z());
	glMultMatrixf(glm::value_ptr(rotationMatrix));
	drawCoordinateSystem(0.75);
	glScalef(cuboidData.edges.x(), cuboidData.edges.y(), cuboidData.edges.z());
	drawCube(1, &renderData->color[0]);
	glPopMatrix();
}

void RenderSystem::Renderer::operator()(const RenderData::Joint& jointData) const
{
	const auto& positionA = jointData.connectorPositions.first;
	const auto& positionB = jointData.connectorPositions.second;

	glPushMatrix();
	glTranslatef(positionA.x(), positionA.y(), positionA.z());
	drawCube(jointData.connectorSize, &renderData->color[0]);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(positionB.x(), positionB.y(), positionB.z());
	drawCube(jointData.connectorSize, &renderData->color[0]);
	glPopMatrix();

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, &renderData->color[0]);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, &renderData->color[0]);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, &renderData->color[0]);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 100.0);
	glLineWidth(jointData.lineWidth);

	glBegin(GL_LINES);
	glVertex3fv(&positionA[0]);
	glVertex3fv(&positionB[0]);
	glEnd();
}

void RenderSystem::drawTriangle(const Eigen::Vector3d &a, const Eigen::Vector3d &b, const Eigen::Vector3d &c, const Eigen::Vector3d &norm, const float *color)
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

void RenderSystem::drawTetrahedron(const Eigen::Vector3d &a, const Eigen::Vector3d &b, const Eigen::Vector3d &c, const Eigen::Vector3d &d, const float *color)
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

void RenderSystem::drawCube(float edgeLength, const float *color)
{
	const double x = 0.5*edgeLength;

	float speccolor[4] = { 1.0, 1.0, 1.0, 1.0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, color);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, speccolor);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 100.0);

	glBegin(GL_POLYGON);
	glVertex3d(x, -x, -x);
	glVertex3d(x, x, -x);
	glVertex3d(-x, x, -x);
	glVertex3d(-x, -x, -x);
	glEnd();

	// BACK
	glBegin(GL_POLYGON);
	glVertex3d(x, -x, x);
	glVertex3d(x, x, x);
	glVertex3d(-x, x, x);
	glVertex3d(-x, -x, x);
	glEnd();

	// RIGHT
	glBegin(GL_POLYGON);
	glVertex3d(x, -x, -x);
	glVertex3d(x, x, -x);
	glVertex3d(x, x, x);
	glVertex3d(x, -x, x);
	glEnd();

	// LEFT
	glBegin(GL_POLYGON);
	glVertex3d(-x, -x, x);
	glVertex3d(-x, x, x);
	glVertex3d(-x, x, -x);
	glVertex3d(-x, -x, -x);
	glEnd();

	// TOP
	glBegin(GL_POLYGON);
	glVertex3d(x, x, x);
	glVertex3d(x, x, -x);
	glVertex3d(-x, x, -x);
	glVertex3d(-x, x, x);
	glEnd();

	// BOTTOM
	glBegin(GL_POLYGON);
	glVertex3d(x, -x, -x);
	glVertex3d(x, -x, x);
	glVertex3d(-x, -x, x);
	glVertex3d(-x, -x, -x);
	glEnd();
}

void RenderSystem::drawCoordinateSystem(float axisLength)
{
	static const Eigen::Vector3d origin(0, 0, 0);
	const Eigen::Vector3d xAxis(axisLength, 0, 0);
	const Eigen::Vector3d yAxis(0, axisLength, 0);
	const Eigen::Vector3d zAxis(0, 0, axisLength);

	static constexpr float SpecColorWhite[4] = { 1,1,1,1 };
	static constexpr float diffColorRed[4] = { 1,0,0,1 };
	static constexpr float diffColorGreen[4] = { 0,1,0,1 };
	static constexpr float diffColorBlue[4] = { 0,0,1,1 };

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
