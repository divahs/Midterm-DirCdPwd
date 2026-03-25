#include <windows.h>
#include <iostream>
#include <vector>
#include <string>

int wmain() {
    DWORD needed = GetCurrentDirectoryW(0, nullptr);
    if (needed == 0) {
        std::wcerr << L"Error: GetCurrentDirectoryW failed. Code: " << GetLastError() << std::endl;
        return 1;
    }

    std::vector<wchar_t> buffer(needed);
    DWORD result = GetCurrentDirectoryW(needed, buffer.data());
    if (result == 0) {
        std::wcerr << L"Error: GetCurrentDirectoryW failed. Code: " << GetLastError() << std::endl;
        return 1;
    }

    std::wcout << buffer.data() << std::endl;
    return 0;
}