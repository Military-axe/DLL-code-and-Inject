// file_name:DLLSource.cpp
#include <windows.h>
#include <iostream>
using namespace std;

BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, void* lpReserved){ 
    HANDLE g_hModule; 
    switch(dwReason) 
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