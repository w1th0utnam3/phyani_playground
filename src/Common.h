#pragma once

// Check compiler versions to enable certain features
#if defined(__cpp_lib_variant) && __cpp_lib_variant >= 201606
	#define HAS_STD_VARIANT
#elif defined(__GNUC__) && __GNUC__ >= 7
	// Current variant implementation has bugs
	//#define HAS_STD_VARIANT
#elif defined(_MSC_VER) && _MSC_VER >= 1910
	#define HAS_STD_VARIANT
#elif defined(__clang_major__) && __clang_major__ >= 4
	#define HAS_STD_VARIANT
#endif

// Switch variant implementation depending on std library support
#if defined(HAS_STD_VARIANT)
	#include <variant>
	namespace common {
		namespace variant = std;
	}
#else
	#include <mpark/variant.hpp>
	namespace common {
		namespace variant = mpark;
	}
#endif
