#include <iostream>
#include <windows.h>
#include <tlhelp32.h>



int main() {
    const char* WindowTitle = "Windows Title";

    HWND hwnd = FindWindowA(0, WindowTitle);
    DWORD pid;
    HANDLE handle;

    if (hwnd == NULL) {
        std::cerr << WindowTitle << ". NOT found" << std::endl;
        return 1;
    }

    GetWindowThreadProcessId(hwnd, &pid);

    handle = OpenProcess(PROCESS_ALL_ACCESS, NULL, pid);

    if (handle == NULL) {
        std::cerr << "Unable to open process. Error code: " << GetLastError() << std::endl;
        return 1;
    }

    std::cout << "hwnd  : " << hwnd << std::endl;
    std::cout << "pid   : " << pid << std::endl;
    std::cout << "handle: " << handle << std::endl;

    MEMORY_BASIC_INFORMATION memoryInfo;
    uintptr_t address = 0;
    const int targetValue = 84;

    std::cout << "target value: " << targetValue << std::endl;

    int values_found = 0;

    while (VirtualQueryEx(handle, (LPVOID)address, &memoryInfo, sizeof(memoryInfo))) {
        if (memoryInfo.State == MEM_COMMIT && (memoryInfo.Type == MEM_MAPPED || memoryInfo.Type == MEM_PRIVATE)) {
            int* buffer = new int[memoryInfo.RegionSize / sizeof(int)];
            SIZE_T bytesRead;

            if (ReadProcessMemory(handle, memoryInfo.BaseAddress, buffer, memoryInfo.RegionSize, &bytesRead)) {
                for (size_t i = 0; i < bytesRead / sizeof(int); ++i) {
                    if (buffer[i] == targetValue) {
                        std::cout << "At address: 0x" << std::hex << (uintptr_t)memoryInfo.BaseAddress + i * sizeof(int) << std::endl;
                        values_found++;
                    }
                }
            }

            delete[] buffer;
        }

        address += memoryInfo.RegionSize;
    }

    CloseHandle(handle);

    std::cout << "values found: " << values_found << std::endl;

    return 0;
}
