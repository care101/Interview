目录

> [vector](https://github.com/care101/Interview/blob/master/cpp/STL.md#vector)

>> [内存扩展（内存复用以及缓存友好）](https://github.com/care101/Interview/blob/master/cpp/STL.md#%E5%86%85%E5%AD%98%E6%89%A9%E5%B1%95%E5%86%85%E5%AD%98%E5%A4%8D%E7%94%A8%E4%BB%A5%E5%8F%8A%E7%BC%93%E5%AD%98%E5%8F%8B%E5%A5%BD)

> map

> set

# vector

## 内存扩展（内存复用以及缓存友好）

> 内存扩展为什么是2倍,不是固定值大小

总结：对比可以发现采用采用成倍方式扩容，可以保证常数的时间复杂度，而增加指定大小的容量只能达到O(n)的时间复杂度，因此，使用成倍的方式扩容。

如果以成倍方式增长。假定有 n 个元素,倍增因子为 m； 完成这 n 个元素往一个 vector 中的 push_back操作，需要重新分配内存的次数大约为 logm(n)； 第 i 次重新分配将会导致复制 m^(i) (也就是当前的vector.size() 大小)个旧空间中元素; n 次 push_back 操作所花费的时间复杂度为O(n)。均摊分析的方法可知，vector 中 push_back 操作的时间复杂度为常量时间.
如果一次增加固定值大小 。假定有 n 个元素,每次增加k个；第i次增加复制的数量为为：100i ；n 次 push_back 操作所花费的时间复杂度为O(n^2)，均摊下来每次push_back 操作的时间复杂度为O(n)；

> 最佳倍数是大于1且小于2间的任意数字

总结：如果以大于2倍的方式扩容，下一次申请的内存会大于之前分配内存的总和，导致之前分配的内存不能再被使用。所以，最好的增长因子在 （1,2）之间。

理想分配方案是是在第N次resize()的时候能复用之前N-1次释放的内存，但选择两倍的增长比如像这样：1，2，4，8，16，32，... 可以看到到第三次resize(4)的时候，前面释放的总和只有1+2=3，到第四次resize(8)的时候前面释放的总和只有1+2+4=7，每次需要申请的空间都无法用到前面释放的内存。这样对cache和MMU都不够友好。
k = 1.5 在几次扩展之后，可以重用之前的内存空间。

> vector内存空间浪费（reserve）
```C++
resize();//强行把 vector 的元素个数扩展或者是减少到所指定的元素。
reserve();//把容器的容量置为所指定的元素。
```
Vector在capacity不够的时候会增长内部空间，是为了保证后续capacity-size个insert的O(1)插入复杂度，但还要考虑可能产生的堆空间浪费，所以增长倍数不能太大，假设vector按两倍空间增长，你要插入17个元素，到16个的时候capacity变为32，那么有15个元素的空间被浪费，当然你也可以用提前调用reserve(17)来解决，可是遇到无法得知insert数量的场景这个问题仍然存在

1、它必须重新分配适当的新的内存空间。

2、把原来的数据copy过到新的空间，再插入一个元素。

3、销毁原来的数据。

这是一个非常耗时的过程。然而这个问题是怎么解决，不至于有这么多好耗时的动作呢？ reserve（）成员函数。只是在 vector 在分配内存的机制上面，预先保留，就可以减少：内存重新分配;内存“数据搬移”;销毁原来的数据三个动作。
```C++
const  int  kSize = 10;
vector<int > iv;
iv.reserve(kSize);
for (int i = 0; i < kSize; ++i)  
  iv.push_back(i);
```
> vector容器间的拷贝（stl::copy和模板偏特化）
```C++
#include <iostream>
#include <algorithm>
#include <vector>
 
using namespace std;
 
int main () 
{
	int myints[] = {10, 20, 30, 40, 50, 60, 70};
	vector<int> myvector;
	vector<int>::iterator it;
	
	myvector.resize(7);   // 为容器myvector分配空间
	
	//copy用法一：
	//将数组myints中的七个元素复制到myvector容器中
	copy ( myints, myints+7, myvector.begin() );
	
	cout << "myvector contains: ";
	for ( it = myvector.begin();  it != myvector.end();  ++it )
	{
		cout << " " << *it;
	}
	cout << endl;
 
	//copy用法二:
	//将数组myints中的元素向左移动一位
	copy(myints + 1, myints + 7, myints);
 
	cout << "myints contains: ";
	for ( size_t i = 0; i < 7; ++i )
	{
		cout << " " << myints[i];
	}
	cout << endl;
 
	return 0;
```



