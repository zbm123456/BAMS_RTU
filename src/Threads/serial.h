#ifndef _SERIAL_H
#define _SERIAL_H 1


/* serial.c */
int OpenComPort (int ComPort, int baudrate, int databit,
                   const char *stopbit, char parity);
void CloseComPort (unsigned char comid);

int ReadComPort (unsigned char comid,void *data, int datalength);
int  WriteComPort (unsigned char comid,unsigned char * data, int datalength);

/**
 * export serial fd to other program to perform
 * directly read, write to serial.
 * 
 * @return serial's file description 
 */
int getPortFd(unsigned char comid);

#endif /* serial.c */

