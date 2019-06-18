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
    int fd = open( dev, O_RDWR| O_NDELAY );         //| O_NOCTTY | O_NDELAY   
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
    newtio.c_cc[VTIME]  = 0;//重要  
    newtio.c_cc[VMIN] = 100;//返回的最小�? 重要  
    tcflush(fd,TCIFLUSH);  
    if((tcsetattr(fd,TCSANOW,&newtio))!=0)  
    {  
        perror("com set error");  
        return -1;  
    }  
    
    return 0;  
}  
char * get_gprmc (char * buf)
{
    char *buff=buf;
    char *target="$GPRMC";
    char *p=NULL;
                
    if((p=strstr(buff,target))==NULL)
    {
        printf("No fonud the string GPRMC\n");
        return 0;
    }
    return p;
}
char * get_gpgga (char * buf)
{
    char *buff=buf;
    char *target="$GPGGA";
    char *p=NULL;
                
    if((p=strstr(buff,target))==NULL)
    {
        printf("No fonud the string GPGGA\n");
        return 0;
    }
    return p;
}
static int getcomma(int num,char *str)
{
    int i,j=0;
    int len=strlen(str);
    for(i=0;i<len;i++)
    {
        if(str[i]==',')j++;
        if(j==num)return i+1; 
    }
    return 0;
}
static double get_double_number(char *s)
{
    char buf[BUF_SIZE];
    int i;
    double rev;
    i=getcomma(1,s);   
    strncpy(buf,s,i);
    buf[i]=0;         
    rev=atof(buf);   
    return rev;
}
static void UTC2BTC(date_time *GPS)  
{  
//如果秒号先出,再出时间数据,则将时间数据+1�? 
        GPS->second++; //加一�? 
        if(GPS->second>59){  
            GPS->second=0;  
            GPS->minute++;  
            if(GPS->minute>59){  
                GPS->minute=0;  
                GPS->hour++;  
            }  
        }     
  
        GPS->hour+=8;        //北京时间跟UTC时间相隔8小时  
        if(GPS->hour>23)  
        {  
            GPS->hour-=24;  
            GPS->day+=1;  
            if(GPS->month==2 ||GPS->month==4 ||GPS->month==6 ||GPS->month==9 ||GPS->month==11 ){  
                if(GPS->day>30){          //上述几个月份�?0天每月，2月份还不�?0  
                    GPS->day=1;  
                    GPS->month++;  
                }  
            }  
            else{  
                if(GPS->day>31){          //剩下的几个月份都�?1天每�? 
                    GPS->day=1;  
                    GPS->month++;  
                }  
            }  
            if(GPS->year % 4 == 0 ){ 
                if(GPS->day > 29 && GPS->month ==2){       //闰年的二月是29�? 
                    GPS->day=1;  
                    GPS->month++;  
                }  
            }  
            else{  
                if(GPS->day>28 &&GPS->month ==2){      //其他的二月是28天每�? 
                    GPS->day=1;  
                    GPS->month++;  
                }  
            }  
            if(GPS->month>12){  
                GPS->month-=12;  
                GPS->year++;  
            }         
        }  
} 
/*此函数传入两个字符串$GPRMC �?GPGGA的首地址*/
void gps_parse(char *line1,char *line2,GPS_INFO *GPS)  
{  
    int i,tmp,start,end;  
    char* buf=line1;  
    char* buff=line2;  
    GPS->D.hour   =(buf[ 7]-'0')*10+(buf[ 8]-'0');  
    GPS->D.minute =(buf[ 9]-'0')*10+(buf[10]-'0');  
    GPS->D.second =(buf[11]-'0')*10+(buf[12]-'0');  
    
    tmp = getcomma(9,buf);      //得到�?个逗号的下一字符序号  
    GPS->D.day    =(buf[tmp+0]-'0')*10+(buf[tmp+1]-'0');  
    GPS->D.month  =(buf[tmp+2]-'0')*10+(buf[tmp+3]-'0');  
    GPS->D.year   =(buf[tmp+4]-'0')*10+(buf[tmp+5]-'0')+2000;  
    
    GPS->status   =buf[getcomma(2,buf)];     //状�? 
    GPS->latitude =get_double_number(&buf[getcomma(3,buf)])/100; //纬度  
    GPS->NS       =buf[getcomma(4,buf)];             //南北�? 
    GPS->longitude=get_double_number(&buf[getcomma(5,buf)])/100; //经度  
    GPS->EW       =buf[getcomma(6,buf)];             //东西�? 
    
    UTC2BTC(&GPS->D);                        //转北京时�? 
 
    GPS->high     = get_double_number(&buff[getcomma(9,buff)]);   
} 
void show_gps(GPS_INFO *GPS)  
{   
    printf("\n");
    printf("DATE     : %ld-%02d-%02d \n",GPS->D.year,GPS->D.month,GPS->D.day);  
    printf("TIME     : %02d:%02d:%02d \n",GPS->D.hour,GPS->D.minute,GPS->D.second);  
    printf("Latitude : %4.4f %c\n",GPS->latitude,GPS->NS);     
    printf("Longitude: %4.4f %c\n",GPS->longitude,GPS->EW);    
    printf("high     : %4.4f \n",GPS->high);      
    printf("STATUS   : %c\n",GPS->status);     
}  
int main(void)  
{  
    int  fd,nset1,nread;  
    char buf[BUF_SIZE];  
    char *buff_gprmc,*buff_gpgga;  
    GPS_INFO GPS;
    
    fd = open_dev("/dev/ttyS1");//打开串口  
     
    nset1 = init_serial(fd,4800, 8, 'N', 1);//设置串口属�? 
    if (nset1 == -1)  
        exit(1);  
  
    while(1)  
    {   
        sleep(2);   
memset(buf,0,BUF_SIZE);   
        nread = read(fd, buf, BUF_SIZE);//读串�? 
        if (nread > 0)
        {  
            printf("\nGPS DATALen=%d\n",nread);   
            buf[nread] = '\0';  
            printf( "GPS information as follow:\n\n%s\n", buf); //输出所读数�? 
        }  
       
        buff_gprmc=get_gprmc(buf);
        buff_gpgga=get_gpgga(buf);
        gps_parse(buff_gprmc,buff_gpgga,&GPS);
        show_gps(&GPS) ;
    }  
    close(fd);  
    return 0;  
}  

