
/*******************************************************************************
* File Name          :IEC61850_type.h
* Author             :lmk
* Date First Issued  :2016/10/12
* Description        :
*                    :

******************************************************************************/
#ifndef __IEC61850_TYPE_H__
#define __IEC61850_TYPE_H__
#include <stdint.h>

#define data_num 1024
#define cmd_num 256

#define ST_UINT unsigned int
#define ST_CHAR char

//////////////////////////////////////////////////////////////////
//数据标识1 portID

enum _PORTID
{
	INFO_EMU = 1, // emu整机信息
	INFO_LCD = 2, // emu下lcd信息
	INFO_PCS = 3, // emu下lcd下pcs信息
	INFO_BMS = 5, // BMS 数据
};

//////////////////////////////////////////////////////////////////
//数据标识2 devID
//////////////////////////////////////////////////////////////////

typedef struct
{
	int portID;
	int devID;
	int typeID;
	int pointID;
} POINT_ADDR;

typedef struct
{
	POINT_ADDR sAddr; //点地址表

	ST_UINT data_size; //数据长度
	ST_CHAR *data;	   //数据指针
	ST_CHAR el_tag;	   //数据类型，3-bool, 5-int, 6-unsigned int,7-float
} ENTRY_MAP;

//实时数据
typedef struct tag_data_info
{
	char buf[16]; //数据值
	ENTRY_MAP map;
} data_info_t;

#endif /* __IEC61850_TYPE_H__ */
