#include "display.h"
#include "signal.h"

#include "data.h"
#include <list>
#include "unistd.h"
#include <sock_json.h>
int main(int argc,char *argv[])
{ 
    //启动后台接口
    DataApiEnter();

    if (argc > 1)
    { 
        if (strcmp(argv[1] ,"-w") == 0)
        { 
            SockJsonEnter();

        }
        if (strcmp(argv[1],"-c") == 0)
        { 
            printf("%s", argv[1]);
            CursesDisplayStart();

        }

    }
    else
    { 
        // CursesDisplayStart();

    }

    return 0;
}
//不允许<C-c>结束进程

//   signal(SIGINT, SIG_IGN);
