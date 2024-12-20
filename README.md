# Console Pauser
## Introduce
A simple console program written in C have the same functionality as ConsolePauser.exe that come
with Dev-C++. I made it for cross-platform purposes and usable in other projects.
## How to use
Running other program and pause the console before it close. Useful for competitive programming.
```
Usage:
  ConsolePauser.exe [option] [-i <inputfile>] [-o <outputfile>] [--] <program> [arguments...]
Options:
  [--] Everything after -- will be <program> [arguments...]. Use for program
has name start with "-". By default, <program> is the first argument which
don't start with "-" and [arguments...] is the left over.
  [-i/-o <file>] Parse <file>'s content as stdin/stdout when starting process.
  [-e/--exit] Exit on finished without press enter key to exit.
By default, you need to press enter key to exit.
  [-n/--no-timer] Do not display execution time.
By default, the execution time will be displayed after finished.
  [-t/--return-timer] Returns the execution time in milliseconds on exited.
By default, the return value is the one of the program.
  [-r/--return-return] Return the "return value" from the execution.
  [-s/--return-success] Always return success on exited.
  [-h/--help] Show this.
```
## Example
```cpp
// gcc -o sorting.exe main.cpp
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;
int main()
{
	int n;
	cout << "n: ";
	cin >> n;
	vector<int> arr(n);
	cout << "arr: ";
	for(int &val : arr)
	{
		cin >> val;
	}
	sort(arr.begin(),arr.end());
	cout << "Sorted array: ";
	for(const int &val : arr)
	{
		cout << val << " ";
	}
	return 1234;
}

```
This is an example of sorting an array using std::sort. When execute command`ConsolePauser -- sorting.exe`,
it will show you some infomation before the terminal close.
```
n: 5
arr: 5 2 4 3 1
Sorted array: 1 2 3 4 5
--------------------------------
Process exited with return value 1234 after 0.045s.
Press enter to continue . . .
```
If you have a file named `sorting.inp`.
```
5
5 2 4 3 1
```
You can execute command `ConsolePauser -i sorting.inp -- sorting.exe`.
Now, all content of `sorting.inp` will be `stdin` of `sorting.exe`.
Read `[-i/-o]` options for more infomations.

## How to build
Just build main.c by whatever compiler supporting C99 standard.
If you use an old compiler, just wish. I haven't test too much with old compiler.

Example: `gcc -o consolepauser main.c` or `gcc -o ConsolePauser.exe main.c`

Or using CMake if you know how to build CMake project.
```
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```
The output executable is in `build/` or `build/Release` folder.

### Building with CMake (Windows, Linux, MacOS(maybe))
- Requirement: `gcc` or `clang`, `make` or `ninja`, `cmake`
- If you use other compiler, put `-DCMAKE_C_COMPILER=<path_to_compiler>` to the end of the first command
- below.

Open terminal in project path and build project using CMake by the following command:
```
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```
The output executable is in `build/` or `build/Release` folder.

### Building with Code::Blocks
Open `ConsolePauser.cbp` in `CodeBlocks` subfolder using Code::Blocks and build it.

### Building with Dev-C++ (Windows)
Open `ConsolePauser.dev` in `DevCpp` subfolder using Dev-C++ and build it.

### Building with VisualStudio (Windows)
Make sure you have installed `CMake C++ tools for Windows` with Visual Studio Installer
(auto install when choose Desktop development with C++)

Open `Developer Command Prompt for VS 2019` from `Start menu` and use CMake to build by the following command:
```batch
cd /D <project_path>
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Note that if you use other version of Visual Studio(example: `Visual Studio 2022`), the name of shortcut
in `Start menu` will have different number.
