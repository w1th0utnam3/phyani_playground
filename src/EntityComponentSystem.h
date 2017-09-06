#pragma once

#include <cstdint>
#include <atomic>

#include <registry.hpp>

#include <Eigen/Geometry>

#include "Common.h"

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

	common::variant::variant<DampedSpring> jointProperties;
};

struct TranslationalState
{
	Eigen::Vector3d position = Eigen::Vector3d(0, 0, 0);
	Eigen::Vector3d linearVelocity = Eigen::Vector3d(0, 0, 0);
};

struct RotationalState
{
	Eigen::Quaterniond rotation = Eigen::Quaterniond(1, 0, 0, 0);
	Eigen::Vector3d angularVelocity = Eigen::Vector3d(0, 0, 0);
};

struct TranslationalAnimatedBody
{
	bool sleeping = false;

	double mass = 0.0;

	Eigen::Vector3d externalForce = Eigen::Vector3d(0, 0, 0);

	TranslationalState state;
};

struct RotationalAnimatedBody
{
	bool sleeping = false;

	Eigen::Vector3d prinicipalInertia = Eigen::Vector3d(0, 0, 0);

	Eigen::Matrix3d globalInertiaMatrix;
	Eigen::Matrix3d globalInverseInertiaMatrix;

	Eigen::Vector3d externalTorque = Eigen::Vector3d(0, 0, 0);

	RotationalState state;
	Eigen::Matrix3d rotationMatrix = Eigen::Matrix3d::Identity();
};

struct RenderData
{
	Eigen::Vector4f color;

	struct Cuboid
	{
		Eigen::Vector3f position = Eigen::Vector3f(0, 0, 0);
		Eigen::Quaternionf rotation = Eigen::Quaternionf(1, 0, 0, 0);
		Eigen::Vector3f edges = Eigen::Vector3f(1, 1, 1);
	};

	struct Joint
	{
		std::pair<Eigen::Vector3f, Eigen::Vector3f> connectorPositions = { Eigen::Vector3f(0, 0, 0) , Eigen::Vector3f(0, 0, 0) };
		float connectorSize = 0.05f;
		float lineWidth = 2.0f;
	};

	common::variant::variant<Cuboid, Joint> properties;
};

using EntityComponentSystemBase = entt::StandardRegistry<EntityType, TranslationalAnimatedBody, RotationalAnimatedBody, Joint, RenderData>;

class EntityComponentSystem : public EntityComponentSystemBase {};
