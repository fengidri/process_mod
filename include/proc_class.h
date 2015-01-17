/********************************************************
  @Author:   丁雪峰
  @File:     proc_class.h
  @LastTime: 2013-08-07 18:33:14
  @Version:  1.0.1
  Description:
      定义了进程类,包括了每个进程的各种参数.
      还有采集与输出方法.
      
      在GetAllProcs模块中会调用Update方法,更新新的数据

      在CDataApi中会调用种接口返回相应的值
********************************************************/
#ifndef _PROC_CLASS
#define _PROC_CLASS

#include <stdio.h>
#include <unistd.h>

#include "SqQueue.h"
#include "stdmacro.h"

using namespace std;

//实例化需要pid
class CProcess
{
    private:
        //进程pid
        int m_iPid;

        char m_cName[PROC_NAME_LEN];

        //进程的父进程的pid
        int m_iPpid;

        //进程的状态
        CSqQueue <char> m_cState;

        //线程组号
        CSqQueue <int> m_iPgrp;

        //会话组id
        //CSqQueue <int> m_iSession;

        //终端的设备号
        //CSqQueue <int> m_iTtyNr;

        //终端的进程组号
        //CSqQueue <int> m_iTpgid;

        //   unsigned m_flags;

        //该任务不需要从硬盘拷数据而发生的缺页（次缺页）的次数
        //CSqQueue <long unsigned> m_ulMinflt;

        // 累计的该任务的所有的waited-for进程曾经发生的次缺页的次数目
        //CSqQueue <long unsigned> m_ulCminflt;

        //该任务需要从硬盘拷数据而发生的缺页（主缺页）的次数
        //CSqQueue <long unsigned> m_ulMajflt;

        //累计的该任务的所有的waited-for进程曾经发生的主缺页的次数目
        //CSqQueue <long unsigned> m_ulCmajflt;

        //该任务在用户态运行的时间，单位为jiffies
        CSqQueue <long unsigned> m_ulUtime;

        //该任务在核心态运行的时间，单位为jiffies
        CSqQueue <long unsigned> m_ulStime;

        //累计的该任务的所有的waited-for进程曾经在用户态运行的时间，单位为jiffies
        //CSqQueue <long> m_lCutime;

        //累计的该任务的所有的waited-for进程曾经在核心态运行的时间，单位为jiffies
        //CSqQueue <long> m_lCstime;

        //任务的动态优先级
        CSqQueue <long> m_lPriority;

        //任务的静态优先级
        CSqQueue <long> m_lNice;

        //该任务所在的线程组里线程的个数
        CSqQueue <long> m_lNumThreads;

        //由于计时间隔导致的下一个 SIGALRM 发送进程的时延，以 jiffy 为单位.
        //CSqQueue <long> m_lItrealvaule;

        //该任务启动的时间，单位为jiffies
        long long unsigned m_ullStartTime;

        //该任务的虚拟地址空间大小
        //CSqQueue <long> m_lVsize;

        //该任务当前驻留物理地址空间的大小
        CSqQueue <long> m_lRss;

        //该任务能驻留物理地址空间的最大值
        //CSqQueue <long unsigned> m_ulRsslim;

        //该任务在虚拟地址空间的代码段的起始地址
        //CSqQueue <long unsigned> m_ulStartcode;

        //该任务在虚拟地址空间的代码段的结束地址
        //CSqQueue <long unsigned> m_ulEndcode;

        //esp(32 位堆栈指针) 的当前值, 与在进程的内核堆栈页得到的一致.
        //CSqQueue <long unsigned> m_ulStartstack;

        //CSqQueue <long unsigned> m_ulKstkesp;

        //CSqQueue <long unsigned> m_ulKstkeip;


        //CSqQueue <long unsigned> m_ulSignal;

        //阻塞信号的位图
        //CSqQueue <long unsigned> m_ulBlocked;
        //CSqQueue <long unsigned> m_ulSigignore;
        //CSqQueue <long unsigned> m_ulSigcatch;
        //CSqQueue <long unsigned> m_ulWchan;



    public:
        CProcess(int pid);
        CProcess();
        ~CProcess();

        /****************************************************************
         * Function:update
         * Description:
         *     根据进程的pid,分析/proc/[pid]/stat文件提取各种信息
         *     供GetAllProc模块使用,更新进程数据
         * Input:无
         * Output:
         *     成功时返回0
         * Return:
         * Others:
         ****************************************************************/
        int Update();

        /****************************************************************
         * Function:Pid Ppid Name
         * Description:
         *     直接返回pid ppid name
         *     并且是不能修改的
         * Input:
         * Output:
         *     直接返回pid ppid name
         * Return:
         * Others:
         ****************************************************************/
        const int Pid();
        const int Ppid();
        const char *Name();
        const unsigned long long  StartTime();

        const CSqQueue<char> *State();
        const CSqQueue<long> *Rss();
        const CSqQueue<long> *Priority();
        const CSqQueue<long> *Nice();
        const CSqQueue <unsigned long> *Utime();
        const CSqQueue <unsigned long> *Stime();

        //返回进程最后一次的cpu时间与前一次的cpu时间的差值
        unsigned long TotalTime();
};
#endif
