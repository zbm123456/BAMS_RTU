#ifndef _BAMS_MAIN_H_
#define _BAMS_MAIN_H_

#define PORTNUM_MAX    2
typedef struct
{
	unsigned char portnum;
    unsigned short baud[PORTNUM_MAX];
	unsigned char devid[PORTNUM_MAX];


} PARA_BAMS; //

int bams_main(void* para);
#endif