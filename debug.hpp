#ifndef _DEBUG_HPP_
#define _DEBUG_HPP_

//#define _DEBUG_ON

#ifdef _DEBUG_ON
#define DPRINTF(x, ...) printf("[DEBUG] " x, __VA_ARGS__)
#else
#define DPRINTF(x, ...) ;
#endif

#endif
