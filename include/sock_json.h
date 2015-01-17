/********************************************************
  @   Author: 丁雪峰
  @     File: sock_json.h
  @ LastTime: 2013-08-12 14:19:19
  @  Version: 1.0.46
  Description:
     输出模块.
     主要是调用了data中的data_api类,对于后台信息进行提取
     并设计了输出的界面样式.与交互操作
********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

#include <string>
#include <arpa/inet.h>

#include <jsoncpp/json.h>

#include <stdmacro.h>
#include <data.h>
using namespace std; 

static char* ResponseHeader(int iStatus, const char *pTitle, const char *pMimeType, off_t Len, char *pBuffer);

/**
 *
 *从客户请求中提取信息.
 *\param
 *     pDes用于保存字段的指针,当前为空
 *     pSrc是指向源字符的指针
 *\return
 *     返回的是指向字符串之后一位的指针
 *\note
 *     不论是pSrc是否指向一个字符还是一个空格/换行/回车,都会过滤掉
 *      从字符开始的字符串到空格/换行/回车结束
 *
 */
char *GetToken(char * pDes, char *const pSrc);


int SockJsonEnter();

///生成所有进程的json信息.
///注意:
///   starttime 是以秒为单位和的运行时间
///   pid, pri ni是int类型
///   name 是string
///   state 是char
///   rss是以k为单位的内存
void JsonAllProc(char *const pJson);

string Http();



static Json::Value JsonRoot;
static Json::Value JsonProc;
static Json::FastWriter JsonWriter;
static void Server(int );


class CJsonServer
{ 
    private:
        //保存数据
        char *m_pContext;
        char *m_pRequest;
        char *m_pResponse;

        char *m_pUrl;
        char *m_pMethod;

        const char *m_pTitle;
        int m_iState;
        int m_iClientFd;
    public:
        CJsonServer(int );
        ~CJsonServer();

        //用于AnalysisRequest
        char *GetToken(char **pDes, char *const pSrc);
        
        void ReadRequest();
        void AnalysisRequest();

        void ChooseHandle();


        void JsonAllProc();
        void JsonOneProc(int iPid);
        void ReadFile(const char * pIndex);



        /**
         *\brief 返回http头信息
         *\params
         *   iStatus:响应代号,如 200 404......       
         *   pTitle:是响应名称,如 OK ,NOT FOUND      
         *   pMimeType:是返回的文件的类型如:text/html
         *   Len:是返回的文件体的长度,如果Len小于0,则是chunked            
         *   pBuffer:是包含返回值的指针              
         *   
         */
        void ResponseHeader(int, const char *);
        void SendHttp();

        void Server();
};
