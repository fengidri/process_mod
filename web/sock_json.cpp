/********************************************************
  @   Author: 丁雪峰
  @     File: sock_json.cpp
  @ LastTime: 2013-08-12 16:37:09
  @  Version: 1.0.185
********************************************************/
#include "sock_json.h"
#include <time.h>
#include <fcntl.h>





/**\brief  进入sock_json的入口函数
 * \note
 *    会启动一个简易的web服务器
 *
 */
int SockJsonEnter()
{
    //socket
    int iSockFd;
    int iClientFd;    
    socklen_t SinSize;

    struct sockaddr_in LocalAddr;    /* 本机地址信息 */
    struct sockaddr_in RemoteAddr;    /* 客户端地址信息 */


    if((iSockFd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket error！");
        exit(1);
    }

    LocalAddr.sin_family=AF_INET;
    LocalAddr.sin_port=htons(SERVERPORT);
    LocalAddr.sin_addr.s_addr = INADDR_ANY;

    bzero(&(LocalAddr.sin_zero),8);

    printf("The web server for the system monitor is started.\n This ip is [%s]. The port is [%d].\n", 
            inet_ntoa(LocalAddr.sin_addr), 
            SERVERPORT);
    //bind
    if(bind(iSockFd, (struct sockaddr *)&LocalAddr, sizeof(struct sockaddr)) == -1) {
        perror("bind出错！");
        exit(1);
    }

    //listen
    if(listen(iSockFd, BACKLOG) == -1) {
        perror("listen出错！");
        exit(1);
    }

    int i = 0;
    //server
    while (1) 
    {
        SinSize = sizeof(struct sockaddr_in);

        //accept
        if((iClientFd = accept(iSockFd, (struct sockaddr *)&RemoteAddr, &SinSize)) == -1) {
            perror("accept出错");
            continue;
        }

        i++;

        printf("%d, received a connection from %s\n", i, inet_ntoa(RemoteAddr.sin_addr));

        //if (fork() == 0)
        {    /* 子进程代码段 */
            CJsonServer JsonServer(iClientFd);
            JsonServer.Server();
            //exit(0);
        }
        //close(iClientFd);
    
    }
    return 0;
}


static void Server(int iClientFd)
{ 

    //TODO 控制这里的大小
    int iContextLen = 23072;
    char *pContext = (char *)malloc(iContextLen);//TODO free
    memset(pContext, 0, iContextLen);

    const int iBufferLen = 2024;
    char *pBuffer = (char *)malloc(iBufferLen);
    memset(pBuffer, 0, iBufferLen);

    const int iRequestLen = 100;

    char pRequest[iRequestLen];
    char * pUrl = NULL;
    char * pMethod = NULL;
    char * pTmp  = NULL;

    int m_iState=404;;
    const char *cTitle = "Not Found";

    //读取request
    if (recv(iClientFd, pRequest, iRequestLen, 0) == 0)
    { 
        m_iState=404;;
        cTitle = "Not Found";

        ResponseHeader(m_iState, cTitle, "text/html", strlen(pContext), pBuffer);
        //传递头
        if(send(iClientFd, pBuffer, strlen(pBuffer), 0) == -1)
        {
            perror("send error 1.\n");
        }
        exit(0);
    }


    printf("the request url:%s\n", pUrl);




    printf("\n");

    free(pContext);
    free(pBuffer);
    close(iClientFd);
    exit(0);
}


CJsonServer::CJsonServer(int iClientFd)
{ 
    m_iState = 404;
    m_pTitle = "Not Found";
    m_iClientFd = iClientFd;

    m_pContext = NULL;
    m_pResponse = NULL;
    m_pRequest = NULL;

}
CJsonServer::~CJsonServer()
{ 
    if (m_pContext != NULL)
    { 
        free(m_pContext);
        m_pContext = NULL;
    }

    if (m_pRequest != NULL)
    { 
        free(m_pRequest);
        m_pRequest = NULL;

    }
    if (m_pResponse != NULL)
    { 
        free(m_pResponse);
        m_pResponse = NULL;
    }
    close(m_iClientFd);

}

char *CJsonServer::GetToken(char **pDes, char *const pSrc)
{ 
    char *pTmp = pSrc;

    while (*pTmp == ' ' || *pTmp == '\r' || *pTmp == '\n')
    { 
        pTmp++;
    }

    *pDes = pTmp;

    while (*pTmp != ' '  && *pTmp != '\r')
    { 
        pTmp++;

    }

    return pTmp;

}

void CJsonServer::ReadRequest()
{ 
    char *pTmp;

    int iSize = HTTP_REQUEST_LEN;
    int iTimes = 1;

    m_pRequest = (char *)malloc(iSize * iTimes);
    pTmp = m_pRequest;

    while (read(m_iClientFd, pTmp, iSize) == iSize)
    { 
        int iLen;

        iLen = pTmp + iSize - m_pRequest;
        iTimes++;

        m_pRequest = (char *)realloc((void *)m_pRequest,  iSize * iTimes);
        pTmp = m_pRequest + iLen;

    }
}

void CJsonServer::AnalysisRequest()
{ 
    char *pTmp = m_pRequest;


    //method
    pTmp = GetToken( &m_pMethod, pTmp);
    *pTmp = '\0';
    pTmp++;

    //url
    pTmp = GetToken( &m_pUrl, pTmp);
    *pTmp = '\0';

}


void CJsonServer::ResponseHeader(int Len, const char *pMimeType)
{ 
    //参数
    //m_iStatus  m_pTitle pMimeType Len pResponeHeader
    m_pResponse = (char *)malloc(HTTP_RESPONSE_LEN);

    char tmp[100];

    char timebuff[55];
    time_t now;
    now = time((time_t*)0);

    memset(timebuff, 0, sizeof(timebuff));

    sprintf(tmp, "%s %d %s\r\n", HTTP_PROTOCOL, m_iState, m_pTitle);
    strcpy(m_pResponse, tmp);

    strftime(timebuff, sizeof(timebuff), HTTP_TIME_FORMAT, gmtime(&now));
    (void) sprintf(tmp, "Date: %s\r\n", timebuff);
    strcat(m_pResponse, tmp);
    
    sprintf(tmp, "Cache-Control:no-cache, max-age=0\r\n");
    sprintf(tmp, "Server: %s\r\n", HTTP_SERVER_NAME);
    strcat(m_pResponse, tmp);

    //sprintf(tmp, "Connection: Keep-Alive\r\n");
    //strcat(m_pResponse, tmp);

    sprintf(tmp, "Content-Type: %s; charset=utf8\r\n", pMimeType);
    strcat(m_pResponse, tmp);

    if (Len < 0)
    { 
        strcat(m_pResponse, "Transfer-Encoding:chunked\r\n");
    }
    else
    { 
        sprintf(tmp, "Content-Length: %d\r\n", (int)Len);
        strcat(m_pResponse, tmp);
    }

    sprintf(tmp, "Content-Language: zh-CN\r\n");
    strcat(m_pResponse, tmp);

    strcat(m_pResponse, "\r\n");
}

void CJsonServer::SendHttp()
{ 
    int iLenContext;
    if (m_pContext != NULL)
    { 
        iLenContext = strlen(m_pContext);

    }
    else
    { 
        iLenContext = 0;
    }
    ResponseHeader( iLenContext, "text/html");

    //传递头
    if(send(m_iClientFd, m_pResponse, strlen(m_pResponse), 0) == -1)
    {
        perror("header send error.\n");
    }

    if (m_iState == 200)
    { 
        //传递数据
        if(send(m_iClientFd, m_pContext, strlen(m_pContext), 0) == -1)
        {
            perror("constext send error2.\n");
        }
    }


}

/**\brief 返回一个json字符串.
 * \note
 *     这个字符串由所有进程的信息组成
 * \param
 *     参数是一个指针,这个指针是不可变的为const
 *     这个指针指向的空间中放入json字符串
 *
 */
void CJsonServer::JsonAllProc()
{ 
    const char *pPre ="{\"proc\":["; 
    const char *pPost = "]}";

    list<int> *PidList;

    int iSize = JSON_ALL_PROC_LEN;
    m_pContext = (char *)malloc(iSize);

    char *pTmp = m_pContext;

    PidList = DataApiPidList();

    strcpy(pTmp, pPre);
    pTmp = pTmp  +  strlen(pPre) ;
    
    char buffer[JSON_PROC_LEN];

    while (!PidList->empty())
    { 
        sprintf(buffer, "{\"pid\":%d,\"name\":\"%s\",\"state\":\"%c\",\"rss\":%ld,\"pri\":%ld,\"ni\":%ld,\"cpu\":%3.1f,\"time\":%lu},",
                PidList->front(), 
                DataApiProcName(),
                DataApiProcState(),
                DataApiProcMem(),
                DataApiProcPriority(),
                DataApiProcNice(),
                DataApiProcCpu(),
                DataApiProcRunTime(PidList->front())
               );
        if (strlen(buffer) + pTmp - m_pContext >=  iSize)
        { 
            int iLen;
            iLen = pTmp - m_pContext;
            iSize = iSize + 10 * JSON_PROC_LEN;
            m_pContext = (char *)realloc((void *)m_pContext, iSize);
            pTmp = m_pContext + iLen;

        }
        strcpy(pTmp, buffer);
        pTmp = pTmp + strlen(buffer);

        PidList->pop_front();
    }

    //为了删除结尾的','
    pTmp--;
    strcpy(pTmp, pPost);

}

void CJsonServer::JsonOneProc(int iPid)
{ 
    const char *pPre = "{\"pid\":%d,\"name\":\"%s\",\"state\":\"%c\",\"rss\":%ld,\"pri\":%ld,\"ni\":%ld,\"time\":%lu, \"cpuarray\":[";
    char *pTmp;
    char Buffer[10];

    m_pContext = (char *)malloc(600);

        sprintf(m_pContext, pPre,
                iPid, 
                DataApiProcName(),
                DataApiProcState(),
                DataApiProcMem(),
                DataApiProcPriority(),
                DataApiProcNice(),
                DataApiProcRunTime(iPid)
               );

    list<double> TmpList = DataApiProcCpuList(iPid);
    list<double>::iterator It;

    for (It = TmpList.begin(); It != TmpList.end(); It++)  
    { 
        sprintf(Buffer, "%3.1f,", *It);
        strcat(m_pContext, Buffer);

    }

    pTmp = m_pContext  + strlen(m_pContext)  - 1;
    strcpy(pTmp, "]}");

}
void CJsonServer::ReadFile(const char *pFile)
{ 
    int iFd;

    char *pTmp;
    int iLen;

    int iSize = 1000;
    int iTimes = 1;

    m_pContext = (char *)malloc(iSize * iTimes);
    pTmp = m_pContext;

    iFd = open(pFile, O_RDONLY);
    if (iFd ==  -1)
    { 
        m_iState=404;;
        m_pTitle = "Not Found";
    }
    else
    { 
        iLen = read(iFd, pTmp, iSize  - 1);
        while ( iLen ==  iSize  - 1)
        { 
            int iTmp;

            iTmp = pTmp  + iSize - m_pContext  - 1;

            iTimes++;

            m_pContext = (char *)realloc((void *)m_pContext, iSize * iTimes);
            pTmp = m_pContext  + iTmp;
            iLen = read(iFd, pTmp, iSize  - 1);

        }
        pTmp = pTmp  + iLen;
        *pTmp = '\0';
        close(iFd);
    }

}
void CJsonServer::ChooseHandle()
{ 
    printf("url:%s\n", m_pUrl);

    const char *pToCmp;

    pToCmp = "/";
    if (strcmp(m_pUrl, pToCmp) == 0)
    { 
        m_iState = 200;
        m_pTitle = "OK"; 
        ReadFile("index.html");
    }

    pToCmp = "/jslib";
    if (strcmp(m_pUrl, pToCmp) == 0)
    { 
        m_iState = 200;
        m_pTitle = "OK"; 
        ReadFile("jslib.js");
    }
    pToCmp = "/AllProc";
    if (strcmp(m_pUrl, pToCmp) == 0)
    { 
        m_iState = 200;
        m_pTitle = "OK"; 
        JsonAllProc();
    }

    pToCmp = "/OneProc/";
    if (strncmp(m_pUrl, pToCmp, strlen(pToCmp)) == 0)
    { 
        m_iState = 200;
        m_pTitle = "OK"; 
        int iPid;
        iPid = atoi(m_pUrl + strlen(pToCmp));
        JsonOneProc(iPid);

    }
}
void CJsonServer::Server()
{ 
    ReadRequest();
    AnalysisRequest();
    ChooseHandle();
    SendHttp();
}
