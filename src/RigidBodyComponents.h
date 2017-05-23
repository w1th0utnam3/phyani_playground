#pragma once

#include <cstdint>

#include <Eigen/Geometry>

using EntityType = std::uint32_t;

struct DynamicBody {};

struct RigidBodyProperties
{
	double mass;
	Eigen::Vector3d inertia;

	Eigen::Matrix3d globalInertiaMatrix;
	Eigen::Matrix3d inverseGlobalInertiaMatrix;
};

struct Position
{
	Eigen::Vector3d translation;
	Eigen::Quaterniond rotation;
};

struct RotationMatrices
{
	Eigen::Matrix3d rotationMatrix;
	Eigen::Matrix3d inverseRotationMatrix;
};

struct Velocity
{
	Eigen::Vector3d linear;
	Eigen::Vector3d angular;
};

struct Kinetics
{
	Eigen::Vector3d force;
	Eigen::Vector3d torque;
};

struct Joints
{
	std::vector<EntityType> joints;
};

struct JointAttachement
{
	EntityType entity;
	Eigen::Vector3d localPosition;
};

struct JointProperties
{
	std::pair<JointAttachement, JointAttachement> connection;

	double restLength;
	double elasticity;
	double damping;
};
