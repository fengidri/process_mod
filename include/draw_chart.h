/*
   
   @     Author:丁雪峰
   @       File:draw_chart.h
   @   LastTime:2013-08-07 18:40:03
   @    Version:1.0.4
  */

#include <list>
#include <curses.h>
using namespace std;
//作用:依据于list的值,绘制直方图
//
//参数:一个窗口指针,一个double类型的list对象
//
//这个list中的值是0-1之间的小数,表示的是每一个峰的调试占
//全部调试的比例
//
//默认输出的图表与窗口边缘的上下左右间距是1
int DrawChart(WINDOW *Win, list<double> List);
