#include <windows.h>
#include <aclapi.h>
#include <sddl.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#pragma comment(lib, "Advapi32.lib")

struct Options {
    bool showAll = false;      // /a
    bool recursive = false;    // /s
    bool showOwner = false;    // /q
    std::wstring path = L".";
};

std::wstring FileTimeToString(const FILETIME& ft) {
    if (ft.dwLowDateTime == 0 && ft.dwHighDateTime == 0) {
        return L"N/A";
    }

    FILETIME localFt;
    SYSTEMTIME st;
    if (!FileTimeToLocalFileTime(&ft, &localFt)) {
        return L"N/A";
    }
    if (!FileTimeToSystemTime(&localFt, &st)) {
        return L"N/A";
    }

    wchar_t buffer[64];
    swprintf_s(buffer, L"%02d/%02d/%04d %02d:%02d",
        st.wMonth, st.wDay, st.wYear, st.wHour, st.wMinute);
    return std::wstring(buffer);
}

std::wstring AttributesToString(DWORD attrs) {
    std::wstring s;
    s += (attrs & FILE_ATTRIBUTE_DIRECTORY) ? L'D' : L'-';
    s += (attrs & FILE_ATTRIBUTE_READONLY) ? L'R' : L'-';
    s += (attrs & FILE_ATTRIBUTE_HIDDEN) ? L'H' : L'-';
    s += (attrs & FILE_ATTRIBUTE_SYSTEM) ? L'S' : L'-';
    s += (attrs & FILE_ATTRIBUTE_ARCHIVE) ? L'A' : L'-';
    return s;
}

unsigned long long FileSizeFromFindData(const WIN32_FIND_DATAW& fd) {
    ULARGE_INTEGER size;
    size.HighPart = fd.nFileSizeHigh;
    size.LowPart = fd.nFileSizeLow;
    return size.QuadPart;
}

std::wstring GetOwnerString(const std::wstring& fullPath) {
    PSID ownerSid = nullptr;
    PSECURITY_DESCRIPTOR securityDescriptor = nullptr;

    DWORD result = GetNamedSecurityInfoW(
        fullPath.c_str(),
        SE_FILE_OBJECT,
        OWNER_SECURITY_INFORMATION,
        &ownerSid,
        nullptr,
        nullptr,
        nullptr,
        &securityDescriptor
    );

    if (result != ERROR_SUCCESS || ownerSid == nullptr) {
        if (securityDescriptor) {
            LocalFree(securityDescriptor);
        }
        return L"<unknown>";
    }

    DWORD nameSize = 0;
    DWORD domainSize = 0;
    SID_NAME_USE sidType;

    LookupAccountSidW(nullptr, ownerSid, nullptr, &nameSize, nullptr, &domainSize, &sidType);

    std::vector<wchar_t> name(nameSize);
    std::vector<wchar_t> domain(domainSize);

    if (!LookupAccountSidW(nullptr, ownerSid, name.data(), &nameSize, domain.data(), &domainSize, &sidType)) {
        LocalFree(securityDescriptor);
        return L"<unknown>";
    }

    std::wstring owner;
    if (!domain.empty() && domain[0] != L'\0') {
        owner = std::wstring(domain.data()) + L"\\" + std::wstring(name.data());
    }
    else {
        owner = std::wstring(name.data());
    }

    LocalFree(securityDescriptor);
    return owner;
}

bool ShouldSkipEntry(const WIN32_FIND_DATAW& fd, bool showAll) {
    std::wstring name = fd.cFileName;

    if (name == L"." || name == L"..") {
        return true;
    }

    if (!showAll) {
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) {
            return true;
        }
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) {
            return true;
        }
    }

    return false;
}

void PrintEntry(const std::wstring& parentPath, const WIN32_FIND_DATAW& fd, bool showOwner) {
    std::wstring fullPath = parentPath;
    if (!fullPath.empty() && fullPath.back() != L'\\') {
        fullPath += L'\\';
    }
    fullPath += fd.cFileName;

    std::wstring timeStr = FileTimeToString(fd.ftCreationTime);
    std::wstring attrStr = AttributesToString(fd.dwFileAttributes);
    unsigned long long size = FileSizeFromFindData(fd);

    std::wcout << std::left
        << std::setw(18) << timeStr
        << std::setw(8) << attrStr;

    if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        std::wcout << std::setw(14) << L"<DIR>";
    }
    else {
        std::wcout << std::setw(14) << size;
    }

    if (showOwner) {
        std::wstring owner = GetOwnerString(fullPath);
        std::wcout << std::setw(30) << owner;
    }

    std::wcout << fd.cFileName << std::endl;
}

void ListDirectory(const std::wstring& path, const Options& opts) {
    std::wstring searchPath = path;
    if (!searchPath.empty() && searchPath.back() != L'\\') {
        searchPath += L'\\';
    }
    searchPath += L"*";

    WIN32_FIND_DATAW findData;
    HANDLE hFind = FindFirstFileExW(
        searchPath.c_str(),
        FindExInfoBasic,
        &findData,
        FindExSearchNameMatch,
        nullptr,
        0
    );

    if (hFind == INVALID_HANDLE_VALUE) {
        std::wcerr << L"Error: cannot open directory [" << path
            << L"]. Code: " << GetLastError() << std::endl;
        return;
    }

    std::wcout << L"\n Directory of " << path << L"\n\n";
    std::wcout << std::left
        << std::setw(18) << L"Created"
        << std::setw(8) << L"Attrs"
        << std::setw(14) << L"Size";

    if (opts.showOwner) {
        std::wcout << std::setw(30) << L"Owner";
    }

    std::wcout << L"Name\n";
    std::wcout << L"--------------------------------------------------------------------------\n";

    std::vector<std::wstring> subdirs;

    do {
        if (ShouldSkipEntry(findData, opts.showAll)) {
            continue;
        }

        PrintEntry(path, findData, opts.showOwner);

        if (opts.recursive && (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            std::wstring subdir = path;
            if (!subdir.empty() && subdir.back() != L'\\') {
                subdir += L'\\';
            }
            subdir += findData.cFileName;
            subdirs.push_back(subdir);
        }

    } while (FindNextFileW(hFind, &findData));

    FindClose(hFind);

    if (opts.recursive) {
        for (const auto& subdir : subdirs) {
            ListDirectory(subdir, opts);
        }
    }
}

Options ParseArgs(int argc, wchar_t* argv[]) {
    Options opts;

    for (int i = 1; i < argc; ++i) {
        std::wstring arg = argv[i];

        if (arg == L"/a" || arg == L"/A") {
            opts.showAll = true;
        }
        else if (arg == L"/s" || arg == L"/S") {
            opts.recursive = true;
        }
        else if (arg == L"/q" || arg == L"/Q") {
            opts.showOwner = true;
        }
        else {
            opts.path = arg;
        }
    }

    return opts;
}

int wmain(int argc, wchar_t* argv[]) {
    Options opts = ParseArgs(argc, argv);
    ListDirectory(opts.path, opts);
    return 0;
}