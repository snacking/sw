// header file for sw::vals (internal)

#ifndef _SW_VALS_H_
#define _SW_VALS_H_

#define _SW_BEGIN namespace sw {
#define _SW_END }

#define _SW_EXPERIMENTAL_BEGIN namespace experimental {
#define _SW_EXPERIMENTAL_END }

#define _SW_VER 1

#define _SW_HAS_EXCEPTIONS 1

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

#if _SW_HAS_EXCEPTIONS
	#define _HAS_EXCEPTIONS 1
	#define _SW_TRY_BEGIN try {
	#define _SW_CATCH(x) \
		}             \
		catch (x) {
	#define _SW_CATCH_ALL \
		}              \
		catch (...) {
	#define _SW_CATCH_END }

	#define _SW_RERAISE throw
	#define _SW_THROW(...) throw(__VA_ARGS__)
#if __cplusplus < 201703L
	#define _SW_DES(...) throw(__VA_ARGS__)
#else
	#define _SW_DES(...) // Dynamic exception specification after C++17 is deprecated. Will automatic replaced by noexcept(false).
#endif
	#define _SW_NOEXCEPT noexcept
	#define _SW_NOEXCEPT_OP(x) noexcept(x)
#else
	#define _SW_TRY_BEGIN \
		{              \
			if (1) {
	#define _SW_CATCH(x) \
		}             \
		else if (0) {
	#define _SW_CATCH_ALL \
		}              \
		else if (0) {
	#define _SW_CATCH_END \
		}              \
		}

	#define _SW_RERAISE
	#define _SW_THROW(...)
	#define _SW_NOEXCEPT
	#define _SW_NOEXCETP_OP(x)
#endif // _HAS_EXCEPTIONS

#ifdef _SW_DEBUG_
	#define ERROR_MSG_SIZE 128

	struct {
		int code;
		char msg[ERROR_MSG_SIZE];
	} sw_error;

	inline const char *error_msg_1001 = "error message size exceeded";

	#define REGISTER_SW_ERROR(CODE, MSG) \
	do { \
		sw_error.code = CODE; \
		if (strlen(MSG) < ERROR_MSG_SIZE) \
			strcpy(sw_error.msg, MSG); \
		else \
			assert((ERROR_MSG_SIZE > strlen(error_msg_1001)) && error_msg_1001); \
			sw_error.code = 1001; \
			strcpy(sw_error.msg, error_msg_1001); \
	} while(0)

	#define SW_ERROR_CODE sw_error.code
	#define SW_ERROR_MSG sw_error.msg;

	#define SW_ASSERT(expr) \
		if (!(expr)) { \
			
		}
#else
	#define REGISTER_SW_ERROR(code, msg)
	#define SW_ERROR_CODE -1
	#define SW_ERROR_MSG "DEBUG MODE IS NOT ENABLED"
	#define SW_ASSERT(expr)
#endif // _SW_DEBUG_

#endif // _SW_VALS_H_
