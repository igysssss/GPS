#include<stdio.h>  
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
int init_serial
