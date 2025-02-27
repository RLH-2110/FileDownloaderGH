#include "Windows.h"
#include "downloader.h"
#include "int.h"
#include <stdio.h>

BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,  /* handle to DLL module */
    DWORD fdwReason,     /* reason for calling function */
    LPVOID lpvReserved )  /* reserved */
{
	return TRUE;
}


__declspec(dllexport) void __cdecl downloader_init_d(void){
	downloader_init();
}

__declspec(dllexport) void __cdecl donwloader_cleanup_d(void){
	donwloader_cleanup();
}

__declspec(dllexport) unsigned char* __cdecl download_file_d(char* url, int32* DNS_LIST, uint32* out_fileSize, FILE* log){
	return download_file(url,DNS_LIST,out_fileSize,log);
}

__declspec(dllexport) void __cdecl free_d(void* ptr){ /* for c#, since I need to free the memory of download_file_d*/
	free(ptr);
}