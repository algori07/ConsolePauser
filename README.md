# Console Pauser
## Introduce
A simple console program written in C have the same functionality as ConsolePauser.exe that come with Dev-C++. I made it for cross-platform purposes and usable in other projects. (just like the pauser of Dev-C++ but can be use in linux)
## How to use
Running other program and pause the console before it close. Useful for competitive programming.
Command line usage: ConsolePauser.exe [option] "<program> arguments ..."
Options:
  [-e/--exit] Exit on finished without press enter key to exit.
By default, you need to press enter key to exit.
  [-n/--no-timer] Do not display execution time.
By default, the execution time will be displayed after finished.
  [-t/--return-timer] Returns the execution time in milliseconds on exited.
By default, the return value is the one of the program.
  [-r/--return-return] Return the "return value" from the execution.
  [-s/--return-success] Always return success on exited.
  [-h/--help] Show help.
## Example
```cpp
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;
int main()
{
	vector<int> arr{1,2,4,3,5};
	sort(arr.begin(),arr.end());
	cout << "Sorted array: ";
	for(int &val : arr)
	{
		cout << val << " ";
	}
	return 0;
}

```
This is an example of sorting an array using std::sort. When run the process with ConsolePauser, it will show you some infomation before the terminal close.
```
Sorted array: 1 2 3 4 5
--------------------------------
Process exited with return value 0 after 0.045s.
Press enter to continue . . .
```
## How to build
- Building single file:
Just build main.c by whatever compiler.
- Using CMake:
+ Using Visual Studio generator (windows only):
`cmake -B build -G "Visual Studio 16" && cmake --build build --config Release`
(You can replace "16" by other number, cmake --help for more)
You will find ConsolePauser.exe in folder "build\Release"
+ Using other generator:
`cmake -B build -G <generator> -DCMAKE_BUILD_TYPE=Release && cmake --build build`
You will find ConsolePauser.exe in folder "build"
