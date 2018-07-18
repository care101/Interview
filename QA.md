# 测IO

## iostat 查看磁盘IO情况

### 安装
```
yum install sysstat -y
```
### 语法
```
-d    表示显示设备（磁盘）使用状态
-x    表示显示和io相关的扩展数据
-k    表示某些使用block为单位的列强制使用Kilobytes为单位
-c    查看cpu信息
```
### 使用
```
#iostat -d -k 1 10         #查看TPS和吞吐量信息
```
```
tps：该设备每秒的传输次数（Indicate the number of transfers per second that were issued to the device.）
"一次传输"意思是"一次I/O请求"。多个逻辑请求可能会被合并为"一次I/O请求"。"一次传输"请求的大小是未知的。
kB_read/s：每秒从设备（drive expressed）读取的数据量；
kB_wrtn/s：每秒向设备（drive expressed）写入的数据量；
kB_read：读取的总数据量；
kB_wrtn：写入的总数量：这些单位都为Kilobytes，即KB
```
```
#iostat -d -x -k 1 10      #查看设备使用率（%util）、响应时间（await）
```
以上可以看到，磁盘的平均响应时间较小，为0，磁盘使用率平均为3.0左右，比较小。
```
#iostat -c 1 10            #查看cpu状态
```
```
iostat -x -k  
```
如果%util接近100%,说明产生的I/O请求太多,I/O系统已经满负载,该磁盘可能存在瓶颈。
```
iostat -d sda 2
```
监控指定设备

在使用LINUX时经常会遇到系统IO占用很高，系统IO占用可以使用iostat -x 1查看。

linux系统中，管理员可以使用top来监控进程的cpu和内存的使用情况，但是对于磁盘的i/o则只能使用iostat笼统地进行监视，不能对应进程这对于管理员来说很不方便。比如你的服务器负载很低，内存使用也很少，但是硬盘狂转，但你就是无法准确确定是哪个进程在搞鬼。

## iotop 磁盘IO 定位负载来源进程

iotop 是一个用来监视磁盘 I/O 使用状况的 top 类工具。但很多时候知道磁盘IO负载高，但并不知道是什么程序占用的，是PHP，还是MYSQL,还是其它的，这就不好查看了。iotop工具可以实现

iotop 可以清楚地知晓是什么程序在读写磁盘，速度以及命令行，pid 等信息。

iotop可以显示磁盘读写的速率，交换分区进出情况和整体磁盘性能状况，这些都是按照每个进程使用情况来统计。进程列表按照I/O使用状态排序并每秒刷新一次。

### 语法
```
-p 指定进程ID，显示该进程的IO情况
-u 指定用户名，显示该用户所有的进程IO情况
```
## vmstat 测硬盘IO

vmstat命令的含义为显示虚拟内存状态（“Viryual Memor Statics”），但是它可以报告关于进程、内存、I/O等系统整体运行状态
### 使用方法
```
vmstat [-V] [-n] [delay [count]]
 -V 显示vmstat的版本；
 -n causes the headers not to be reprinted regularly.
 -a 显示所有激活和未激活内存的状态；print inactive/active page stats.
 -d 显示硬盘统计信息；prints disk statistics
 -D 显示硬盘分区表；prints disk table
 -p 显示硬盘分区读写状态等；prints disk partition statistics
 -s 显示内存使用情况；prints vm table
 -m prints slabinfo
 -S 定义单位，k K
 delay 是两次刷新时间间隔；
 单位体积： k:1000 K:1024 m:1000000 M:1048576 (默认是 K)
 count 刷新次数；
```
### 显示参数
```
r  :The number of processes waiting for run time. 等待运行时间的进程数，即等待的进程数、
b  :The number of processes in uninterruptible sleep. 在等待io的进程数
swpd: 虚拟内存的使用量
free：空闲内存量
buff：缓冲区中的内存
cache：被用来做为高速缓存的内存数
si: 从磁盘交换到内存的交换页数量，单位：KB/秒。
so: 从内存交换到磁盘的交换页数量，单位：KB/秒。
bi: 发送到块设备的块数，单位：块/秒。
bo: 从块设备接收到的块数，单位：块/秒。
in: 每秒的中断数，包括时钟中断。
cs: 每秒的环境（上下文）转换次数。
us：用户进程使用的时间 。以百分比表示。
sy：系统进程使用的时间。 以百分比表示。
id：中央处理器的空闲时间 。以百分比表示。
wa：io等待时间
st：Time stolen from a virtual machine
```
## pt-ioprofile 定位负载来源文件 数据库运维

## iftop 测网络IO

### 安装

- 第一步：安装EPEL源    yum install epel-release

- 第二部：安装iftop     yum install iftop
### 使用

- 1、iftop界面相关说明

界面上面显示的是类似刻度尺的刻度范围，为显示流量图形的长条作标尺用的。

中间的<= =>这两个左右箭头，表示的是流量的方向。

TX：发送流量

RX：接收流量

TOTAL：总流量

Cumm：运行iftop到目前时间的总流量

peak：流量峰值

rates：分别表示过去 2s 10s 40s 的平均流量

- 2、iftop相关参数

- 常用的参数

-i设定监测的网卡，如：# iftop -i eth1

-B 以bytes为单位显示流量(默认是bits)，如：# iftop -B

-n使host信息默认直接都显示IP，如：# iftop -n

-N使端口信息默认直接都显示端口号，如: # iftop -N

-F显示特定网段的进出流量，如# iftop -F 10.10.1.0/24或# iftop -F 10.10.1.0/255.255.255.0

-h（display this message），帮助，显示参数信息

-p使用这个参数后，中间的列表显示的本地主机信息，出现了本机以外的IP信息;

-b使流量图形条默认就显示;

-f这个暂时还不太会用，过滤计算包用的;

-P使host信息及端口信息默认就都显示;

-m设置界面最上边的刻度的最大值，刻度分五个大段显示，例：# iftop -m 100M

- 进入iftop画面后的一些操作命令(注意大小写)

按h切换是否显示帮助;

按n切换显示本机的IP或主机名;

按s切换是否显示本机的host信息;

按d切换是否显示远端目标主机的host信息;

按t切换显示格式为2行/1行/只显示发送流量/只显示接收流量;

按N切换显示端口号或端口服务名称;

按S切换是否显示本机的端口信息;

按D切换是否显示远端目标主机的端口信息;

按p切换是否显示端口信息;

按P切换暂停/继续显示;

按b切换是否显示平均流量图形条;

按B切换计算2秒或10秒或40秒内的平均流量;

按T切换是否显示每个连接的总流量;

按l打开屏幕过滤功能，输入要过滤的字符，比如ip,按回车后，屏幕就只显示这个IP相关的流量信息;

按L切换显示画面上边的刻度;刻度不同，流量图形条会有变化;

按j或按k可以向上或向下滚动屏幕显示的连接记录;

按1或2或3可以根据右侧显示的三列流量数据进行排序;

按<根据左边的本机名或IP排序;

按>根据远端目标主机的主机名或IP排序;

按o切换是否固定只显示当前的连接;

按f可以编辑过滤代码，这是翻译过来的说法，我还没用过这个！

按!可以使用shell命令，这个没用过！没搞明白啥命令在这好用呢！

按q退出监控。


