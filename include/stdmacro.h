/********************************************************
  @   Author: 丁雪峰
  @     File: stdmacro.h
  @ LastTime: 2013-08-09 12:41:16
  @  Version: 1.0.11
  File:stdmacro.h
  Description:
      定义了在项目中会用到的一些宏变量
********************************************************/
#ifndef _BASE_MACRO
#define _BASE_MACRO


#define TRUE 1
#define FALSE 0

#ifndef OK
#define OK 0
#endif

#define ERROR  1
#define OVERFLOW 2

#define ONEK 1024

//CSqQueue数据队列的长度
#define QUEUE_MAX_LEN 100

//Process
#define PROC_NAME_LEN 64
#define MAX_LEN_E_PROC 100
#define ONE_MB (1024 * 1024)


//在Display中用到的变量
//一次滚动的数量
#define SCROLL_DOWN 1
#define SCROLL_UP -1

//位置参数
#define CURSES_PAD_X 0
#define CURSES_PAD_WIN_Y 3
#define CURSES_PAD_WIN_X 0
#define CURSES_DISPLAY_PROC_MAX 200
#define CURSES_MAIN_OFFSET_Y 3
#define CURSES_MAIN_OFFSET_X 0

//输出num name state rss的宽度
const int CURSES_DISPLAY_PROC_PID = 5;
const int CURSES_DISPLAY_PROC_NAME = 14;
const int CURSES_DISPLAY_PROC_STATE = 2;
const int CURSES_DISPLAY_PROC_RSS = 14;
const int CURSES_DISPLAY_PROC_CPU = 9; 
const int CURSES_DISPLAY_PROC_PRI = 9;
const int CURSES_DISPLAY_PROC_NI = 5;
const int CURSES_DISPLAY_PROC_STARTTIME = 14;

#define CURSES_DISPLAY_SUBMIN_HEIGHT (LINES - 5)
#define CURSES_DISPLAY_MIN_HEIGHT (LINES - 3)

//数据采集间隔时间
const int COLLECT_SLEEP_TIME = 1;

//显示输出间隔时间
const int DISPLAY_SLEEP_TIME = 1;

//display list pid mode
const int DIS_LIST_PID  = 0;
//display list mem mode
const int DIS_LIST_MEM  = 1;
//display list cpu mode
const int DIS_LIST_CPU  = 2;

//display list
const int DIS_MODE_LIST =  1;

//display focus
#define DIS_MODE_FOCUS 2 
//web sock json

//*服务器监听端口号 */
const int SERVERPORT =  3333;

/* 最大同时连接请求数 */
const int BACKLOG  = 10;

//json
const int JSON_PROC_LEN = 550;
const int JSON_ALL_PROC_LEN = 15000;


/**请求头的长度
 *
 */
const int HTTP_REQUEST_LEN = 200;
const int HTTP_RESPONSE_LEN = 250;
#define HTTP_PROTOCOL   "HTTP/1.0"
#define HTTP_TIME_FORMAT   "%a, %d %b %Y %H:%M:%S GMT"
#define HTTP_SERVER_NAME  "system moni"

#endif
























