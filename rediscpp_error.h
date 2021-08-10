#ifndef __REDISCPP_ERROR_H__
#define __REDISCPP_ERROR_H__

#include <stdio.h>

#define rediscpp_error(fmt, args...) fprintf(stderr, "[%s:%d:%s] " fmt "\n", __FILE__, __LINE__, __FUNCTION__, ##args)

#endif
