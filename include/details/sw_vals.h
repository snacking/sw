// header file for sw::vals (internal)

#ifndef _SW_VALS_H_
#define _SW_VALS_H_

#include "./sw_config.h"

#define _SW_BEGIN namespace sw {
#define _SW_END }

#define _SW_EXPERIMENTAL_BEGIN namespace experimental {
#define _SW_EXPERIMENTAL_END }

#define _SW_VER 1

#if defined(_MSC_VER)
	#undef max
	#undef min
#endif // #if defined(_MSC_VER)

#if defined(__GNUC__) || defined(__clang__)
	#define __FUNC_NAME__ __PRETTY_FUNCTION__ 
#elif defined(_MSC_VER)
	#define __FUNC_NAME__ __func__
#else
	#define __FUNC_NAME__ __FUNCTION__
#endif // #if defined(__GNUC__) || defined(__clang__)

#endif // _SW_VALS_H_

