
#include <stdio.h>
#include "threads.h"




int bams_main(void* para)
{
	printf("初始化\n");
    CreateThreads(para);
}