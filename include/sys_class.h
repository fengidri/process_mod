/********************************************************
  @Author:   丁雪峰
  @File:     sys_class.h
  @LastTime: 2013-08-07 18:34:40
  @Version:  1.0.2
  Description:
      定义了系统类,包括了各种参数.
      还有采集与输出方法.
      
      由DataApi通过CollectInfo调用
********************************************************/
#ifndef _SYS_CLASS
#define _SYS_CLASS

#include "SqQueue.h"
#include "tools.h"

class CSystem
{ 
    private:
        //user time
        CSqQueue<unsigned long> m_Utime;

        //nice time
        CSqQueue<unsigned long> m_Ntime;

        //system
        CSqQueue<unsigned long> m_Stime;

        //idle 
        CSqQueue<unsigned long> m_Itime;

        //iowait
        CSqQueue<unsigned long> m_Iotime;

        //irq
        CSqQueue<unsigned long> m_Irtime;

        //steal
        CSqQueue<unsigned long> m_Sttime;

        //guest
        CSqQueue<unsigned long> m_Gtime;

        //guest_nice
        CSqQueue<unsigned long> m_Gntime;

        //uptime
        long double m_Uptime;

    public:
        //CSystem();
        //~CSystem();
        
        //更新数据
        int Update();

        //返回系统运行到现在的时间
        //long double 以秒为单位
        long double Uptime();

        //返回系统最后一次的总时间与前一次的总时间的差值
        //单位是系统节拍
        //由于cpu计算的复杂性,在这里没有返回SqQueue对象,
        //而时返回当前一秒的使用总时间
        unsigned long TotalTime();
        const CSqQueue<unsigned long> *Utime();
        const CSqQueue<unsigned long> *Ntime();
        const CSqQueue<unsigned long> *Stime();
        const CSqQueue<unsigned long> *Itime();
        const CSqQueue<unsigned long> *Iotime();
        const CSqQueue<unsigned long> *Irtime();
        const CSqQueue<unsigned long> *Sttime();
        const CSqQueue<unsigned long> *Gtime();
        const CSqQueue<unsigned long> *Gntime();
};

#endif
