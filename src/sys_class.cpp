#include "sys_class.h"
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
//CSystem::CSystem()
//{ 
//}
//CSystem::~CSystem()
//{ 
//
//}
int CSystem::Update()
{ 

    char cTmpBuffer[4096]; 
    char *pPos;

    int iFd;
    int iLen;

    strcpy(cTmpBuffer, "/proc/stat");

    iFd = open(cTmpBuffer, O_RDONLY);
    iLen = read(iFd, cTmpBuffer, sizeof(cTmpBuffer) - 1);
    close(iFd);

    cTmpBuffer[iLen] = '\0';



    pPos = cTmpBuffer;
    //utime
    pPos = g_NextToken(pPos);
    m_Utime.Push(strtoul(pPos,  &pPos, 10));
    pPos = g_SkipWs(pPos);
    m_Ntime.Push(strtoul(pPos,  &pPos, 10));
    pPos = g_SkipWs(pPos);
    m_Stime.Push(strtoul(pPos,  &pPos, 10));
    pPos = g_SkipWs(pPos);
    m_Itime.Push(strtoul(pPos,  &pPos, 10));
    pPos = g_SkipWs(pPos);
    m_Iotime.Push(strtoul(pPos,  &pPos, 10));
    pPos = g_SkipWs(pPos);
    m_Irtime.Push(strtoul(pPos,  &pPos, 10));
    pPos = g_SkipWs(pPos);
    m_Sttime.Push(strtoul(pPos,  &pPos, 10));
    pPos = g_SkipWs(pPos);
    m_Gtime.Push(strtoul(pPos,  &pPos, 10));
    pPos = g_SkipWs(pPos);
    m_Gntime.Push(strtoul(pPos,  &pPos, 10));


    strcpy(cTmpBuffer, "/proc/uptime");

    iFd = open(cTmpBuffer, O_RDONLY);
    iLen = read(iFd, cTmpBuffer, sizeof(cTmpBuffer) - 1);
    close(iFd);

    cTmpBuffer[iLen] = '\0';
    m_Uptime = strtod(cTmpBuffer, NULL);

    return 0;




}

long double CSystem::Uptime()
{ 
    return m_Uptime;

}

unsigned long CSystem::TotalTime()
{ 
    return 
        m_Utime.Tail() 
        + m_Ntime.Tail()
        + m_Stime.Tail()
        + m_Itime.Tail()
        + m_Iotime.Tail()
        + m_Irtime.Tail()
        + m_Sttime.Tail()
        + m_Gtime.Tail()
        + m_Gntime.Tail()
        - m_Utime.LastButOne() 
        - m_Ntime.LastButOne()
        - m_Stime.LastButOne()
        - m_Itime.LastButOne()
        - m_Iotime.LastButOne()
        - m_Irtime.LastButOne()
        - m_Sttime.LastButOne()
        - m_Gtime.LastButOne()
        - m_Gntime.LastButOne();

}
const CSqQueue<unsigned long> *CSystem::Utime()
{ 
    return  &m_Utime;
}
const CSqQueue<unsigned long> *CSystem::Ntime()
{ 
    return  &m_Ntime;
}
const CSqQueue<unsigned long> *CSystem::Stime()
{ 
    return  &m_Stime;
}
const CSqQueue<unsigned long> *CSystem::Itime()
{ 
    return  &m_Itime;
}
const CSqQueue<unsigned long> *CSystem::Iotime()
{ 
    return  &m_Iotime;
}
const CSqQueue<unsigned long> *CSystem::Irtime()
{ 
    return  &m_Irtime;
}
const CSqQueue<unsigned long> *CSystem::Sttime()
{ 
    return  &m_Sttime;
}
const CSqQueue<unsigned long> *CSystem::Gtime()
{ 
    return  &m_Gtime;
}
const CSqQueue<unsigned long> *CSystem::Gntime()
{ 
    return  &m_Gntime;
}
