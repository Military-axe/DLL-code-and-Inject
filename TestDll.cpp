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