#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <stdexcept>
#include <iostream>
#include <cassert>

#define cg_assert__do_nothing(condition,message) (void(0))
#define cg_assert__std_assert(condition,message) std::assert(condition)
#define cg_assert__throw_exception(condition,message) \
	throw std::runtime_error((std::stringstream()<<message).str());
#define cg_assert__pause_debugger(condition,message) \
   { if (not (condition)) { std::cerr << "ERROR: " << (message) << std::endl; asm("int3"); asm("nop"); } }

#ifdef NDEBUG
#	define cg_assert cg_assert__throw_exception
#	define cg_info(message) (void(0))
#else
#	define cg_assert cg_assert__pause_debugger
#	define cg_info(message) std::cerr << (message) << std::endl;
#endif

#define cg_error(message) cg_assert(false,message)

#endif
