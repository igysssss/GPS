#include<stdio.h>  
#include<stdlib.h>  
#include<string.h>  
#include<sys/types.h>  
#include<sys/stat.h>  
#include<fcntl.h>  
#include<unistd.h>  
#include<termios.h>  
#include<string.h>  
#define BUF_SIZE 1024
typedef struct{
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
}date_time;
typedef struct{
     date_time D;
     char status;       
     double latitude;   
     double longitude;   
     char NS;             
     char EW;           
     double speed;        
     double high;        
}GPS_INFO;
int open_dev(char *dev)
{
    int fd = open( dev, O_RDWR| O_NDELAY );          
    if (-1 == fd)
    {
        perror("Can't Open Serial Port !");
        return -1;
    }
    else
 
    return fd;
}
int init_serial(int fd,int nSpeed, int nBits, char nEvent, int nStop)  
{  
    struct termios newtio,oldtio;  
    if(tcgetattr( fd,&oldtio)  !=  0) {   
        perror("SetupSerial 1");  
        return -1;  
    }  
    bzero( &newtio, sizeof( newtio ) );  
    newtio.c_cflag  |=  CLOCAL | CREAD;  
    newtio.c_cflag &= ~CSIZE;  
  
    switch( nBits )  
    {  
    case 7:  
        newtio.c_cflag |= CS7;  
        break;  
    case 8:  
        newtio.c_cflag |= CS8;  
        break;  
    }  
  
    switch( nEvent )  
    {  
    case 'O':  
        newtio.c_cflag |= PARENB;  
        newtio.c_cflag |= PARODD;  
        newtio.c_iflag |= (INPCK | ISTRIP);  
        break;  
    case 'E':   
        newtio.c_iflag |= (INPCK | ISTRIP);  
        newtio.c_cflag |= PARENB;  
        newtio.c_cflag &= ~PARODD;  
        break;  
    case 'N':    
        newtio.c_cflag &= ~PARENB;  
        break;  
    }  
  
    switch( nSpeed )  
    {  
    case 2400:  
        cfsetispeed(&newtio, B2400);  
        cfsetospeed(&newtio, B2400);  
        break;  
    case 4800:  
        cfsetispeed(&newtio, B4800);  
        cfsetospeed(&newtio, B4800);  
        break;  
    case 9600:  
        cfsetispeed(&newtio, B9600);  
        cfsetospeed(&newtio, B9600);  
        break;  
    case 115200:  
        cfsetispeed(&newtio, B115200);  
        cfsetospeed(&newtio, B115200);  
        break;  
    case 460800:  
        cfsetispeed(&newtio, B460800);  
        cfsetospeed(&newtio, B460800);  
        break;  
    default:  
        cfsetispeed(&newtio, B9600);  
        cfsetospeed(&newtio, B9600);  
        break;  
    }  
    if( nStop == 1 )  
        newtio.c_cflag &=  ~CSTOPB;  
    else if ( nStop == 2 )  
    newtio.c_cflag |=  CSTOPB;  
    newtio.c_cc[VTIME]  = 0;  
    newtio.c_cc[VMIN] = 100; 
    tcflush(fd,TCIFLUSH);  
    if((tcsetattr(fd,TCSANOW,&newtio))!=0)  
    {  
        perror("com set error");  
        return -1;  
    }  
    
    return 0;  
}  

