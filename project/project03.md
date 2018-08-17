### 干什么用/实现什么功能

大数据时代，海量数据的运算成为挑战。我的项目能够加快图算法的收敛速度，同时降低系统缓存缺失和IO开销。其中，图数据是一种用点和边来表示事物之间依赖关系的数据。图处理系统已经应用到电商、金融管理的反欺诈等很多领域。

淘宝电商：

应用1：刷单检测：小时级别完成几百点规模biclique模式识别

应用2：凑单推荐计算：宝贝相似性图上，50毫秒完成6度增量随机游走

应用3：欺诈交易检测：动态交易图，秒级动态环检测

平安科技：

应用1：医疗档案相似性筛查（医疗档案图上，秒级完成6度增量随机行走）

应用2：转卖药品，超范围购药囤积药品等（医保药品报销图上，小时级别完成几百点相似性识别）

### 结构

1、基于图结构和运行中图顶点状态度的变化动态增量图划分

2、为图划分块自适应调度合适的处理方法
### 技术

多进程多线程项目，进程之间的消息通信用的是mmap共享内存，好处是利用内存缓冲区直接交换信息，无须复制，快捷、信息量大；缺陷是进程之间的读写操作的同步问题。对此，我采用MPI_Barrier函数，用于一个通信子中所有进程的同步。同时，我还用了std::thread和omp来实现多线程。同一进程中的多个线程共享相同的地址空间，可以访问进程中的大部分数据。但是线程可能存在安全和数据一致性问题，我用了mutex加锁和atomic无锁操作来解决。

# 单机多核处理器项目

目录

> [多进程](https://github.com/care101/Interview/new/master#%E5%A4%9A%E8%BF%9B%E7%A8%8B)

>> [创建多进程](https://github.com/care101/Interview/new/master#%E5%88%9B%E5%BB%BA%E5%A4%9A%E8%BF%9B%E7%A8%8B)

>> [mmap共享内存](https://github.com/care101/Interview/new/master#mmap%E5%85%B1%E4%BA%AB%E5%86%85%E5%AD%98)

>> [全局同步](https://github.com/care101/Interview/blob/master/project03.md#%E5%85%A8%E5%B1%80%E5%90%8C%E6%AD%A5)

> [多线程](https://github.com/care101/Interview/blob/master/project03.md#%E5%A4%9A%E7%BA%BF%E7%A8%8B)

>>[std::thread和omp](https://github.com/care101/Interview/blob/master/project/project03.md#stdthread%E5%92%8Comp)

>> [线程同步互斥（mutex，barrier，atomic）](https://github.com/care101/Interview/blob/master/project03.md#%E7%BA%BF%E7%A8%8B%E5%90%8C%E6%AD%A5%E4%BA%92%E6%96%A5mutexbarrieratomic)

> [NUMA](https://github.com/care101/Interview/blob/master/project03.md#numa)

# 多进程

## 创建多进程

```C++
MPI_Init_thread(argc, argv, MPI_THREAD_MULTIPLE, &provided);//为MPI添加PThread多线程支持
MPI_Comm_rank(MPI_COMM_WORLD, &partition_id);
MPI_Comm_size(MPI_COMM_WORLD, &partitions);
```
## mmap共享内存
```C++
// 把文件映射到内存，返回内存首地址
char * array = (char *)mmap(NULL, sizeof(T) * vertices, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
assert(array!=NULL);
```

在说mmap之前我们先说一下普通的读写文件的原理，进程调用read或是write后会陷入内核，因为这两个函数都是系统调用，进入系统调用后，内核开始读写文件，假设内核在读取文件，内核首先把文件读入自己的内核空间，读完之后进程在内核回归用户态，内核把读入内核内存的数据再copy进入进程的用户态内存空间。实际上我们同一份文件内容相当于读了两次，先读入内核空间，再从内核空间读入用户空间。
　	
Linux提供了内存映射函数mmap, 它把文件内容映射到一段内存上(准确说是虚拟内存上), 通过对这段内存的读取和修改, 实现对文件的读取和修改,mmap()系统调用使得进程之间可以通过映射一个普通的文件实现共享内存。普通文件映射到进程地址空间后，进程可以向访问内存的方式对文件进行访问，不需要其他系统调用(read,write)去操作。

### 特点：

（1）进程相关的

（2）与XSI共享内存一样，需要与同步原语一起使用

（3）只能是有共同祖先的进程才能使用

### 共享内存缺点

共享内存针对消息缓冲的缺点改而利用内存缓冲区直接交换信息，无须复制，快捷、信息量大是其优点。但是共享内存的通信方式是通过将共享的内存缓冲区直接附加到进程的虚拟地址空间中来实现的．因此，这些进程之间的读写操作的同步问题操作系统无法实现。必须由各进程利用其他同步工具解决。另外，由于内存实体存在于计算机系统中．所以只能由处于同一个计算机系统中的诸进程共享。不方便网络通信。

### 系统调用mmap()用于共享内存的方式：

（1）使用普通文件提供的内存映像：适用于任何进程之间。此时，需要打开或创建一个文件，然后再调用mmap()

典型调用代码如下：
```C++
fd=open(name, flag, mode); if(fd<0) ...
ptr=mmap(NULL, len , PROT_READ|PROT_WRITE, MAP_SHARED , fd , 0);
```
（2）使用特殊文件提供匿名内存映像：适用于具有亲缘关系的进程之间。由于父子进程特殊的亲缘关系，在父进程中先调用mmap()，然后调用fork()。那么在调用fork()之后，子进程继承父进程匿名映射后的地址空间，同样也继承mmap()返回的地址，这样，父子进程就可以通过映射区域进行通信了。一般来说，子进程单独维护从父进程继承下来的一些变量。而mmap()返回的地址，却由父子进程共同维护。对于具有亲缘关系的进程实现共享内存最好的方式应该是采用匿名内存映射的方式。 

### 与shm比较

1、mmap是在磁盘上建立一个文件，每个进程地址空间中开辟出一块空间进行映射。而对于shm而言，shm每个进程最终会映射到同一块物理内存。shm保存在物理内存，这样读写的速度要比磁盘要快，但是存储量不是特别大。

2、相对于shm来说，mmap更加简单，调用更加方便，所以这也是大家都喜欢用的原因。

3、另外mmap有一个好处是当机器重启，因为mmap把文件保存在磁盘上，这个文件还保存了操作系统同步的映像，所以mmap不会丢失，但是shmget就会丢失。

## 全局同步

MPI_Barrier函数 用于一个通信子中所有进程的同步，调用函数时进程将处于等待状态，直到通信子中所有进程 都调用了该函数后才继续执行。

```C++
 MPI_Barrier(MPI_COMM_WORLD);
```

# 多线程

## std::thread和omp

对于C++而言，当我们需要使用多线程时，可以使用boost::thread库或者自从C++ 11开始支持的std::thread，也可以使用操作系统相关的线程API，如在Linux上，可以使用pthread库。除此之外，还可以使用omp来使用多线程。它的好处是跨平台，使用简单。

在Linux平台上，如果需要使用omp，只需在编译时使用"-fopenmp"指令。在Windows的visual studio开发环境中，开启omp支持的步骤为“项目属性 -> C/C++ -> 所有选项 -> openmp支持 -> 是(/openmp)”。

## 线程同步互斥（mutex，barrier，atomic）

线程同步不用互斥锁，代价太大。锁1次是临界区的20倍甚至50倍以上的时间。线程同步可以用volatile变量、interlocked系列函数、SRW读写锁(vista系统及以上)、临界区。消耗的时间从小到大。这些都只能用于线程同步，不能用于进程同步。当然线程同步也用到计时器、信号量、时间和等待函数。

### mutex

在访问共享资源前对互斥量进行加锁，访问后释放。
```C++
std::mutex recv_queue_mutex;//对recv_queue的锁
```
```C++
recv_queue_mutex.lock();
recv_queue_size += 1;
recv_queue_mutex.unlock();
```
可能导致死锁：线程试图对同一个互斥量加锁两次，或者需要控制加锁顺序。解决方案：
```C++
timed_mutex
try_lock_for();
```
如果已经占有某些锁哦，且返回成功，就可以继续线程；如果不能获取锁，可以先释放已经占有的锁，过段时间再试。

### gcc 原子操作 无锁同步

啥叫无锁同步？ 所谓锁，就是指linux原来提供的那些锁，包括mutex等东西，如果线程无法获得锁，就会进行任务调度CAS compare and swap，不使用系统提供的锁，而是直接利用cpu提供的指令，实现互斥操作。在写入新值之前，读出旧值，当且仅当旧值与存储中的当前值一致时，才把新值写入存储。当无法获得锁时，不需要进行任务调度，从而减轻了任务切换而引起的系统开销。__sync_bool_compare_and_swap是可供程序员调用的接口

stdlib.h中，可以使用bool __sync_bool_compare_and_swap (type *ptr, type oldval, type newval, ...)和type __sync_val_compare_and_swap (type *ptr, type oldval, type newval, ...)两个函数

### barrier（thread.join）

允许任意数量的线程等待，知道所有线程完成处理，而线程不需要退出。所有线程到达barrier后可以继续工作。
```C++
std::thread recv_thread_dst();
```
```C++
recv_thread_dst.join();//执行这个.join之后上面的recv_thread_dst才会执行
```

### volatile

C/C++ 中的 volatile 关键字和 const 对应，用来修饰变量，通常用于建立语言级别的 memory barrier。
```C++
volatile int send_queue_size = 0;
```
遇到这个关键字声明的变量，编译器对访问该变量的代码就不再进行优化，从而可以提供对特殊地址的稳定访问。声明时语法：int volatile vInt; 当要求使用 volatile 声明的变量的值的时候，系统总是重新从它所在的内存读取数据，即使它前面的指令刚刚从该处读取过数据。而且读取的数据立刻被保存。

volatile 指出 i 是随时可能发生变化的，每次使用它的时候必须从 i的地址中读取，因而编译器生成的汇编代码会重新从i的地址读取数据放在 b 中。而优化做法是，由于编译器发现两次从 i读数据的代码之间的代码没有对 i 进行过操作，它会自动把上次读的数据放在 b 中。而不是重新从 i 里面读。这样以来，如果 i是一个寄存器变量或者表示一个端口数据就容易出错，所以说 volatile 可以保证对特殊地址的稳定访问。

多线程下有些变量是用volatile关键字声明的。当两个线程都要用到某一个变量且该变量的值会被改变时，应该用volatile声明，该关键字的作用是防止优化编译器把变量从内存装入CPU寄存器中。如果变量被装入寄存器，那么两个线程有可能一个使用内存中的变量，一个使用寄存器中的变量，这会造成程序的错误执行。volatile的意思是让编译器每次操作该变量时一定要从内存中真正取出，而不是使用已经存在寄存器中的值

因为在多核CPU中，每个CPU都有自己的缓存。缓存中存有一部分内存中的数据，CPU要对内存读取与存储的时候都会先去操作缓存，而不会直接对内存进行操作。所以多个CPU“看到”的内存中的数据是不一样的，这个叫做内存可见性问题（memory visibility）。并发编程下，一个程序可以有多个线程在不同的CPU核中同时运行，这个时候内存可见性就会影响程序的正确性。放到例子中就是，Thread 2修改了m_flag对应的内存，但是Thread 1在其他CPU核上运行，而两个CPU缓存和内存没有做同步，导致Thread 1运行的核上看到的一直都是旧的数据，于是Thread 1永远都不能醒来。内存可见性问题不是多线程环境下会遇到的唯一的问题，CPU的乱序执行也会导致一些意想不到的事情发生，关于这点volatile能做的也是有限的。这些都是属于并发编程的内容，在此我就不多做展开，总之volatile关键字对并发编程基本是没有帮助的。

那么用不了volatile，我们该怎么修改上面的例子？C++11开始有一个很好用的库，那就是atomic类模板，在<atomic>头文件中，多个线程对atomic对象进行访问是安全的。

### stl::atomic 无锁

在C++11之后，可以使用stl中atomic类。std::atomic对int, char, bool等数据结构进行原子性封装，在多线程环境中，对std::atomic对象的访问不会造成竞争-冒险。利用std::atomic可实现数据结构的无锁设计。

```C++
template< class T > bool atomic_compare_exchange_weak( std::atomic<T>* obj,T* expected, T desired ); 
template< class T > bool atomic_compare_exchange_weak( volatile std::atomic<T>* obj,T* expected, T desired );
```
只要把“volatile bool”替换为“atomic<bool>”就可以。<atomic>头文件也定义了若干常用的别名，例如“atomic<bool>”就可以替换为“atomic_bool”。atomic模板重载了常用的运算符，所以atomic<bool>使用起来和普通的bool变量差别不大。
```C++
volatile bool m_flag;
atomic<bool> m_flag;
```
# NUMA
```C++
numa_tonode_memory(array + sizeof(T) * local_partition_offset[s_i], sizeof(T) * (local_partition_offset[s_i+1] -local_partition_offset[s_i]), s_i);
numa_free(array, sizeof(T) * vertices);
```
    
