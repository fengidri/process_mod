#include <sys/stat.h>
#include <unistd.h>
#include "data.h"
#include "sys_class.h"
#include<algorithm>
//^^^^^^^^^^^^^^^[start:核心数据]^^^^^^^^^^^^^^^^^^^^^^//
//name:进程总集                                        //
static MAP_PROCES *s_pProcsMap;                        //
                                                       //
//name:系统对象                                        //
//s_pSystem 用于保存系统相关信息                       //
static CSystem *s_pSystem;                             //
                                                       //
//name:标志位                                          //
//description:                                         //
//对于一个数据队列的每一位元素进行编号                 //
//软件中的多个不同的数据队列是之间在尾端是对齐的       //
//那么对齐的元素之间的标志位是相同的                   //
static long s_lFlag;                                   //
//________________[end]________________________________//


static MAP_PROCES::iterator s_itIt;
bool s_itBool;

static pthread_t s_pthread_t;
list<int> s_liMapPid;

list<int> s_PidList;
int s_iListMode = DIS_LIST_CPU;

//^^^^^^^^^^^^^^^^[start:固有信息]^^^^^^^^^^^^^^^^^^^^^^^//
//系统的
//CPU 个数为
const long s_lNumProcs = sysconf (_SC_NPROCESSORS_CONF);

//系统页面的大小k
const long s_lPageSize = sysconf (_SC_PAGESIZE)/ONEK;

//系统中物理页数个数
const long s_lNumPages = sysconf (_SC_PHYS_PAGES);

//clock    ticks
const unsigned long long s_ullClockTicks = (unsigned long long)sysconf(_SC_CLK_TCK);
//__________________[end]______________________//


//^^^^^^^^^^^^^^^^[start:DataApiEnter]^^^^^^^^^^^^^^^^^^^^^^^//
int DataApiEnter()
{ 
    s_pProcsMap = new MAP_PROCES();
    s_pSystem = new CSystem();

    //prcess
    RefreshListKey();
    GetProcsInfo();
    EraseKilledProc();
    //systerm
    s_pSystem->Update();
    s_lFlag++;

    //启动线程
    pthread_create(&s_pthread_t, NULL, CollectInfo, NULL);
    return 0;
}
static int DataApiWait()
{ 
    pthread_join( s_pthread_t, NULL);
    return 0;
}
//线程函数
static void *CollectInfo(void *)
{ 
    while (TRUE)
    { 
        //prcess
        RefreshListKey();
        GetProcsInfo();
        EraseKilledProc();
        //systerm
        s_pSystem->Update();

        s_lFlag++;
        sleep(COLLECT_SLEEP_TIME); 
    }

    return NULL;
}
static int RefreshListKey()
{ 
        //生成一个由s_pProcsMap中的key组成的list
        static MAP_PROCES::iterator itIt;

        for (itIt = s_pProcsMap->begin(); itIt != s_pProcsMap->end(); itIt++)
        { 
            s_liMapPid.push_back(itIt->first);
        }
        return 0;
}
static int GetProcsInfo()
{ 
        static MAP_PROCES::iterator itIt;
        static CProcess *pTmpPro;

        //目录相关变量
        static DIR *s_pDir;
        static struct dirent *s_pEnt;

        //用于保存pid的int 与str值
        static const char *s_cPid;
        static int s_iPid;


        s_pDir = opendir("/proc");

        //分析全部进程,插入到s_pProcsMap中,
        //如果在pProcMap中已经有了,就更新数据
        while ((s_pEnt = readdir(s_pDir)) != NULL)
        {
            if (!isdigit(s_pEnt->d_name[0]))
            { 
                continue;
            }

            //pid 进程名
            s_cPid = s_pEnt->d_name;
            s_iPid = atoi(s_cPid);

            //删除list中的pid元素
            s_liMapPid.remove(s_iPid);

            itIt = s_pProcsMap->find(s_iPid);

            if(itIt == s_pProcsMap->end())
            {
                //新建
                pTmpPro = new CProcess(s_iPid);
                s_pProcsMap->insert(PAIR_PROCES(s_iPid, pTmpPro));
            }
            else 
            {
                //已存在
                itIt->second->Update();
            }
        }
        closedir(s_pDir);

        //分析有没有进程已经结束

        return 0;
}
static int EraseKilledProc()
{ 
        while (!s_liMapPid.empty())
        { 
            s_pProcsMap->erase(s_liMapPid.back());
            s_liMapPid.pop_back();
        }

        return 0;
}



//^^^^^^^^^^^^^^^^^^^^^^^^^^^[START:迭代器]^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^//
const int DataApiIteratorInit()
{ 
    s_itIt = s_pProcsMap->begin();
    s_itBool = TRUE;
    return 0;
}

MAP_PROCES::iterator  DataApiIterator()
{ 
    return s_itIt;
}

bool DataApiIteratorNext()
{ 
    s_itIt++;
    if (s_itIt == s_pProcsMap->end())
    { 
        s_itBool = FALSE;

    }
    return s_itBool;
}
//__________________[end]______________________//

//^^^^^^^^^^^^^^^^^^^^^^^^^[START:铺助函数]^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^//

static int MemCmp(const MEMPAIR& x, const MEMPAIR& y)  
{  
    return x.second > y.second;  
}  

static list<int> MemSort(MAP_PROCES * pProcMap)
{ 

    vector<MEMPAIR> vec;  
    list<int> TmpList;
    for (MAP_PROCES::iterator curr = pProcMap->begin(); curr != pProcMap->end(); ++curr)  
    {  
        vec.push_back(make_pair(curr->first, curr->second->Rss()->Tail()));  
    }  
    sort(vec.begin(), vec.end(), MemCmp);
    for (vector<MEMPAIR>::iterator it = vec.begin(); it != vec.end(); it++)
    { 
        TmpList.push_back(it->first);
    }
    return TmpList;
}

static int CpuCmp(const CPUPAIR& x, const CPUPAIR& y)  
{  
    return x.second > y.second;  
}  

static list<int> CpuSort(MAP_PROCES * pProcMap)
{ 

    vector<CPUPAIR> vec;  
    list<int> TmpList;
    for (MAP_PROCES::iterator curr = pProcMap->begin(); curr != pProcMap->end(); ++curr)  
    {  
        vec.push_back(make_pair(curr->first, curr->second->TotalTime()));  
    }  
    sort(vec.begin(), vec.end(), CpuCmp);
    for (vector<CPUPAIR>::iterator it = vec.begin(); it != vec.end(); it++)
    { 
        TmpList.push_back(it->first);
    }
    return TmpList;
}

//__________________[end]______________________//

//^^^^^^^^^^^^^^^^^^^^^^^^^^^[START:接口函数]^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^//
//输出进程数量
const int DataApiNumProc()
{ 
    return s_pProcsMap->size();
}

list<int> * DataApiPidList()
{ 
    static MAP_PROCES::iterator itIt;
    s_PidList.clear();
    switch (s_iListMode)
    { 
        case DIS_LIST_PID://按pid大小进行排序
            { 

                for (itIt = s_pProcsMap->begin(); itIt != s_pProcsMap->end(); itIt++)
                { 
                    s_PidList.push_back(itIt->first);
                }
                break;
            }
        case DIS_LIST_MEM://按Mem大小进行排序
            { 
                s_PidList = MemSort(s_pProcsMap);
                break;
            }
        case DIS_LIST_CPU://按CPU大小进行排序
            { 
                s_PidList = CpuSort(s_pProcsMap);
                break;
            }

    }
    return  &s_PidList; 



}

void DataApiListMode(int iMode)
{ 
    s_iListMode = iMode;
}
//输出指定进程的名字
const char * DataApiProcName(int pid)
{ 
    s_itIt = s_pProcsMap->find(pid);

    if(s_itIt == s_pProcsMap->end()) 
    {
        printf("test pid no find\n");
        return NULL;
    }
    else 
    {
        return s_itIt->second->Name();
    }

}

const char * DataApiProcName()
{ 
    return s_itIt->second->Name();
}

const int  DataApiProcPpid(int pid)
{ 
    s_itIt = s_pProcsMap->find(pid);

    if(s_itIt == s_pProcsMap->end()) 
    {
        printf("test pid no find\n");
        return  0;
    }
    else 
    {
        return s_itIt->second->Ppid();
    }

}

const int  DataApiProcPpid()
{ 
    return s_itIt->second->Ppid();
}

const long DataApiProcMem(int pid)
{ 
    s_itIt = s_pProcsMap->find(pid);

    if(s_itIt == s_pProcsMap->end()) 
    {
        printf("test pid no find\n");
        return 0;
    }
    else 
    {
        return s_itIt->second->Rss()->Tail() * s_lPageSize ;
    }
}

const long DataApiProcMem()
{ 
    return s_itIt->second->Rss()->Tail() * s_lPageSize;
}

//输出指定进程的Priority
const long  DataApiProcPriority(int pid)
{ 
    s_itIt = s_pProcsMap->find(pid);

    if(s_itIt == s_pProcsMap->end()) 
    {
        printf("test pid no find\n");
        return 0;
    }
    else 
    {
        return s_itIt->second->Priority()->Tail();
    }
}

const long  DataApiProcPriority()
{ 
    return s_itIt->second->Priority()->Tail();
}

//输出指定进程的Nice
const long DataApiProcNice(int pid)
{ 
    s_itIt = s_pProcsMap->find(pid);

    if(s_itIt == s_pProcsMap->end()) 
    {
        printf("test pid no find\n");
        return 0;
    }
    else 
    {
        return s_itIt->second->Nice()->Tail();
    }
}

const long DataApiProcNice()
{ 
    return s_itIt->second->Nice()->Tail();
}

const char DataApiProcState(int pid)
{ 
    s_itIt = s_pProcsMap->find(pid);

    if(s_itIt == s_pProcsMap->end()) 
    {
        printf("test pid no find\n");
        return 0;
    }
    else 
    {
        return s_itIt->second->State()->Tail();
    }
}

const char DataApiProcState()
{ 
    return s_itIt->second->State()->Tail();
}

//返回值是某进程在当前的cpu使用率double
const double DataApiProcCpu(int pid)
{ 
    s_itIt = s_pProcsMap->find(pid);

    if(s_itIt == s_pProcsMap->end()) 
    {
        printf("test pid no find\n");
        return  0;
    }
    else 
    {

        return s_lNumProcs*100*(double)s_itIt->second->TotalTime()
            /(double)s_pSystem->TotalTime();
    }
}

//返回值是某进程在当前的cpu使用率double
const double DataApiProcCpu()
{ 
        return s_lNumProcs*100*(double)s_itIt->second->TotalTime()
            /(double)s_pSystem->TotalTime();
}

const unsigned long DataApiProcRunTime(int pid)
{ 
    s_itIt = s_pProcsMap->find(pid);

    if(s_itIt == s_pProcsMap->end()) 
    {
        printf("test pid no find\n");
        return  0;
    }
    else 
    {
        return (unsigned long)(s_pSystem->Uptime()) 
             - (unsigned long)s_itIt->second->StartTime()/s_ullClockTicks;
    }

}

const unsigned long DataApiProcRunTime()
{ 
    return (unsigned long)(s_pSystem->Uptime()) 
        - (unsigned long)s_itIt->second->StartTime()/s_ullClockTicks;

}

const unsigned long long  DataApiProcStartTime(int pid)
{ 
    s_itIt = s_pProcsMap->find(pid);

    if(s_itIt == s_pProcsMap->end()) 
    {
        printf("test pid no find\n");
        return  0;
    }
    else 
    {
        return s_itIt->second->StartTime();
    }

}

const unsigned long long  DataApiProcStartTime()
{ 
    return s_itIt->second->StartTime();
}

list<double> DataApiProcCpuList(int pid, long flag)
{ 

    list<double> TmpList;
    s_itIt = s_pProcsMap->find(pid);

    if(s_itIt == s_pProcsMap->end()) 
    {
        printf("test pid no find\n");
    }
    else 
    {
        double ProcTmp;
        double SysTmp;
        int iOffset;
        int iSize;

        if (flag <  1)
        { 
            //还使用flag
            iOffset = 0;
        }
        else
        { 
            //由于cpu的数据是邻位的差值,所以请的flag比实际的flag大1
            flag--;
            if (s_lFlag - flag < QUEUE_MAX_LEN)
            { 
                iOffset = QUEUE_MAX_LEN - 1  - s_lFlag + flag;
            }
            else
            { 
                //请求的flag已经是过期的
                //返回空的list
                return TmpList;

            }

        }


        const CSqQueue<unsigned long> * pProcUtime = s_itIt->second->Utime();
        const CSqQueue<unsigned long> * pProcStime = s_itIt->second->Stime();

        const CSqQueue<unsigned long> * pSysUtime = s_pSystem->Utime();
        const CSqQueue<unsigned long> * pSysNtime = s_pSystem->Ntime();
        const CSqQueue<unsigned long> * pSysStime = s_pSystem->Stime();
        const CSqQueue<unsigned long> * pSysItime = s_pSystem->Itime();
        const CSqQueue<unsigned long> * pSysIotime = s_pSystem->Iotime();
        const CSqQueue<unsigned long> * pSysIrtime = s_pSystem->Irtime();
        const CSqQueue<unsigned long> * pSysSttime = s_pSystem->Sttime();
        const CSqQueue<unsigned long> * pSysGtime = s_pSystem->Gtime();
        const CSqQueue<unsigned long> * pSysGntime = s_pSystem->Gntime();

        iSize = pProcUtime->Size();

        while (iOffset  + 1  < iSize)
        { 
            ProcTmp = double((pProcUtime->Offset(iOffset  + 1) - pProcUtime->Offset(iOffset))
                    + (pProcStime->Offset(iOffset  + 1) - pProcStime->Offset(iOffset)));
            SysTmp = double((pSysUtime->Offset(iOffset  + 1) - pSysUtime->Offset(iOffset))
                     + (pSysNtime->Offset(iOffset + 1) - pSysNtime->Offset(iOffset))
                     + (pSysStime->Offset(iOffset + 1) - pSysStime->Offset(iOffset))
                     + (pSysItime->Offset(iOffset + 1) - pSysItime->Offset(iOffset))
                     + (pSysIotime->Offset(iOffset + 1) - pSysIotime->Offset(iOffset))
                     + (pSysIrtime->Offset(iOffset + 1) - pSysIrtime->Offset(iOffset))
                     + (pSysSttime->Offset(iOffset + 1) - pSysSttime->Offset(iOffset))
                     + (pSysGtime->Offset(iOffset + 1) - pSysGtime->Offset(iOffset))
                     + (pSysGntime->Offset(iOffset + 1) - pSysGntime->Offset(iOffset)));
            TmpList.push_back(ProcTmp/SysTmp*s_lNumProcs);
            iOffset++;

        }
    }
    return  TmpList;
}

list<double> DataApiProcMemList(int pid, long flag)
{ 

    list<double> TmpList;
    s_itIt = s_pProcsMap->find(pid);

    if(s_itIt == s_pProcsMap->end()) 
    {
        printf("test pid no find\n");
    }
    else 
    {
        int iOffset;
        int iSize;

        if (flag <  1)
        { 
            //不使用flag
            iOffset = 0;
        }
        else
        { 
            if (s_lFlag - flag < QUEUE_MAX_LEN)
            { 
                iOffset = QUEUE_MAX_LEN - 1  - s_lFlag + flag;
            }
            else
            { 
                //请求的flag已经是过期的
                //返回空的list
                return TmpList;

            }

        }


        const CSqQueue<long> *pProcRSS = s_itIt->second->Rss();

        iSize = pProcRSS->Size();
        while (iOffset < iSize)
        { 
            TmpList.push_back((double)(pProcRSS->Offset(iOffset))/(double)s_lNumPages);
            iOffset++;
        }
    }

    return TmpList;
}
//_______________________[END]________________________________________//





















