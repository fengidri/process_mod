#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <string>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>
#include "proc_class.h"
#include "tools.h"
using namespace std;




//CProcess 类的定义
CProcess::CProcess(int pid)
{ 
    m_iPid = pid;
    Update( );
}
CProcess::CProcess()
{ 
}
CProcess::~CProcess()
{ 
}

int CProcess::Update()
{ 
    char cTmpBuffer[4096]; 
    char *pPos;

    int iFd;
    int iLen;

    sprintf(cTmpBuffer, "/proc/%d/stat", m_iPid);

    iFd = open(cTmpBuffer, O_RDONLY);
    iLen = read(iFd, cTmpBuffer, sizeof(cTmpBuffer) - 1);
    close(iFd);

    cTmpBuffer[iLen] = '\0';



    pPos = cTmpBuffer;
    pPos = strchr(pPos, '(') + 1;/* skip pid */

    //name
    {
        char *q = strrchr(pPos, ')');
        unsigned int len;
        len = q - pPos; 
        if (len >= sizeof(m_cName))
        { 
            len = sizeof(m_cName) - 1;
        }
        memcpy(m_cName, pPos, len);
        m_cName[len] = 0;

        pPos = q + 1;
    }

    //state
    pPos = g_SkipWs(pPos);
    m_cState.Push(*pPos++); 

    //ppid
    pPos = g_SkipWs(pPos);
    m_iPpid = atoi(pPos); 

    //pgrp
    pPos = g_NextToken(pPos);
    m_iPgrp.Push(atoi(pPos)); 

    /*session */
    pPos = g_NextToken(pPos);

    /*tty */
    pPos = g_NextToken(pPos);

    /*tty pgrp */
    pPos = g_NextToken(pPos);

    /* flags */
    pPos = g_NextToken(pPos);

    /* min flt */
    pPos = g_NextToken(pPos);

    /* cmin flt */
    pPos = g_NextToken(pPos);

    /* maj flt */
    pPos = g_NextToken(pPos);

    /* cmaj flt */
    pPos = g_NextToken(pPos);

    /* utime  */
    pPos = g_NextToken(pPos);
    m_ulUtime.Push(strtoul(pPos, NULL, 10));

    /* stime  */
    pPos = g_NextToken(pPos);
    m_ulStime.Push(strtoul(pPos, NULL, 10));

    /* cutime  */
    pPos = g_NextToken(pPos);

    /* cstime  */
    pPos = g_NextToken(pPos);

    /* priority  */
    pPos = g_NextToken(pPos);
    m_lPriority.Push(strtol(pPos, NULL, 10));

    /* nice  */
    pPos = g_NextToken(pPos);
    m_lNice.Push(strtol(pPos, NULL, 10));

    //num_threads
    pPos = g_NextToken(pPos);
    // m_lNice.Push(strtol(pPos, NULL, 10));

    //itrealvalue
    pPos = g_NextToken(pPos);

    //starttime
    pPos = g_NextToken(pPos);
    m_ullStartTime = strtoull(pPos, NULL,10);

    //vsize
    pPos = g_NextToken(pPos);

    //rss
    pPos = g_NextToken(pPos);
    m_lRss.Push(strtol(pPos, &pPos, 10)); 

    return 0;

}
//输出参数
const int CProcess::Pid()
{ 
    return m_iPid;
}
const char *CProcess::Name()
{ 
    return m_cName;
}
const int CProcess::Ppid()
{ 
    return m_iPpid;
}

const CSqQueue <char> *CProcess::State()
{ 
    return  &m_cState;

}
const CSqQueue <long> *CProcess::Priority()
{ 
    return  &m_lPriority;

}
const CSqQueue <long> *CProcess::Nice()
{ 
    return  &m_lNice;

}
const CSqQueue <long> *CProcess::Rss()
{ 
    //返回CSqQueue对象
    //记录了进程所占用的页数,
    return  &m_lRss;

}


const CSqQueue <unsigned long> *CProcess::Utime()
{ 
    return  &m_ulUtime;
}

const CSqQueue <unsigned long> *CProcess::Stime()
{ 
    return  &m_ulStime;
}

//返回进程最后一次的cpu时间与前一次的cpu时间的差值
unsigned long CProcess::TotalTime()
{ 
    return m_ulUtime.Tail() 
        + m_ulStime.Tail()
        - m_ulUtime.LastButOne()
        - m_ulStime.LastButOne();

}

const unsigned long long  CProcess::StartTime()
{ 
    // 返回值:是进程对象的开始时间
    // 相对于系统的偏移的节拍数long long unsinged
    // 一秒是g_ullClockTicks个节拍
    return  m_ullStartTime;

}















