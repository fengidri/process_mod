#include <ncurses.h> 
#include <SqQueue.h>  
#include <data.h>
#include <stdmacro.h>
#include "display.h"
#include <pthread.h>
#include "draw_chart.h"

static WINDOW *s_TopWin;
static WINDOW *s_MainWin;
static WINDOW *s_MainSubPid;
static WINDOW *s_MainSubName;
static WINDOW *s_MainSubState;
static WINDOW *s_MainSubRss;
static WINDOW *s_MainSubCpu;
static WINDOW *s_MainSubPri;
static WINDOW *s_MainSubNi;
static WINDOW *s_MainSubStartTime;

static int (*s_pRefresh_f) ();
//static CDataApi s_DataApi;
//
//discription:光标所在行
static int s_iPos;

//显区域的最上行所在的在位置
static int s_iStartDisplay;

//注意:其变化范围是1-CURSES_DISPLAY_SUBMIN_HEIGHT
static int s_iFocusLine;
static int s_iFocusPid;
static list <int> *s_PidList;


int CursesDisplayStart()
{ 
    //初始化curses
    initscr();  
    noecho();

    /* 开启功能键响应模式 */
    keypad(stdscr, TRUE);

    //光标不显示
    curs_set(0);

    WaitForUser();
    
    //还原窗口
    endwin();

    return 0;
}

static int WaitForUser()
{ 
    char cInput;
    bool bWait;
    //mode 1:list
    //     2:focus
    int mode;

    bWait = TRUE;



    s_pRefresh_f = DisplayList;
    s_iPos = 0;
    s_iStartDisplay = 0;
    s_iFocusLine =  1;

    pthread_t pthread_id;

    //默认输出
    //线程,定时更新输出
    DisplayListWin();
    pthread_create(&pthread_id, NULL, Display, NULL);
    mode = DIS_MODE_LIST;


    //list按cpu排序
    DataApiListMode(DIS_LIST_CPU);

    //等待用户输入
    //并执行相应的动作
    while (bWait)
    { 
        cInput = wgetch(s_MainWin);//等待按键   

        switch (cInput)
        { 
            case ' ':
                { 
                    if (mode == DIS_MODE_LIST)
                    { 
                        //向下滚动
                    }
                    else
                    { 
                        break;
                    }
                }
            case 'j':
                { 
                    if (mode == DIS_MODE_LIST)
                    { 
                        //向下滚动
                        if (s_iFocusLine < CURSES_DISPLAY_SUBMIN_HEIGHT)
                        { 
                            s_iFocusLine++;
                            DisplayList();
                        }
                        else
                        { 
                            if (s_iStartDisplay <  DataApiNumProc()  - SCROLL_DOWN)
                            { 
                                s_iStartDisplay += SCROLL_DOWN;
                                DisplayList();
                            }
                        }
                        break;
                    }
                    else
                    { 
                        break;
                    }
                }
            case 'k':
                { 
                    if (mode == DIS_MODE_LIST)
                    {
                        if (s_iFocusLine > 1)
                        { 
                            s_iFocusLine--; 
                            DisplayList();
                        }
                        else
                        { 
                            if (s_iStartDisplay > 0)
                            { 
                                s_iStartDisplay -= SCROLL_DOWN;
                                DisplayList();
                            }
                        }
                        break;
                    }
                    else
                    { 
                        break;
                    }
                }
            case 'm':
                { 
                    if (mode == DIS_MODE_LIST)
                    {
                        DataApiListMode(DIS_LIST_MEM);
                        DisplayList();
                        break;
                    }
                    else
                    { 
                        break;
                    }
                }
            case 'c':
                { 
                    if (mode == DIS_MODE_LIST)
                    {
                        DataApiListMode(DIS_LIST_CPU);
                        DisplayList();
                        break;
                    }
                    else
                    { 
                        break;
                    }
                }
            case 'p':
                { 
                    if (mode == DIS_MODE_LIST)
                    {
                        DataApiListMode(DIS_LIST_PID);
                        DisplayList();
                        break;
                    }
                    else
                    { 
                        break;
                    }
                }
            case 'q'://退出
                { 
                    bWait = FALSE;
                    break;
                }
            case KEY_F(1)://F1
                { 

                }
            case '\n'://进入drawchart模式
                { 
                    if (mode ==  1)//focus
                    { 


                        //s_MainWin
                        //使用迭代的方法输出所有的进程信息
                        //

                        list<int>::iterator PidListIt;
                        PidListIt = s_PidList->begin();
                        advance(PidListIt, s_iStartDisplay  + s_iFocusLine  - 1);
                        s_iFocusPid = *(PidListIt);

                        FocusProcCpu();

                        s_pRefresh_f = FocusProcCpu;

                        mode = 2;
                    }
                    break;

                }
            case 'l':
                { 
                    if (mode ==  2)//focus
                    { 
                        s_pRefresh_f = DisplayList;
                        DisplayList();
                        mode = 1;


                    }
                    break;

                }
            default:
                { 

                }

        }
    }
    return 0;

}

//线程函数
static void *Display(void *)
{ 
    while (TRUE)
    {
        (*s_pRefresh_f)();
        sleep(DISPLAY_SLEEP_TIME);

    }
    return NULL;
}


static int FocusProcCpu()
{ 

    DrawChart(s_MainWin, DataApiProcCpuList(s_iFocusPid));
    mvwprintw(s_TopWin, 1, 1, "n of the proc is: %d; this proc is:%s", 
            DataApiNumProc(), DataApiProcName(s_iFocusPid));
    wrefresh(s_TopWin);
    return 0;

}
static int DisplayList()
{ 
    int iPid;
    int iDisLines;

    iDisLines = getmaxy(s_MainSubPid);


    list<int>::iterator PidListIt;

    unsigned long ulTime;

    wclear(s_MainWin);
    wclear(s_TopWin);

    //s_MainWin
    //使用迭代的方法输出所有的进程信息
    //
    s_PidList = DataApiPidList();

    PidListIt = s_PidList->begin();
    advance(PidListIt, s_iStartDisplay);

    for (int iPos = 0; iPos<iDisLines; iPos++)
    { 
        if (PidListIt == s_PidList->end())
        { 
            break;

        }
        iPid = *(PidListIt);

        ulTime = DataApiProcRunTime(iPid);
        //mvwprintw(s_MainWin, iPos, 0, "%s", DataApiProcName());

        mvwprintw(s_MainSubPid, iPos, 0, "%d", iPid);
        mvwprintw(s_MainSubName, iPos, 0, "%s", DataApiProcName());
        mvwprintw(s_MainSubState, iPos, 1, "%c", DataApiProcState());
        mvwprintw(s_MainSubRss, iPos, 0, "mem:%ldk", DataApiProcMem());
        mvwprintw(s_MainSubPri, iPos, 0, "PRI:%ld", DataApiProcPriority());
        mvwprintw(s_MainSubNi, iPos, 0, "NI:%ld", DataApiProcNice());
        mvwprintw(s_MainSubStartTime, iPos, 0, "TIME:%02lu:%02lu:%02lu", 
                ulTime/3600, 
                (ulTime - (ulTime/3600)*3600)/60,
                (ulTime - (ulTime/3600)*3600)%60
                );

        mvwprintw(s_MainSubCpu, iPos, 1, "CPU:%3.1f%" , DataApiProcCpu());
        PidListIt++;
    }

    if (s_iFocusLine - s_iStartDisplay < iDisLines)
    { 
        mvwchgat(s_MainWin, s_iFocusLine, 1,  -1, A_REVERSE, 1, NULL);
    }


    //s_TopWin
    //输出基本信息
    box(s_MainWin, ACS_VLINE, ACS_HLINE);
    box(s_TopWin, ACS_VLINE, ACS_HLINE);
    mvwprintw(s_TopWin, 1, 1, "n of the proc is: %d   Offset:%3d", 
            DataApiNumProc(), s_iStartDisplay
            );

    //refresh();
    wrefresh(s_TopWin);
    wrefresh(s_MainWin);

    return 0;
}
static void DisplayListWin()
{ 
    //分配窗口
    int iMainStartY = 3;
    int iMainStartX = 0;
    s_TopWin = newwin(CURSES_MAIN_OFFSET_Y, COLS, 0, 0);
    s_MainWin = newwin(CURSES_DISPLAY_MIN_HEIGHT , 
            COLS, iMainStartY, iMainStartX);
    s_MainSubPid = subwin(s_MainWin, 
            CURSES_DISPLAY_SUBMIN_HEIGHT,
            CURSES_DISPLAY_PROC_PID, 
            iMainStartY  + 1, 
            iMainStartX  + 1);
    s_MainSubName = subwin(s_MainWin, 
            CURSES_DISPLAY_SUBMIN_HEIGHT,
            CURSES_DISPLAY_PROC_NAME, 
            iMainStartY  + 1, 
            iMainStartX  + CURSES_DISPLAY_PROC_PID  + 2);
    s_MainSubState = subwin(s_MainWin, 
            CURSES_DISPLAY_SUBMIN_HEIGHT,
            CURSES_DISPLAY_PROC_STATE, 
            iMainStartY  + 1, 
            iMainStartX  + CURSES_DISPLAY_PROC_PID
            + CURSES_DISPLAY_PROC_NAME + 2
            );
    s_MainSubRss = subwin(s_MainWin, 
            CURSES_DISPLAY_SUBMIN_HEIGHT,
            CURSES_DISPLAY_PROC_RSS, 
            iMainStartY  + 1, 
            iMainStartX  + CURSES_DISPLAY_PROC_PID
            + CURSES_DISPLAY_PROC_NAME 
            + CURSES_DISPLAY_PROC_STATE 
            + 2
            );
    s_MainSubCpu = subwin(s_MainWin, 
            CURSES_DISPLAY_SUBMIN_HEIGHT,
            CURSES_DISPLAY_PROC_CPU, 
            iMainStartY  + 1, 
            iMainStartX  + CURSES_DISPLAY_PROC_PID
            + CURSES_DISPLAY_PROC_NAME 
            + CURSES_DISPLAY_PROC_STATE 
            + CURSES_DISPLAY_PROC_RSS
            + 2
            );
    s_MainSubPri = subwin(s_MainWin, 
            CURSES_DISPLAY_SUBMIN_HEIGHT,
            CURSES_DISPLAY_PROC_PRI, 
            iMainStartY  + 1, 
            iMainStartX  + CURSES_DISPLAY_PROC_PID
            + CURSES_DISPLAY_PROC_NAME 
            + CURSES_DISPLAY_PROC_STATE 
            + CURSES_DISPLAY_PROC_RSS
            + CURSES_DISPLAY_PROC_CPU
            + 2
            );
    s_MainSubNi = subwin(s_MainWin, 
            CURSES_DISPLAY_SUBMIN_HEIGHT,
            CURSES_DISPLAY_PROC_NI, 
            iMainStartY  + 1, 
            iMainStartX  + CURSES_DISPLAY_PROC_PID
            + CURSES_DISPLAY_PROC_NAME 
            + CURSES_DISPLAY_PROC_STATE 
            + CURSES_DISPLAY_PROC_RSS
            + CURSES_DISPLAY_PROC_CPU
            + CURSES_DISPLAY_PROC_PRI
            + 2
            );
    s_MainSubStartTime = subwin(s_MainWin, 
            CURSES_DISPLAY_SUBMIN_HEIGHT,
            CURSES_DISPLAY_PROC_STARTTIME, 
            iMainStartY  + 1, 
            iMainStartX  + CURSES_DISPLAY_PROC_PID
            + CURSES_DISPLAY_PROC_NAME 
            + CURSES_DISPLAY_PROC_STATE 
            + CURSES_DISPLAY_PROC_RSS
            + CURSES_DISPLAY_PROC_CPU
            + CURSES_DISPLAY_PROC_PRI
            + CURSES_DISPLAY_PROC_NI
            + 2
            );
    //为窗口画出边框
    box(s_MainWin, ACS_VLINE, ACS_HLINE);
    box(s_TopWin, ACS_VLINE, ACS_HLINE);
    mvwprintw(s_TopWin, 1, 1, "collecting......");

    //刷新
    refresh();
    wrefresh(s_TopWin);
    wrefresh(s_MainWin);

}
void JustRun()
{ 

}
