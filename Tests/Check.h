#ifndef CHECK_H
#define CHECK_H

#include <stdio.h>
#include <stdlib.h>

/** Check if condition holds, used for unit-testing */
#define CHECK(condition)		do{													\
									if(!(condition)){								\
										fprintf(stderr,								\
												"%s, line %d: Check failed!\n",		\
												__FILE__, __LINE__);				\
										exit(1);									\
									}												\
								}while(0)

#endif /* CHECK_H */
