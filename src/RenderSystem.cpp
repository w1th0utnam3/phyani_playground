#include "RenderSystem.h"

#include <iostream>
#include <array>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

RenderSystem::RenderSystem(EntityComponentSystem& ecs)
	: m_ecs(ecs)
{
}

void RenderSystem::render()
{
	static constexpr std::array<float, 4> white = { 1,1,1,1 };
	static constexpr std::array<float, 4> red = { 1,0,0,1 };
	static constexpr std::array<float, 4> green{ 0.0f, 1.0f, 0.0f, 1.0f };
	static constexpr std::array<float, 4> blue{ 0.0f, 0.0f, 1.0f, 1.0f };

	// Loop over rigid bodies and particles: Integrate time
	for (auto rigidBodyEntity : m_ecs.view<RigidBody>()) {
		auto& rigidBody = m_ecs.get<RigidBody>(rigidBodyEntity);
		const auto& position = rigidBody.linearState.position;
		const auto& rotation = rigidBody.angularState.rotation;

		auto quat = glm::make_quat<double>(rotation.coeffs().data());
		auto rotationMatrix = glm::rotate(glm::dmat4(), glm::angle(quat), glm::axis(quat));

		glPushMatrix();
		glTranslated(position.x(), position.y(), position.z());
		glMultMatrixd(glm::value_ptr(rotationMatrix));
		drawCube(0.5, green.data());
		glPopMatrix();
		
	}

	for (auto particleEntity : m_ecs.view<Particle>()) {
		auto& particle = m_ecs.get<Particle>(particleEntity);
		const auto& position = particle.linearState.position;

		glPushMatrix();
		glTranslated(position.x(), position.y(), position.z());
		drawCube(0.1, blue.data());
		glPopMatrix();
	}

	for (auto jointEntity : m_ecs.view<Joint>()) {
		auto& joint = m_ecs.get<Joint>(jointEntity);
		const auto& positionA = joint.connectors.first.globalPosition;
		const auto& positionB = joint.connectors.second.globalPosition;

		glPushMatrix();
		glTranslated(positionA.x(), positionA.y(), positionA.z());
		drawCube(0.05, red.data());
		glPopMatrix();

		glPushMatrix();
		glTranslated(positionB.x(), positionB.y(), positionB.z());
		drawCube(0.05, red.data());
		glPopMatrix();

		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, red.data());
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, red.data());
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white.data());
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 100.0);
		glLineWidth(2);

		glBegin(GL_LINES);
		glVertex3dv(&positionA[0]);
		glVertex3dv(&positionB[0]);
		glEnd();
	}

	//drawTetrahedron(Eigen::Vector3d::Zero(), Eigen::Vector3d::UnitX(), Eigen::Vector3d::UnitY(), Eigen::Vector3d::UnitZ(), green.data());
	//drawCube(1, green.data());
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

void RenderSystem::drawCube(const double edgeLength, const float *color)
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
