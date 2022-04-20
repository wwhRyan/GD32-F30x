# Athena GD32

### 功能需求

* **idle thread**
  * Laser NTC detect
  * LCOS I2C message get
* **AT decode thread**
  * debug handle cmd decode
  * AT handle decode
  * usart receive
* **data thread**
  * storage dirty data
  * Periodicity check mem data
* **LED interrupt**
  * change the current, Sectional current setting

### 技术栈（Technology stack used）

1. STM32 flash to EEPROM

   * 参考STM官网flash作为EEPROM的源码
   * 提供的EEPROM仿真固件的实现从内部运行闪存，因此在需要闪存的操作期间，对闪存的访问将暂停擦除或编程（EEPROM初始化、变量更新或页面擦除）。作为一个结果，应用程序代码未执行，中断无法服务。对于许多应用程序来说，这种行为可能是可以接受的，但是对于具有实时约束的应用程序，您需要从内部RAM运行关键进程。
   * 在这种情况下：1.在内部RAM中重新定位矢量表。2.从内部RAM执行所有关键代码和中断服务例程。这个编译器提供一个关键字，将函数声明为RAM函数；功能是与任何初始化变量一样，在系统启动时从闪存复制到RAM。它是重要的是要注意，对于RAM函数，所有使用的变量和调用的函数应该在内存中。
   * Flash的读写会关闭中断，对应整个系统的实时性有影响。决定不用了。👎
2. data manager

   * 参考V56&PWC878数据管理的函数,这个不用搞了。👎
3. AT protocol 👍

   * 参考S3AT库
4. uart N buffer received

   * 使用多重缓冲，防止丢包，注意堆栈的大小，防止溢出了
5. IDU change current

   * 参考对应的IDU工程
6. FreeRTOS 👍

   * 移植
7.

### 日志

1. 移植FreeRTOS的过程
   * 主要工作是修改对应的portable文件夹和freertos config头文件移植
   * 遇到编译不成功的问题大部分都是freertos config文件没有弄好的问题
     * 移植的时候，把FreeRTOS的config头文件各个功能选项都看一遍，基本就Ok了。不会搞出很多的幺蛾子。
   * 遇到freertos 运行到一半，遇到systick handle 就硬件错误了
     * 这个网上说要提前初始化任务函数？真的假的？这么不靠谱吗？
     * 就是运行卡在了：configASSERT( ucMaxPriorityValue == (configKERNEL_INTERRUPT_PRIORITY & ucMaxPriorityValue ) );说这个systick中断需要设置在最低的优先级。我猜应该是我的systick提前打开了，且优先级不是最低。后面发现是FreeRTOS的config头文件没有包含`#include "gd32f30x.h"`
     * 初始化任务函数的问题是因为定时器中断的进入没有判断RTOS是否有初始化，结果导致空指针的访问，导致硬件错误。这个在官方例程里居然没有，就离谱。
     * 发现禁止sys_tick中断这么难搞的吗？对应cortex m4 的disable的接口没有效果
2. GD32接口函数的开发
   * 一开始一直在找GD32的库函数开发手册，但是都没有，找了很多，发现例程库里面自带了很多的example，直接对比这些example就行了。
   * 移植Keil工程时，发现版本对不上，各种问题，最后还是用例程的工程。直接修改工程文本。GUI的配置难搞。
   * 留了一个meta_f30x的代码分支，之后可以用这个分支弄其他的30x项目，master还是搞Athena项目。
3. 寄存器抽象，对每个寄存器的各个位域进行抽象。
   * 可以优化的地方，将每个模块的寄存器进行更加一个层次的抽象。（需要用到typeof GCC工具链）。将整块寄存器的地址连续起来，方便连续发送和取地址。

### **业务TODO：**

* [ ] 建立与电源板的IIC控制联系。
* [ ] 完善IO的中断接收功能。
* [ ] 解决画面闪烁的问题。
* [ ] 红色、绿色的测试图不能显示
* [ ] 读取LCOS面板温度
* [ ]
