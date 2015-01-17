/********************************************************
  @Author:   丁雪峰
  @File:     data.h
  @LastTime: 2013-08-07 18:29:35
  @Version:  1.0.1
  Description:
      数据接口.
      处于项目的中心位置.
      一方面调用后台的采集模块进行数据采集,另一方面为上层
      各种数据的访问提供接口.

      前端的模块不必与后台的采集信息模块直接交流,
      全部通过本模块进行访问.

********************************************************/
#ifndef DATA_API_
#define DATA_API_

#include <pthread.h>
#include <ctype.h>


#include <dirent.h>
#include <unistd.h>//for sleep
#include <stdlib.h>

#include "stdmacro.h"
#include "tools.h"
#include "SqQueue.h"
#include "proc_class.h"

#include <map>
#include <list>
#include <vector>
typedef map<int, CProcess *> MAP_PROCES;
typedef pair<int, CProcess *> PAIR_PROCES;
typedef pair<int, double > CPUPAIR;  
typedef pair<int, long > MEMPAIR;  



//数据接口的入口函数,在使用数据前必须启动数据接口
//这个函数主要是完成了数据采集线程的启动
int DataApiEnter();
//等数据采集线程的退出
static int DataApiWait();

//采集信息的入口函数,
//属性:线程函数
//任务:运行一个死循环
//     调用了进程采集信息方法与系统信息采集方法
static void *CollectInfo(void *p);

//^^^^^^^^^^^^^^^^^^^^^^^[START:采集进程信息]^^^^^^^^^^^^^^^^^^^//
//用于采集进程的信息                                        // 
//                                                          //
//生成list.这个list是由进程总集中记录的进程的pid组成,       // 
//在扫描系统进程信息时如果有这个进程pid,就删除这个list      //
//中的pid,这样list中最后留下的就是已经结束了的进程的pid     //
static int RefreshListKey();                                //
//扫描系统的进程信息,                                       //
//更新或插入新的进程信息                                    //
static int GetProcsInfo();                                  //
                                                            //
//分析有没有进程已经结束,删除已经结束的进程                 //
static int EraseKilledProc();                               //
//_______________________[END]______________________________//



//^^^^^^^^^^^^^^^^^^^^^^^[START:迭代器]^^^^^^^^^^^^^^^^^^^^//
const int DataApiIteratorInit();                           //
MAP_PROCES::iterator  DataApiIterator();                   //
bool DataApiIteratorNext();                                //
//_______________________[END]_____________________________//

//^^^^^^^^^^^^^^^^^^^^^^^^^[START:铺助函数]^^^^^^^^^^^^^^^^^^^^^^^^^^^//
//用于sort里的排序                                                    //
static int MemCmp(const MEMPAIR& x, const MEMPAIR& y);                //
                                                                      //
//返回一个pid组成的list这个                                           //
//按照mem从大到小排列                                                 //
static list<int> MemSort(MAP_PROCES * pProcMap);                      //
                                                                      //
//用于sort里的排序                                                    //
static int CpuCmp(const CPUPAIR& x, const CPUPAIR& y);                //
                                                                      //
//返回一个pid组成的list这个                                           //
//按照cpu从大小于排列                                                 //
static list<int> CpuSort(MAP_PROCES * pProcMap);                      //
                                                                      //
//__________________[end]_____________________________________________//

//^^^^^^^^^^^^^^^^^^^^^^^[START:接口函数]^^^^^^^^^^^^^^^^^^^^^^^^^^^^^//
//返回值:进程数目 int                                                 //
const int DataApiNumProc();                                           //
                                                                      //
//返回值:一个list对象的const 指针,                                    //
//这个对象用于进程信息的列表输出                                      //
//对于list对象可以进行过滤,排序等                                     //
//                                                                    //
//注意:通过s_iListMode来判断是返回哪种list对象                        //
list<int> * DataApiPidList();                                         //
                                                                      //
//作用:修改DataApiPidList()返回的list的对象的结果                     //
//0:返回的list中的pid按从小到大排序                                   //
//1:返回的list中的pid按mem从大到小排序                                //
//2:返回的list中的pid按cpu从大到小排序                                //
void DataApiListMode(int iMode);                                      //
                                                                      //
//返回值:进程名 string                                                //
//参数:  进程pid                                                      //
const char * DataApiProcName(int pid);                                //
                                                                      //
//返回值:进程名 string                                                //
//参数:  没有参数                                                     //
//注意:   这个接口没有参数,其返回的是一个隐式的pid的进程名            //
//        这个进程是刚刚被访问的过的一个进程.                         //
//        也就是说想隐式地使用这个函数,这个pid之前必须显式地被        //
//        访问过.                                                     //
//同类的函数:name ppid  Time mem  priority  nice  state  proccpu      //
//procruntime                                                         //
const char * DataApiProcName();                                       //
                                                                      //
//返回值:int ppid                                                     //
//参数:  进程pid                                                      //
const int  DataApiProcPpid(int pid);                                  //
                                                                      //
//属性:隐式访问函数,同上                                              //
const int  DataApiProcPpid();                                         //
                                                                      //
//返回值:指定pid的进程的内存以K为单位                                 //
//参数:pid                                                            //
//其他:这个内存是RSS,也就是Resident Set Size 实际使用物理内存         //
//     包含共享库占用的内存                                           //
const long DataApiProcMem(int pid);                                   //
                                                                      //
//返回值:指定pid的进程的内存以K为单位                                 //
//参数:pid                                                            //
//属性:隐式访问函数,同上                                              //
//其他:这个内存是RSS,也就是Resident Set Size 实际使用物理内存         //
//     包含共享库占用的内存                                           //
const long DataApiProcMem();                                          //
                                                                      //
//返回值:指定pid的进程的静态优先级                                    //
//参数:pid                                                            //
const long DataApiProcPriority(int pid);                              //
                                                                      //
//返回值:指定pid的进程的静态优先级                                    //
//参数:pid                                                            //
//属性:隐式访问函数,同上                                              //
const long DataApiProcPriority();                                     //
                                                                      //
//返回值:指定pid的进程的动态优先级                                    //
//参数:pid                                                            //
const long DataApiProcNice(int pid);                                  //
                                                                      //
//属性:隐式访问函数,同上                                              //
const long DataApiProcNice();                                         //
                                                                      //
//返回值:指定pid的进程的运行状态                                      //
const char DataApiProcState(int pid);                                 //
                                                                      //
//属性:隐式访问函数,同上                                              //
const char DataApiProcState();                                        //
                                                                      //
                                                                      //
//返回值:指定pid的进程的cpu使用百分率,返回值是一个double的小数        //
const double DataApiProcCpu(int pid);                                 //

//属性:隐式访问函数,同上                                              //
const double DataApiProcCpu();                                        //
                                                                      //
//返回值:指定pid的进程的运行时间,单位是秒                             //
const unsigned long DataApiProcRunTime(int pid);                      //
                                                                      //
//属性:隐式访问函数,同上                                              //
const unsigned long DataApiProcRunTime();                             //
                                                                      //
//返回值是进程相对于系统开始的偏移时间以秒为单位,                     //
//也就是多系统开始多少秒之后进程开始                                  //
const unsigned long long  DataApiProcStartTime(int pid);              //

//属性:隐式访问函数,同上                                              //
const unsigned long long  DataApiProcStartTime();                     //

//return:返回由cpu使用比率组成的list对象,数据类型是double
//flag是标志位,返回从flag开始的list
//   如果flag小于零,则不使用flag,返回的是内存中的所有的
//   有用的数据
//
//   如果flag的已经不存在于内存中返回的是空的list
//
//   如果pid不存在了,那么返回的是也是空的list
list<double> DataApiProcCpuList(int pid, long flag =  -1);


//return:返回由内存(RSS)使用比率组成的list对象,数据类型是double
//flag是标志位,返回从flag开始的list
//   如果flag小于零,则不使用flag,返回的是内存中的所有的
//   有用的数据
//
//   如果flag的已经不存在于内存中返回的是空的list
//
//   如果pid不存在了,那么返回的是也是空的list
list<double> DataApiProcMemList(int pid, long flag =  -1);

//_______________________[END]________________________________________//






#endif
