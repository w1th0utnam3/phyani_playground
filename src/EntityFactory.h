#pragma once

#include <Eigen/Geometry>

#include "EntityComponentSystem.h"

class EntityFactory
{
public:
	static EntityType createCube(EntityComponentSystem& ecs, double mass, double edgeLength);
	static EntityType createCube(EntityComponentSystem& ecs, double mass, double edgeLength, Eigen::Vector3d center);

	static EntityType createParticle(EntityComponentSystem& ecs, double mass);
	static EntityType createParticle(EntityComponentSystem& ecs, double mass, Eigen::Vector3d location);

	static EntityType createSpring(EntityComponentSystem& ecs, EntityType fromEntity, EntityType toEntity, Joint::DampedSpring parameters);
	static EntityType createSpring(EntityComponentSystem& ecs, EntityType fromEntity, Eigen::Vector3d formLocalCoords, EntityType toEntity, Eigen::Vector3d toLocalCoords, Joint::DampedSpring parameters);
};
