/********************************************************
  @Author:   丁雪峰
  @File:     tools.h
  @LastTime: 2013-08-07 18:35:12
  @Version:  1.0.4
********************************************************/


#ifndef _TOOLS_H
#define _TOOLS_H


//处理proc下的文件的方法
extern char *g_SkipWs(const char *p);
extern char *g_SkipToken(const char *p);
extern char *g_NextToken(const char *p);

#endif
