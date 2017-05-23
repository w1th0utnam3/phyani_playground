#pragma once

#include <cstdint>
#include <atomic>
#include <variant>

#include <registry.hpp>

#include <Eigen/Geometry>

using EntityType = std::uint32_t;

struct Connector
{
	EntityType parentEntity;
	Eigen::Vector3d localPosition = Eigen::Vector3d(0, 0, 0);

	Eigen::Vector3d globalPosition = Eigen::Vector3d(0, 0, 0);
	Eigen::Vector3d globalVelocity = Eigen::Vector3d(0, 0, 0);
};

struct Joint
{
	std::pair<Connector, Connector> connectors;

	struct DampedSpring
	{
		double restLength;
		double elasticity;
		double damping;
	};

	std::variant<DampedSpring> jointProperties;
};

struct LinearState
{
	Eigen::Vector3d position = Eigen::Vector3d(0, 0, 0);
	Eigen::Vector3d linearVelocity = Eigen::Vector3d(0, 0, 0);
};

struct AngularState
{
	Eigen::Quaterniond rotation = Eigen::Quaterniond(1, 0, 0, 0);
	Eigen::Vector3d angularVelocity = Eigen::Vector3d(0, 0, 0);
};

struct RigidBody
{
	bool sleeping = false;

	double mass = 0.0;
	Eigen::Vector3d prinicipalInertia = Eigen::Vector3d(0, 0, 0);

	Eigen::Matrix3d globalInertiaMatrix;
	Eigen::Matrix3d globalInverseInertiaMatrix;

	Eigen::Vector3d externalForce = Eigen::Vector3d(0, 0, 0);
	Eigen::Vector3d externalTorque = Eigen::Vector3d(0, 0, 0);

	LinearState linearState;
	AngularState angularState;
	Eigen::Matrix3d rotationMatrix = Eigen::Matrix3d::Identity();
};

struct Particle
{
	bool sleeping = false;

	double mass = 0.0;

	Eigen::Vector3d externalForce = Eigen::Vector3d(0, 0, 0);

	LinearState linearState;
};

struct RenderData
{
	Eigen::Vector4f color;

	struct Cube
	{
		double edgeLength;
	};

	struct Joint
	{
		double connectorSize;
		double lineWidth;
	};

	std::variant<Cube, Joint> properties;
};

using EntityComponentSystem = entt::StandardRegistry<EntityType, RigidBody, Particle, Joint, RenderData>;
