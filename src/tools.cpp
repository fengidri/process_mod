#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include "tools.h"


//处理proc下的文件的方法
//跳过空格
char *g_SkipWs(const char *p)
{
    while (isspace(*p))
    { 
        p++;
    }
    return (char *)p;
}

//跳过字段
char *g_SkipToken(const char *p)
{
    while (isspace(*p))
    { 
        p++;
    }
    while (*p && !isspace(*p))
    { 
        p++;
    }
    return (char *)p;
}
//
char *g_NextToken(const char *p)
{
    while (*p && !isspace(*p))
    { 
        p++;
    }
    while (isspace(*p))
    { 
        p++;
    }
    return (char *)p;
}

