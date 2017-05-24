#include "EntityFactory.h"

EntityType EntityFactory::createCube(EntityComponentSystem& ecs, double mass, double edgeLength)
{
	return createCube(ecs, mass, edgeLength, Eigen::Vector3d(0, 0, 0));
}

EntityType EntityFactory::createCube(EntityComponentSystem& ecs, double mass, double edgeLength, Eigen::Vector3d center)
{
	auto cubeEntity = ecs.create<RigidBody, RenderData>();
	
	{
		auto& cube = ecs.get<RigidBody>(cubeEntity);
		cube.mass = mass;
		double inertia = (cube.mass * edgeLength * edgeLength) / 6;
		cube.prinicipalInertia = Eigen::Vector3d(inertia, inertia, inertia);
		cube.linearState.position = center;
	}
	
	{
		auto& renderData = ecs.get<RenderData>(cubeEntity);
		renderData.color = Eigen::Vector4f(0.0f, 1.0f, 0.0f, 1.0f);
		renderData.properties = RenderData::Cube{edgeLength};
	}

	return cubeEntity;
}

EntityType EntityFactory::createParticle(EntityComponentSystem& ecs, double mass)
{
	return createParticle(ecs, mass, Eigen::Vector3d(0, 0, 0));
}

EntityType EntityFactory::createParticle(EntityComponentSystem& ecs, double mass, Eigen::Vector3d location)
{
	auto particleEntity = ecs.create<Particle, RenderData>(); 

	{
		auto& particle = ecs.get<Particle>(particleEntity);
		particle.mass = mass;
		particle.linearState.position = location;
	} 

	{
		auto& renderData = ecs.get<RenderData>(particleEntity);
		renderData.color = Eigen::Vector4f(0.0f, 0.0f, 1.0f, 1.0f);
		renderData.properties = RenderData::Cube{0.1};
	}

	return particleEntity;
}

EntityType EntityFactory::createSpring(EntityComponentSystem& ecs, EntityType fromEntity, EntityType toEntity, Joint::DampedSpring parameters)
{
	return createSpring(ecs, fromEntity, Eigen::Vector3d(0, 0, 0), toEntity, Eigen::Vector3d(0, 0, 0), parameters);
}

EntityType EntityFactory::createSpring(EntityComponentSystem& ecs, EntityType fromEntity, Eigen::Vector3d formLocalCoords, EntityType toEntity, Eigen::Vector3d toLocalCoords, Joint::DampedSpring parameters)
{
	auto springEntity = ecs.create<Joint, RenderData>(); 
	
	{
		auto& spring = ecs.get<Joint>(springEntity);
		spring.connectors.first.parentEntity = fromEntity;
		spring.connectors.first.localPosition = formLocalCoords;
		spring.connectors.second.parentEntity = toEntity;
		spring.connectors.second.localPosition = toLocalCoords;
		spring.jointProperties = parameters;
	} 
	
	{
		auto& renderData = ecs.get<RenderData>(springEntity);
		renderData.color = Eigen::Vector4f(1.0f, 0.0f, 0.0f, 1.0f);
		renderData.properties = RenderData::Joint{0.05, 1};
	}

	return springEntity;
}
