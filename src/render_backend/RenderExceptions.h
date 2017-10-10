#include <stdexcept>
#include <string>

//! Exception which is thrown when critical GLFW errors occured
class GlfwError : public std::runtime_error {
public:
	explicit GlfwError(const std::string& what_arg)
		: std::runtime_error(what_arg)
	{}

	explicit GlfwError(const char* what_arg)
		: std::runtime_error(what_arg)
	{}
};
