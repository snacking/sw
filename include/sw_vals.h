// header file for sw::vals (internal)

#ifndef _SW_VALS_H_
#define _SW_VALS_H_

// #define _SW_DEBUG_

#ifdef DEBUG
    #define _SW_DEBUG_
#endif // DEBUG

#define _SW_BEGIN namespace sw {
#define _SW_END }

#define _SW_DETAIL_BEGIN namespace sw {
#define _SW_DETAIL_END }

#define _SW_EXPERIMENTAL_BEGIN namespace experimental {
#define _SW_EXPERIMENTAL_END }

#ifdef _SW_DEBUG_
    #include <iostream>
    #include <cassert>
#endif // _SW_DEBUG_

#endif // _SW_VALS_H_

