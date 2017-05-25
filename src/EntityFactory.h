#pragma once

#include <Eigen/Geometry>

#include "EntityComponentSystem.h"

class EntityFactory
{
public:
	static EntityType createParticle(EntityComponentSystem& ecs, double mass, Eigen::Vector3d location = Eigen::Vector3d(0, 0, 0));
	static EntityType createCube(EntityComponentSystem& ecs, double mass, double edgeLength, Eigen::Vector3d center = Eigen::Vector3d(0, 0, 0));

	static EntityType createSpring(EntityComponentSystem& ecs, EntityType fromEntity, EntityType toEntity, Joint::DampedSpring parameters);
	static EntityType createSpring(EntityComponentSystem& ecs, EntityType fromEntity, Eigen::Vector3d formLocalCoords, EntityType toEntity, Eigen::Vector3d toLocalCoords, Joint::DampedSpring parameters);
};
