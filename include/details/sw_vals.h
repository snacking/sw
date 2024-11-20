// header file for sw::vals (internal)

#ifndef _SW_VALS_H_
#define _SW_VALS_H_

#include "./sw_config.h"

#ifdef DEBUG
    #define _SW_DEBUG_
#endif // DEBUG

#define _SW_BEGIN namespace sw {
#define _SW_END }

#define _SW_EXPERIMENTAL_BEGIN namespace experimental {
#define _SW_EXPERIMENTAL_END }

#define _SW_VER 1



#ifdef _SW_DEBUG_
    #include <cassert>
	#include <cstring>

	#define ERROR_MSG_SIZE 128

	struct {
		int code;
		char msg[ERROR_MSG_SIZE];
	} sw_error;

	#if defined(__GNUC__) || defined(__clang__)
		#define __FUNC_NAME__ __PRETTY_FUNCTION__ 
	#elif defined(_MSC_VER)
		#define __FUNC_NAME__ __func__
	#else
		#define __FUNC_NAME__ __FUNCTION__
	#endif // #if defined(__GNUC__) || defined(__clang__)

	#define REGISTER_SW_ERROR(code, msg) \
		do { \
			sw_error.code = code; \
			if (sizeof(msg) < ERROR_MSG_SIZE) \
				strcpy(sw_error.msg, msg); \
			else \
				const char error_msg_1002[] = "Error message size exceeded" \
				assert(ERROR_MSG_SIZE > sizeof(error_msg_1002) && error_msg_1002); \
				REGISTER_SW_ERROR(1002, error_msg_1002); \
		} while(0)

	#define SW_ERROR_CODE sw_error.code
	
	#define SW_ERROR_MSG return sw_error.msg;
#endif // _SW_DEBUG_

#endif // _SW_VALS_H_

