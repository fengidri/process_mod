#include <draw_chart.h>
int DrawChart(WINDOW *Win, list<double> List)
{ 
    int iStartY;
    int iStartX;
    int iMaxY;
    int iMaxX;
    int iHeight;
    int iWidth;
    int iLen;

    int iLeftEdig;
    int iRightEdig;
    int iTopEdig;
    int iBotEdig;

    iLeftEdig = 1;
    iRightEdig = 1;
    iTopEdig = 1;
    iBotEdig  = 1;

    double iHei;
    getbegyx(Win, iStartY, iStartX);
    getmaxyx(Win, iMaxY, iMaxX);
    iHeight = iMaxY - iTopEdig - iBotEdig;
    iWidth = iMaxX - iLeftEdig - iRightEdig;

    iLen = (unsigned int)iWidth < List.size() ? iWidth : List.size();


    //draw
    wclear(Win);
    box(Win, ACS_VLINE, ACS_HLINE);
    for (int i = 0; i < iLen; i++)
    { 
        iHei = List.back() * iHeight;
        List.pop_back();
        for (int j = 0; j < iHei; j++)
        { 
            mvwprintw(Win, 
                    iHeight - j, //Y
                    iWidth - i, //X
                    "|");

        }
    }
    wrefresh(Win);
    return 0;
}
