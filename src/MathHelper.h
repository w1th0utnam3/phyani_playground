#pragma once

#include <ostream>
#include <sstream>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

template <typename T, glm::qualifier P>
std::ostream& operator<<(std::ostream& os, glm::tquat<T, P> quat)
{
	os << "[w=" << quat.w << ",x=" << quat.x << ",y=" << quat.y << ",z=" << quat.z << "]";
	return os;
}

template <typename T, glm::qualifier P>
std::ostream& operator<<(std::ostream& os, glm::tvec3<T, P> vec)
{
	os << "[x=" << vec.x << ",y=" << vec.y << ",z=" << vec.z << "]";
	return os;
}

template <glm::length_t l, typename T, glm::qualifier P>
std::ostream& operator<<(std::ostream& os, glm::vec<l, T, P> vec)
{
	std::ostringstream oss;

	oss << "[";
	oss << vec[0];
	if (l > 1) for (glm::length_t i = 1; i < l; i++) oss << "," << vec[i];
	oss << "]";

	os << oss.str();
	return os;
}

template <glm::length_t rows, glm::length_t cols, typename T, glm::qualifier P>
std::ostream& operator<<(std::ostream& os, glm::mat<rows, cols, T, P> mat)
{
	std::ostringstream oss;

	oss << "[[";
	oss << mat[0][0];
	if (cols > 1) {
		for (glm::length_t j = 1; j < cols; j++) {
			oss << "," << mat[0][j];
		}
	}
	oss << "]";
	if (rows > 1) {
		for (glm::length_t i = 1; i < rows; i++) {
			oss << ";[";
			oss << mat[i][0];
			if (cols > 1) {
				for (glm::length_t j = 1; j < cols; j++) {
					oss << "," << mat[i][j];
				}
			}
			oss << "]";
		}
	}
	oss << "]";

	os << oss.str();
	return os;
}

template <typename T, glm::qualifier P>
bool isfinite(glm::tquat<T, P> quat)
{
	return (std::isfinite(quat.x)
			&& std::isfinite(quat.y)
			&& std::isfinite(quat.z)
			&& std::isfinite(quat.w));
}

template <typename T, glm::qualifier P>
bool isfinite(glm::tvec3<T, P> vec)
{
	return (std::isfinite(vec.x)
			&& std::isfinite(vec.y)
			&& std::isfinite(vec.z));
}
