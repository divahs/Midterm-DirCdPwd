#include <windows.h>
#include <iostream>
#include <vector>
#include <string>

void PrintUsage() {
    std::wcout << L"Usage: mycd <path>\n";
    std::wcout << L"Examples:\n";
    std::wcout << L"  mycd C:\\Windows\n";
    std::wcout << L"  mycd ..\n";
    std::wcout << L"  mycd SomeSubdirectory\n";
}

std::wstring GetCurrentDirString() {
    DWORD needed = GetCurrentDirectoryW(0, nullptr);
    if (needed == 0) {
        return L"";
    }

    std::vector<wchar_t> buffer(needed);
    DWORD result = GetCurrentDirectoryW(needed, buffer.data());
    if (result == 0) {
        return L"";
    }

    return std::wstring(buffer.data());
}

int wmain(int argc, wchar_t* argv[]) {
    if (argc != 2) {
        PrintUsage();
        return 1;
    }

    std::wstring target = argv[1];

    if (!SetCurrentDirectoryW(target.c_str())) {
        std::wcerr << L"Error: SetCurrentDirectoryW failed for [" << target
            << L"]. Code: " << GetLastError() << std::endl;
        return 1;
    }

    std::wstring current = GetCurrentDirString();
    if (current.empty()) {
        std::wcerr << L"Error: changed directory, but GetCurrentDirectoryW failed.\n";
        return 1;
    }

    std::wcout << L"Current directory is now: " << current << std::endl;
    return 0;
}