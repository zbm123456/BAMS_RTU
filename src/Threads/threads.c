
#include "threads.h"
#include "sys.h"
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include "serial.h"
#include "bams_main.h"
#include "protocol_bams.h"
#include "crc.h"
#include "IEC61850_type.h"

PARA_BAMS ParaBams;
PARA_BAMS *pParaBams = &ParaBams;

void Uart_Init(unsigned char portid, unsigned int baud)
{
	int ret;

	printf("正在试图打开串口%d   波特率=%d！！！！！\n", portid, baud);
	ret = OpenComPort(portid, baud, 8, "1", 'N');
	if (ret == -1)
		printf("串口%d打开失败！！！！！\n", portid);
	else
	{
		printf("串口%d打开成功  波特率=%d！！！！！\n", portid, baud);
	}
}
//所有的BMS信息数据数据标识1都用5来表示，数据标识2编号从1-36，每个BMS占用18个编号，BMS1为1-18，BMS2为19-36，
//如果该编号和LCD的模块信息数据标识2相同，那么它们为一组
int AnalysFun10(int bamsid, unsigned short RegAddr, unsigned short *pdata)
{
	POINT_ADDR poi;
	poi.portID = 5;
	int i;

	switch (RegAddr)
	{
	case 0x0001:
	{
		static unsigned short val[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
		for (i = 0; i < 16; i++)
		{
			if (pdata[i] != val)
			{
				if (bamsid == 1)
					poi.devID = i;
				else
					poi.devID = 18 + i;
			}
		}
	}

	break;
	case 0x0002:
	{
		static unsigned short val[16] =  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
		for (i = 0; i < 16; i++)
		{
			if (pdata[i] != val)
			{
				if (bamsid == 1)
					poi.devID = i;
				else
					poi.devID = 18 + i;
			}
		}
	}
	break;
	default:
		break;
	}
	return 0;
}
static int doRecvFunTasks(int portid)
{
	unsigned char commbuf[256];
	int lencomm=9, lentemp1, lentemp2, lenframe;
	unsigned short crcval;
	unsigned char b1, b2;
	int bmsid = portid + 1;
	unsigned short regAddr;
	lentemp1 = ReadComPort(portid, commbuf, lencomm);

	if (lentemp1 < 2)
		return 255;

	if (commbuf[1] != 0x10 && commbuf[1] != 0x04 && commbuf[1] != 0x03 && commbuf[1] != 0x06)
	{
		return 254;
	}

	switch (commbuf[1])
	{
	case 0x06:
		lenframe = 8;
		break;
	case 0x03:
		lenframe = 7;
		break;
	case 0x10:
	case 0x04:
		lenframe = 9;
		break;
	default:
		lenframe = 7;
		break;
	}

	if (lentemp1 < lenframe)
		return 253;

	if (0x10 == commbuf[1])
	{
		lentemp2 = commbuf[4] * 256 + commbuf[5];
		if (lentemp2 != commbuf[6])
			return 252;
		lenframe = lentemp2 + 9;
		lencomm = lenframe - lentemp1;

		lentemp2 = ReadComPort(portid, commbuf, lencomm);
		if ((lentemp1 + lentemp2) < lenframe)
			return 251;
	}
	crcval = crc(commbuf, lenframe - 2);

	b1 = crcval / 256;
	b2 = crcval % 256;

	if (b1 != commbuf[lenframe - 2] || b2 != commbuf[lenframe - 1])
		return 250;

	if (commbuf[1] != 0x10) //本程序暂时不处理其他功能码，以非法功能直接返回
	{
		commbuf[1] += 0x80;
		commbuf[2] = 1;

	    crcval = crc(commbuf, 3);
		
	    commbuf[3] = crcval / 255;
		commbuf[4] = crcval % 255;
	    WriteComPort(portid, commbuf, 5);

	}	

	regAddr = commbuf[2] * 256 + commbuf[3];
	AnalysFun10(1,regAddr, (unsigned short *)&commbuf[7]);

	crcval = crc(commbuf, 6);

	commbuf[6] = crcval / 256;
	commbuf[7] = crcval % 256;

	WriteComPort(portid, commbuf, 8);

	return 0;
}

void *serial_thread(void *arg)
{

	int portid = (int)arg;
	int taskid;
	int ret;

	printf("端口号 =%d \"n", portid);
	Uart_Init(portid, pParaBams->baud[portid]);
	while (1)
	{
		ret = doRecvFunTasks(portid);
		if (ret != 0)
			usleep(100000); //延时100ms
	}
}

void CreateThreads(void *para)
{
	pthread_t ThreadID;
	pthread_attr_t Thread_attr;
	int i;
	memcpy((void *)pParaBams, (PARA_BAMS *)para, sizeof(PARA_BAMS));
	for (i = 0; i < pParaBams->portnum; i++)
	{
		if (FAIL == CreateSettingThread(&ThreadID, &Thread_attr, (void *)serial_thread, (int *)i, 1, 1))
		{
			printf("serial_thread CREATE ERR!\n");

			exit(1);
		}
	}
	printf("serial_thread CREATE success!\n");
}