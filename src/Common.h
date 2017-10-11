#pragma once

// Include useless header in order to get std lib version macros
#include <ciso646>

#if defined(__clang_major__) && __clang_major__ < 5 && defined(__GLIBCXX__)
	#error Clang/LLVM older than version 5.0.0 is incompatible with libstdc++ <functional> in C++17 mode (missing template deduction guides)
#endif

// Conditionally switch on std::variant support
#if __has_include(<variant>) || (defined(__cpp_lib_variant) && __cpp_lib_variant >= 201606)
	#define HAS_STD_VARIANT
#endif

// Clang 5.0 is incompatible with current libstdc++ std::variant implementation
#if defined(__clang_major__) && defined(__GLIBCXX__)
	#undef HAS_STD_VARIANT
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
