#include "EntityFactory.h"

EntityType EntityFactory::createParticle(EntityComponentSystem& ecs, double mass, Eigen::Vector3d location)
{
	auto particleEntity = ecs.create<TranslationalAnimatedBody, RenderData>();

	{
		auto& particle = ecs.get<TranslationalAnimatedBody>(particleEntity);
		particle.mass = mass;
		particle.state.position = location;
	}

	{
		auto& renderData = ecs.get<RenderData>(particleEntity);
		renderData.color = Eigen::Vector4f(0.0f, 0.0f, 1.0f, 1.0f);
		renderData.properties = RenderData::Cuboid{ location.cast<float>(), Eigen::Quaternionf(1,0,0,0), Eigen::Vector3f(0.1f, 0.1f, 0.1f) };
	}

	return particleEntity;
}

EntityType EntityFactory::createCube(EntityComponentSystem& ecs, double mass, double edgeLength, Eigen::Vector3d center)
{
	return createCuboid(ecs, mass, Eigen::Vector3d(edgeLength, edgeLength, edgeLength), center);
}

EntityType EntityFactory::createCuboid(EntityComponentSystem& ecs, double mass, Eigen::Vector3d edges, Eigen::Vector3d center)
{
	auto cubeEntity = ecs.create<TranslationalAnimatedBody, RotationalAnimatedBody, RenderData>();

	{
		auto& cube = ecs.get<TranslationalAnimatedBody>(cubeEntity);
		cube.mass = mass;
		cube.state.position = center;
	}

	{
		auto& cube = ecs.get<RotationalAnimatedBody>(cubeEntity);
		cube.prinicipalInertia = (mass/12)*Eigen::Vector3d(edges[1]*edges[1] + edges[2]*edges[2],
																edges[0]*edges[0] + edges[2]*edges[2],
																edges[0]*edges[0] + edges[1]*edges[1]);
	}

	{
		auto& renderData = ecs.get<RenderData>(cubeEntity);
		renderData.color = Eigen::Vector4f(0.0f, 1.0f, 0.0f, 1.0f);
		renderData.properties = RenderData::Cuboid{ center.cast<float>(), Eigen::Quaternionf(1,0,0,0), edges.cast<float>() };
	}

	return cubeEntity;
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

		RenderData::Joint joint;
		joint.connectorPositions = { Eigen::Vector3f(0, 0, 0) , Eigen::Vector3f(0, 0, 0) };
		joint.connectorSize = 0.05f;
		joint.lineWidth = 2.0f;

		renderData.properties = joint;

	}

	return springEntity;
}
