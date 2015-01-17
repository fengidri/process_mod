# linux系统监控软件

## 设计要求

* 监控设置的程序的CPU
* 监控设置的程序存储情况
* 监控设置的程序的内存使用情况
* 可以对设置的程序进行进程管理
* 可以对设置的程序查看相应的信息，启动，停止
* 提供cursor与web接口

## 设计
核心部分是对于系统进程的信息的收集，还有就是对于进程进行管理。

### 信息采集
为了实现信息的收集使用linux 的proc文件系统来完成对于系统与进程的信息的收集。
在对于cpu，memory等进行分析时，往往要分析一段时间里的数据，而不是一个点上的数据。
使用图表分析cpu，memory在一段时间里的走趋是很有必要的。
这就要求在采集信息时对于这些数据要放到内存里保存起来。
 
### 进程管理
还要求可以对于进程进行管理，这些管理常见的是启动、停止。
启动、停止可以分别使用linux的fork（）、与kill（）函数来实现。

### Ui设计
在gui设计上考虑以tui（文件终端界面）为主，

### B/S，C/S
Linux做为服务器，那么就有可能是在机房里，用户不能本地运行查看本软件。
那么使用C/S，B/S的方式就很有必要了。我认为使用B/C的方式更加的方便利于使用，
在任意一台机器上都可以查看，很方便。
如果是C/S，那么用户在使用时还要安装客户端，但是本软件并没有特别的信息传递，
没有使用客户端的必要。

## 数据结构
### 进程数据结构

![struct](doc/struct.png)

### 数据队列
是用于保存每个系统参数（如内存）的数据的池。故每一个数据都要记录在一段时间内的很多参数，才能让用户知道系统参数的变化情况。这里的数据队列是一个循环队列，并且当队列满的时候如果再加入数据，队首自动出队，第二个数据成为队首，原队首成为队尾。数据队列可以不断的加入新的数据，而不会溢出，同时数据队列只保留固定数量的数据。

![quene](doc/quene.png)


### Tui本地输出
“本地输出”是与采集的两个模块是线程关系，使用线程主要是考虑到线程之间的通信比进程之间的通信要方便很多，可以直接提取数据并输出，并且不要花费资源进行数据的拷贝。
本地输出模块设计上主要是使用文本终端，这与linux的环境相适应，也适用于没有图形界面的系统。将使用Linux终端图形库 Curses 进行UI设计。

### 数据接口/Web
数据接口主要是对外，使用socket编程。使用http协议实现一个小型的web服务器，可以处理各种请求。
web使用ajax进行轮询，请求json信息，并使用javascript进行数据分析，一方面生成进程列表，另一方面可以使用html canvas生成拆线图。
在从服务器传递数据到客户端的过程中，是使用网络进行的进程间的通信，并不是线程之间的通信，线程之间的通信，十分方便并且是对于源数据的访问，
不会有多余的资源的浪费。但是由于网络的不及时，以及网络信息传递的特殊性，在网络信息传递的时候，一个简单的方法是把数据池里的数据都传递过去，这个方法每一次都要处理并传递大量的数据。

外部的UI主要是完成本地的文本终端的设计。同时提供了TCP的网络接口，使用http与客户机进行通信。

### 内部接口
多个进程之间使用线程间的通信方式，进程的数据以一个MAP\_PROCES* procs_map指针作为接口。

##Linux系统接口
命令行/函数接口API
命令行的命令是用于交互的，并不合适于编程，在C++中调用命令比较不方便，并且对于系统的依赖比较大，因为那么命令大多是可以选择安装与否的。
Linux下还有一些API可以访问系统目前的状态，这是一个可行的方式。但是相对于下面要说到的proc，还有以下一些不足：
Kernel的版本很多，由于一些原因，各个大的版本都有一些场合在使用。Kernel的版本不同在API上就有一些不同，这会给编程带来不便。另一个原因就是相对于proc而言，API提供的信息不够集中，相对分散，要得到不同的信息要调用多个接口。
综上所述，这两个方法理论上都是可行的，但是在编译上都不如proc文件系统来得方便。通过对于top的源代码的分析可知，top也是使用这种方法对于系统信息进行分析的。

###Proc文件系统
proc 文件系统是一种内核和内核模块用来向进程 (process) 发送信息的机制 (所以叫做 /proc)。这个伪文件系统让你可以和内核内部数据结构进行交互，获取有关进程的有用信息，在运行中 (on the fly) 改变设置 (通过改变内核参数)。 与其他文件系统不同，/proc 存在于内存之中而不是硬盘上。/proc 的文件可以用于访问有关内核的状态、计算机的属性、正在运行的进程的状态等信息。
下面是一些重要的记载了系统或进程信息的文件：

* /proc/[pid]/stat中记录是进程pid的一些信息包括：pid， comm，state, ppid......
* /proc/[pid]/statm 中记录进程内存的信息
* /proc/cpuinfo记录系统的物理cpu信息
* /proc/meminfo记录系统的物理内存信息
* /proc/apm 高级电源管理（APM）版本信息及电池相关状态信息，通常由apm命令使用
* /proc/buddyinfo用于诊断内存碎片问题的相关信息文件
* /proc/cmdline在启动时传递至内核的相关参数信息，这些信息通常由lilo或grub等启动管理工具进行传递
* /proc/cpuinfo处理器的相关信息的文件
* /proc/crypto系统上已安装的内核使用的密码算法及每个算法的详细信息列表
* /proc/devices系统已经加载的所有块设备和字符设备的信息，包含主设备号和设备组（与主设备号对应的设备类型）名
* /proc/diskstats每块磁盘设备的磁盘I/O统计信息列表；（内核2.5.69以后的版本支持此功能）
* /proc/dma每个正在使用且注册的ISA DMA通道的信息列表
* /proc/execdomains内核当前支持的执行域（每种操作系统独特“个性”）信息列表
* /proc/fb帧缓冲设备列表文件，包含帧缓冲设备的设备号和相关驱动信息
* /proc/filesystems当前被内核支持的文件系统类型列表文件，被标示为nodev的文件系统表示不需要块设备的支持；通常mount一个设备时，如果没有指定文件系统类型将通过此文件来决定其所需文件系统的类型
