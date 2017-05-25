#pragma once

#include <ostream>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

template <typename T, glm::precision P>
std::ostream& operator<<(std::ostream& os, glm::tquat<T, P> quat)
{
	os << "w=" << quat.w << ",x=" << quat.x << ",y=" << quat.y << ",z=" << quat.z;
	return os;
}

template <typename T, glm::precision P>
std::ostream& operator<<(std::ostream& os, glm::tvec3<T, P> vec)
{
	os << "x=" << vec.x << ",y=" << vec.y << ",z=" << vec.z;
	return os;
}

template <typename T, glm::precision P>
bool isfinite(glm::tquat<T, P> quat)
{
	return (std::isfinite(quat.x)
	        && std::isfinite(quat.y)
	        && std::isfinite(quat.z)
	        && std::isfinite(quat.w));
}

template <typename T, glm::precision P>
bool isfinite(glm::tvec3<T, P> vec)
{
	return (std::isfinite(vec.x)
			&& std::isfinite(vec.y)
			&& std::isfinite(vec.z));
}
