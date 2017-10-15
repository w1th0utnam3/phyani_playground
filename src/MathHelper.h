#pragma once

#include <random>
#include <iostream>
#include <ostream>
#include <sstream>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

//! Uniform real pseudo random number generator with a simple interface
template <typename RealType>
class uniform_real_rng {
	std::mt19937 gen;
	std::uniform_real_distribution<RealType> dis;

public:
	//! Constructs and seeds the rng for the range [a,b).
	uniform_real_rng(RealType a = 0.0, RealType b = 1.0)
		: gen(std::random_device()())
		, dis(a, b) {}

	//! Returns the next random number of the rng.
	RealType operator()() {
		return dis(gen);
	}
};

//! Insertion operator for glm quaternions
template <typename T, glm::qualifier P>
std::ostream& operator<<(std::ostream& os, glm::tquat<T, P> quat)
{
	os << "[w=" << quat.w << ",x=" << quat.x << ",y=" << quat.y << ",z=" << quat.z << "]";
	return os;
}

//! Insertion operator for glm 3d vectors
template <typename T, glm::qualifier P>
std::ostream& operator<<(std::ostream& os, glm::tvec3<T, P> vec)
{
	os << "[x=" << vec.x << ",y=" << vec.y << ",z=" << vec.z << "]";
	return os;
}

//! Insertion operator for glm vectors
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

//! Insertion operator for glm matrices
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

//! Returns whether the supplied quaternion has only finite components
template <typename T, glm::qualifier P>
bool isfinite(const glm::tquat<T, P>& quat)
{
	return (std::isfinite(quat.x)
			&& std::isfinite(quat.y)
			&& std::isfinite(quat.z)
			&& std::isfinite(quat.w));
}

//! Returns whether the supplied 3d vector has only finite components
template <typename T, glm::qualifier P>
bool isfinite(const glm::tvec3<T, P>& vec)
{
	return (std::isfinite(vec.x)
			&& std::isfinite(vec.y)
			&& std::isfinite(vec.z));
}
