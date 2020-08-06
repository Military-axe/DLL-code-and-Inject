# DLL-code-and-Inject
learn DLL code and inject

文章可能图片加载不出，可以看我blog:https://mi1itray_axe.gitee.io/2020/08/06/DLL%E7%BC%96%E5%86%99%E5%92%8C%E6%B3%A8%E5%85%A5/#more

<a name="237Mw"></a>
## Tools
用vscode 编辑代码<br />cl.exe 编译器和link.exe链接器可以从vs路径下找到 <br />`../Microsoft Visual Studio/2019/Community/VC/Tools/MSVC/14.25.28610/bin/Hostx64/x64`<br />32位的就在32文件夹中找<br />我直接整个路径加入`path`

但是编译过程很不顺利，由于不在vs中编辑，有头文件按path中路径去mingw中找，然后找不到<br />没查到应该在vs中那个目录<br />
<br />~~不想用vs是想加深一下编译链接的过程，仔细想想，也就是cl和link改点参数的事，还是用vs吧🐶~~<br />~~vs2019~~<br />我用没装mingw的windows再操作了一下，无需按上面的操作加入路径，vs安装会安装**x64/x86 Native Tools Command Prompt for VS 2019，**用这个工具就可以，具体用法下面写
<a name="703d6307"></a>
## 过程 
<a name="X94qf"></a>
### 用vs2019生成dll
直接创建一个DLL项目，最简单的就直接编译就vans了(忽然想到，我直接把头文件和源文件拿出来用cl和link走一遍不就仿佛自己没用vs了吗🐶)<br />这个有两个头文件和两个cpp，我不是很清楚主文件外其他文件存在的意义，效率？规范？<br />我稍微改一下dllmai.cpp
```cpp
#include "pch.h"
#include <iosteram>
using namespace std;

BOOL APIENTRY DLLMain(HMODULE hMoudle,DWORD ul_reason_for_call,LPVOID lpReserved)
{
    cout<<"helloworld"<<endl;
    return TRUE;
}
```
一看就是老新手了<br />这就是试一下，后面所有的过程都不是用vs的，用vs能不能走下面的流程我不敢说
<a name="0u2w3"></a>
### 非vs生成dll
这会稍微麻烦一些，但是可以直接写在一个cpp文件中，就不用vs那样，还要pch之类的文件<br />MyDll.cpp
```cpp
// file_name:DLLSource.cpp
#include <windows.h>
#include <iostream>
using namespace std;

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, void* lpReserved){ 
    HANDLE g_hModule; 
    switch(ul_reason_for_call) 
    { 
    case DLL_PROCESS_ATTACH: 
        cout<<"Dll is attached!"<<endl; 
        g_hModule = (HINSTANCE)hModule; 
        break; 
    case DLL_PROCESS_DETACH: 
        cout<<"Dll is detached!"<<endl; 
        g_hModule=NULL; 
        break; 
    } 
    return true; 
}
```
三个参数，名字不重要，意思分别是

- hModule表示本dll的实例句柄
- ul_reason_for_call是一个说明动态库被调原因的标志
- 最后一个参数是一个保留参数

DLL_PROCESS_ATTACH就是dll被进程加载，DLL_PROCESS_DETACH就是从进程中卸载<br />类似的还有DLL_THREAD_ATTACH被线程被加载……<br />代码意思很简单，不废话了<br />**编译一下**<br />我编译成64位，所以打开**x64 Native Tools Command Prompt for VS 2019**<br />走到MyDll.cpp所造路径下`cl /c MyDll.cpp`,/c是只编译不链接的意思，cl其他的参数可以参考[MSVC编译选项](https://docs.microsoft.com/zh-cn/cpp/build/reference/compiler-options?view=vs-2019)<br />得到MyDll.obj,然后再`Link /dll MyDll.obj`,得到可执行文件<br />![image.png](https://cdn.nlark.com/yuque/0/2020/png/2212593/1596466660060-452dece6-89a6-4ab7-8564-1ff3a4aaee63.png#align=left&display=inline&height=174&margin=%5Bobject%20Object%5D&name=image.png&originHeight=348&originWidth=2109&size=85386&status=done&style=none&width=1054.5)
<a name="Zfkkk"></a>
## 调用dll
简单调用一下手动链接出来的那个dll，写一个testdll.cpp
```cpp
#include <windows.h>
#include <iostream>

int main(void){ 
    //加载我们的dll 
    HINSTANCE hinst=::LoadLibrary("MyDll.dll"); 
    if (NULL != hinst) 
    { 
        std::cout<<"dll loaded!"<<std::endl; 
    } 
    return 0; 
}
```
编译直接`cl Testdll.cpp`就完了，编译出obj之后会自动调用link链接成exe文件<br />需要windows.h头文件，用`::LoadLibrary("MyDll.dll"`加载dll<br />![image.png](https://cdn.nlark.com/yuque/0/2020/png/2212593/1596509618981-d4ea47b5-ea4e-44d6-9287-c6f1056087a3.png#align=left&display=inline&height=80&margin=%5Bobject%20Object%5D&name=image.png&originHeight=160&originWidth=613&size=27809&status=done&style=none&width=306.5)
<a name="eE8j7"></a>
## DLL中函数声明
一般使用的方式是使用`__declspec(dllexport)`在源码中定义dll可被外部使用的函数<br />但是由于c++会重新修饰名称，俗称名称粉碎机，简单试一下<br />dll代码加3行
```cpp
#include <windows.h>
#include <iostream>
__declspec(dllexport) void FuncFromDll (void);

void FuncFromDll(){
    std::cout<<"helloworld"<<std::endl;
}

BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, void* lpReserved){ 
    HANDLE g_hModule; 
    switch(dwReason) 
    { 
    case DLL_PROCESS_ATTACH: 
        std::cout<<"Dll is attached!"<<std::endl; 
        g_hModule = (HINSTANCE)hModule; 
        break; 
    case DLL_PROCESS_DETACH: 
        std::cout<<"Dll is detached!"<<std::endl; 
        g_hModule=NULL; 
        break; 
    } 
    return true; 
}
```
编译成MyDll2.dll,用dumpbin命令可以查看dll定义的外部函数用`dumpbin -exports MyDll2.dll`<br />![image.png](https://cdn.nlark.com/yuque/0/2020/png/2212593/1596513293812-51dd4bd7-06e4-46f2-854e-7aa012d3eeb7.png#align=left&display=inline&height=438&margin=%5Bobject%20Object%5D&name=image.png&originHeight=876&originWidth=811&size=69220&status=done&style=none&width=405.5)<br />很好看到那个函数，但是名字从`FuncFromDLL`变成 `?FuncFromDll@@YAXXZ`,这个名称修饰api原理没公布，ctf里也出过类似的题目。<br />觉得这个名字不好等下cpp文件中调用，有个常用的方法<br />把`__declspec(dllexport) void FuncFromDll (void);`前面加上`extern "C"`<br />变成`extern "C" __declspec(dllexport) void FuncFromDll (void);`这样再编译一下<br />这个意思是和编译器说，我这个是c代码，不是c++，别给👴改名称<br />编译完在dumpbin看一下<br />`  1    0 00001120 FuncFromDll`<br />很完美<br />

<a name="oAYnL"></a>
## 调用dll函数
首先`::LoadLibrary("MyDll.dll")`加载dll，然后用`GetProcAddress`函数导入dll中的函数
```cpp
#include <windows.h>
#include <iostream>

int main(void){ 
    typedef void(* DLLFUNC)(void);
    DLLFUNC GetDllfunc= NULL;
    HINSTANCE hinst=::LoadLibrary("MyDll2.dll"); 
    if (NULL != hinst) 
    { 
        std::cout<<"dll loaded!"<<std::endl; 
    } 
    GetDllfunc = (DLLFUNC)GetProcAddress(hinst, "FuncFromDll");
    if(GetDllfunc!=NULL){
         (*GetDllfunc)();
    } 
    return 0; 
}
```
结果<br />![image.png](https://cdn.nlark.com/yuque/0/2020/png/2212593/1596522107909-e91b7522-8d39-4b35-942e-7b269ac0608e.png#align=left&display=inline&height=84&margin=%5Bobject%20Object%5D&name=image.png&originHeight=167&originWidth=530&size=11951&status=done&style=none&width=265)<br />很ok，很nice<br />这种调用dll函数的方法叫显示调用，需要`::LoadLibrary`函数和`GetProcAddress`函数<br />还有一种隐式调用，意思类似，就是不用这两个函数，而是生成dll之后要在客户端cpp中包含dll头文件和导入lib文件，难度不大，想用查一下就行。可以参考这篇blog[windows下编写dll](https://www.cnblogs.com/weiyinfu/p/11235273.html)<br />

<a name="CXawK"></a>
## DLL注入
从上面的可以看到，dll的使用都是代码中使用的，那这就很没意思了，我肯定是要别人的程序运行我的dll，之后我可以dll中写入hook代码甚至反弹shell，这时候就要dll注入了（当然这个功能本意是给程序后期不修改源码情况下增加功能，比如软件更新，补丁等）<br />
<br />**dll注入的方法很多**

- 修改注册表来注入dll
- 使用`CreatRemoteThread`函数对运行中的进程注入dll
- 使用`SetwindowsHookEx`对应用程序hook来注入dll
- 替换成应用程序一定会使用的dll
- 把dll当成调试器注入
- 使用`CreatProcess`对子进程注入
- 修改被导入进程的导入表

这些不同方法涉及的知识面很广很广啊<br />有些方法就不是很靠谱，比如修改注册表和替换同名dll，我问了问凯哥，他说主要还是用远程线程注入，那我就学这个，其他的可以参考这篇文章[windows系统的dll注入](https://www.cnblogs.com/wf751620780/p/10730013.html)
<a name="pazlf"></a>
### `CreatRemoteThread`注入dll
详细原理可以在上面的文章中看到，代码也是嫖人家的，然后改一下<br />
<br />思路是利用CreatRemoteThread函数给目标进程附加一个进程，然后在线程中加载dll<br />线程中加载dll还需要目标进程中存在我们dll的路径，最保险的做法是<br />用VirtualAllocEx函数在目标进程中开辟一块内存，然后用WriteProcessMemory向我们开辟的内存写入路径。<br />加载dll还是LoadLibrary函数，目标进程肯定一开始就加载了kernal32.dll(kernal64.dll)，可以用GetProcAddress函数找到LoadLibrary函数。
```cpp
#include <windows.h>
#include <iostream>
#include <TlHelp32.h>
#include <io.h>
#include <tchar.h>
using namespace std;


void InjectDLL(DWORD pid, LPCTSTR dllpath){
    HANDLE                  hProcess = NULL;//保存目标进程的句柄
    LPVOID                  pRemoteBuf = NULL;//目标进程开辟的内存的起始地址
    DWORD                   dwBufSize = (DWORD)(_tcslen(dllpath) + 1) * sizeof(TCHAR);//开辟的内存的大小
    LPTHREAD_START_ROUTINE  pThreadProc = NULL;//loadLibreayW函数的起始地址
    HMODULE                 hMod = NULL;//kernel32.dll模块的句柄
    BOOL                    bRet = FALSE;
    
    //打开目标进程，获得句柄
    hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    //开辟一块内存
    pRemoteBuf = VirtualAllocEx(hProcess, NULL, dwBufSize,MEM_COMMIT, PAGE_READWRITE);
    //向内存中复制dll路径
    WriteProcessMemory(hProcess, pRemoteBuf,(LPVOID)dllpath, dwBufSize, NULL);
    //获取kernel32.dll
    hMod = GetModuleHandle("kernel32.dll");
    //从kernel32.dll中获取loadLibraryW的地址
    pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(hMod, "LoadLibraryA");
    //远程线程
    CreateRemoteThread(hProcess, NULL, 0, pThreadProc, pRemoteBuf, 0, NULL);
}


int main(){
    InjectDLL(6666,"C:\\Users\\axe\\Documents\\C\\MyDll.dll");
    return 0;
}
```
这个东西主要让我头痛的是类型转化，索性我全用ansi，不用unicode了，unicode。。。我再慢慢摸索摸索<br />写个程序试一下，目标程序object.exe<br />object.cpp,一个输入等注入
```cpp
#include <iostream>
using namespace std;

int main(){
    char wait_for_inject[10]={NULL};
    cout<<"Inject Me bro"<<endl;
    cin>>wait_for_inject;
}
```
编译运行后，任务管理器找一下pid<br />![image.png](https://cdn.nlark.com/yuque/0/2020/png/2212593/1596690901715-cd5bbc4c-8372-45ea-a857-9e257ea9f367.png#align=left&display=inline&height=400&margin=%5Bobject%20Object%5D&name=image.png&originHeight=799&originWidth=1164&size=183406&status=done&style=none&width=582)<br />修改一下注入代码的pid，编译运行<br />![image.png](https://cdn.nlark.com/yuque/0/2020/png/2212593/1596690956138-3795fb98-c418-4df4-b487-3d39c01b01c2.png#align=left&display=inline&height=93&margin=%5Bobject%20Object%5D&name=image.png&originHeight=185&originWidth=792&size=19264&status=done&style=none&width=396)<br />good job！
<a name="ZFwpX"></a>
### 伸手党的胜利
这个东西有的四现成的，52pojie去找找吧，但是对不同程序，注入可能要稍微变变？最好自己能懂一点原理
<a name="jjTkr"></a>
## 参考链接
[windows下编写dll](https://www.cnblogs.com/weiyinfu/p/11235273.html)<br />[windows系统的dll注入](https://www.cnblogs.com/wf751620780/p/10730013.html)<br />
