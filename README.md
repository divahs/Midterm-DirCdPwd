# Midterm-DirCdPwd
# Midterm: Reimplementation of dir, cd, and pwd in C++

## Overview

This project reimplements selected functionality of the Windows `dir`, `cd`, and `pwd` commands using C++ and Win32 APIs. The assignment required a working reimplementation of the assigned tools, compilation into executable binaries, and defensive analysis using monitoring tools.

I was assigned the `dir / cd / pwd` group.

## Tools Included

This project contains the following executables:

- `mydir.exe`
- `mycd.exe`
- `mypwd.exe`

## Features

### mydir

`mydir.exe` emulates selected functionality of the Windows `dir` command.

Supported features:

- lists the contents of the current directory or a specified path
- displays file creation time
- displays file attributes
- displays whether an entry is a directory
- displays file size in bytes
- supports `/a` to show hidden and system files
- supports `/s` to recursively traverse subdirectories
- supports `/q` to display file ownership information

### mycd

`mycd.exe` emulates selected functionality of the Windows `cd` command.

Supported features:

- changes to a full path
- changes to a child directory using a relative path
- changes to the parent directory using `..`
- prints the resulting current directory using `GetCurrentDirectoryW`

**Important note:** A standalone executable cannot permanently change the working directory of the parent shell. Because of that, `mycd.exe` changes its own process working directory and then prints the resulting path before exiting.

### mypwd

`mypwd.exe` prints the current working directory using `GetCurrentDirectoryW`.

## Win32 APIs Used

This project uses the following Win32 APIs:

- `GetCurrentDirectoryW`
- `SetCurrentDirectoryW`
- `FindFirstFileExW`
- `FindNextFileW`
- `GetNamedSecurityInfoW`
- `LookupAccountSidW`

Requirements

To build and run this project, the following are required:

Windows 10 or Windows 11
Visual Studio 2022 Community
Desktop development with C++ workload installed

Build Instructions
Open Midterm-DirCdPwd.sln in Visual Studio.
In the toolbar, set the build configuration to Release.
In the toolbar, set the platform to x64.
Click Build > Build Solution.
After a successful build, the compiled binaries will be located in the x64\Release\ folder.

How to Run the Programs

Open Command Prompt and change into the folder that contains the built executables. Example:
cd C:\Users\vboxuser\source\repos\Midterm-DirCdPwd\x64\Release

Then run the tools as shown below.

mypwd

Prints the current working directory. : mypwd.exe
Example Output: 

C:\Users\vboxuser\source\repos\Midterm-DirCdPwd\x64\Release

mycd

Changes the current directory for the mycd.exe process and then prints the resulting path.
mycd.exe C:\Windows
mycd.exe ..
mycd.exe System32

Example Output:
Current directory is now: C:\Windows

mydir

Lists files and directories in the current directory or a specified path.
mydir.exe
mydir.exe /a
mydir.exe /s
mydir.exe /q
mydir.exe C:\Windows /a /s /q

Example output:

Directory of .

Created           Attrs   Size          Name
--------------------------------------------------------------------------
03/25/2026 18:33  ----A   17408         mycd.exe
03/25/2026 18:33  ----A   27136         mydir.exe
03/25/2026 18:33  ----A   14848         mypwd.exe

Directory of /q

Created           Attrs   Size          Owner                         Name
--------------------------------------------------------------------------
03/25/2026 18:33  ----A   17408         WINDOWS\vboxuser              mycd.exe
03/25/2026 18:33  ----A   27136         WINDOWS\vboxuser              mydir.exe
03/25/2026 18:33  ----A   14848         WINDOWS\vboxuser              mypwd.exe

After testing the files myself I concluded that

mypwd.exe prints the current directory
mycd.exe handles full paths, parent directory traversal, and relative child directory traversal
mydir.exe lists files and directories and supports the required /a, /s, and /q switches

Author

Janna A.
