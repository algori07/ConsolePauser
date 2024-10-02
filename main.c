#if defined(_WIN32)
  #define ISWINDOWS
  #define _CRT_SECURE_NO_WARNINGS
  #include <wchar.h>
  #include <wctype.h> // for iswspace()
  #include <windows.h> // for CreateProcess()
  #include <time.h> // for clock() and CLOCKS_PER_SEC
#elif defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__))
  #define ISUNIX
  #define ENDLINE "\n"
  #include <string.h>
  #include <unistd.h>
  #include <time.h> // for clock_gettime() (only available on posix)
  #include <sys/wait.h>
#else
  #define ENDLINE "\n"
#endif
// use \r\n for windows and \n for unix

#include <stdio.h> // io
#include <stdlib.h> // for memory allocation
//#include <string.h> // for parsing argument
#include <stdbool.h> // bool
//#include <ctype.h> // isspace()

#if defined(ISWINDOWS)
#define ENDLINE L"\r\n"
typedef wchar_t char_t;
#define char_c(ch) L##ch // can be use for wide string too
#define strcmp_c(s1,s2) wcscmp(s1,s2)
#define strcat_c(s1,s2) wcscat(s1,s2)
#define strlen_c(s) wcslen(s)
#define strcpy_c(s1,s2) wcscpy(s1,s2)
//#define printf_c(fmt,...) wprintf(L##fmt,##__VA_ARGS__)
int _printf_c(FILE *stream,const wchar_t* format,...); // i put it at the end of this file
//#define printf_c(...) _printf_c(L##__VA_ARGS__)
//#define perror_c(...) fprintf(stderr,__VA_ARGS__)
#define fprintf_c(stream,...) _printf_c(stream,__VA_ARGS__)
#define printf_c(...) fprintf_c(stdout,L##__VA_ARGS__)
#define perror_c(...) fprintf_c(stderr,L##__VA_ARGS__)
#define isspace_c(s) iswspace(s)
#elif defined(ISUNIX)
typedef char char_t;
#define char_c(ch) ch // keep default
#define strcmp_c(s1,s2) strcmp(s1,s2)
#define strcat_c(s1,s2) strcat(s1,s2)
#define strlen_c(s) strlen(s)
#define strcpy_c(s1,s2) strcpy(s1,s2)
#define fprintf_c(...) fprintf(__VA_ARGS__)
#define printf_c(...) fprintf_c(stdout,__VA_ARGS__)
#define perror_c(...) fprintf_c(stderr,__VA_ARGS__)
#define isspace_c(s) isspace(s)
#endif

bool start(char_t *program,char_t **argument,int *returnvalue,char_t *inputfile,char_t *outputfile);

enum Option
{
  ExitOnFinished,
  NoTimer,
  ReturnTimer,
  ReturnReturn,
  ReturnSuccess,
  Help,
  NumberOfOptions
};
const char_t *options[NumberOfOptions][3] =
{
  {char_c("-e"),char_c("--exit"),char_c("Exit on finished without press enter key to exit.") ENDLINE
char_c("By default, you need to press enter key to exit.")},
  {char_c("-n"),char_c("--no-timer"),char_c("Do not display execution time.") ENDLINE
char_c("By default, the execution time will be displayed after finished.")},
  {char_c("-t"),char_c("--return-timer"),char_c("Returns the execution time in milliseconds on exited.") ENDLINE
char_c("By default, the return value is the one of the program.")},
  {char_c("-r"),char_c("--return-return"),char_c("Return the \"return value\" from the execution.")},
  {char_c("-s"),char_c("--return-success"),char_c("Always return success on exited.")},
  {char_c("-h"),char_c("--help"),char_c("Show this.")}
};

#if defined(ISWINDOWS)
#ifndef _MSC_VER // 
#include <stdarg.h>
int wmain(int argc,wchar_t **argv);
int main(int _argc,char **_argv)
{
  LPCWSTR lpCmdLine=GetCommandLineW();
  int argc;
  wchar_t **argv=CommandLineToArgvW(lpCmdLine,&argc);
  wmain(argc,argv);
  LocalFree(argv);
}
#endif // _MSC_VER
int wmain(int argc,wchar_t **argv)
#elif defined(ISUNIX)
int main(int argc,char **argv)
#endif
{
  bool opt[(size_t)NumberOfOptions];
  memset(opt,false,sizeof(opt));
  char_t *program=NULL;
  
  size_t argumentsize=0;
  char_t **argument=NULL;
  
  char_t *inputfile=NULL;
  char_t *outputfile=NULL;
  bool endofoptions=false;
  for(int i=1;i<argc;i++)
  {
    if(!endofoptions)
    {
      if(argv[i][0]!=char_c('-')||strcmp_c(argv[i],char_c("--"))==0)
      {
        endofoptions=true;
        if(argv[i][0]==char_c('-')&&i+1>=argc)
        {
          perror_c("Missing <program> argument after --." ENDLINE);
          return EXIT_FAILURE;
        }
        
        if(argv[i][0]==char_c('-')) i++;
        size_t programsize=strlen_c(argv[i])+1; //with \0 character
        program=(char_t*)calloc(programsize,sizeof(char_t));
        program[0]=char_c('\0');
        strcat_c(program,argv[i]);
        
        argumentsize=2;
//        argument=(char_t**)realloc(argument,argumentsize*sizeof(char_t*));
        argument=(char_t**)calloc(argc-i+1,sizeof(char_t*)); // +1 is NULL pointer at the end of list
        argument[0]=(char_t*)calloc(programsize,sizeof(char_t));
        strcpy_c(argument[0],program);
        argument[1]=NULL;
      }
      else if(strcmp_c(argv[i],char_c("-i"))==0)
      {
        if(i+1>=argc)
        {
          perror_c("Missing <inputfile> argument after -i." ENDLINE);
          return EXIT_FAILURE;
        }
        i++;
        inputfile=(char_t*)calloc(strlen_c(argv[i])+1,sizeof(char_t)); // with \0 char
        strcpy_c(inputfile,argv[i]);
      }
      else if(strcmp_c(argv[i],char_c("-o"))==0)
      {
        if(i+1>=argc)
        {
          perror_c("Missing <outputfile> argument after -o." ENDLINE);
          return EXIT_FAILURE;
        }
        i++;
        outputfile=(char_t*)calloc(strlen_c(argv[i])+1,sizeof(char_t)); // with \0 char
        strcpy_c(outputfile,argv[i]);
      }
      else if(argv[i][0]==char_c('-'))
      {
        bool unknowed=true;
        for(int j=0;j<NumberOfOptions;j++)
        {
          if(strcmp_c(argv[i],options[j][0])==0||strcmp_c(argv[i],options[j][1])==0)
          {
            opt[j]=true;
            unknowed=false;
            // break;
          }
        }
        if(unknowed)
        {
          perror_c("Unknowed option %s. See %s --help for available options." ENDLINE,argv[i],argv[0]);
          return EXIT_FAILURE;
        }
      }
    }
    else
    {
      argument[argumentsize-1]=(char_t*)calloc(strlen_c(argv[i])+1,sizeof(char_t)); // with \0 character
      strcpy_c(argument[argumentsize-1],argv[i]);
      argument[argumentsize]=NULL;
      argumentsize++;
    }
  }
  
  if(opt[Help]||argc==1)
  {
    printf_c("Usage:" ENDLINE);
    printf_c("  %s [option] [-i <inputfile>] [-o <outputfile>] [--] <program> [arguments...]" ENDLINE,argv[0]);
    printf_c("Options:" ENDLINE);
    printf_c("  [--] Everything after -- will be <program> [arguments...]. Use for program" ENDLINE
           "has name start with \"-\". By default, <program> is the first argument which" ENDLINE
           "don't start with \"-\" and [arguments...] is the left over." ENDLINE);
    printf_c("  [-i/-o <file>] Parse <file>'s content as stdin/stdout when starting process." ENDLINE);
    for(int j=0;j<NumberOfOptions;j++)
    {
      printf_c("  [%s/%s] %s" ENDLINE,options[j][0],options[j][1],options[j][2]);
    }
    return EXIT_SUCCESS;
  }

  // if(program==NULL)
  // {
  //   printf("Missing argument <program>. Exit!%s",ENDLINE);
  //   return EXIT_FAILURE;
  // }


  long long timermillis; // unix: use long long to handle large number of milliseconds since epoch
  long double timersec;
  int returnvalue=EXIT_FAILURE;
  bool successed=false;

#if defined(ISWINDOWS)

  long long timerclock=(long long)clock();

  successed=start(program,argument,&returnvalue,inputfile,outputfile);

  timerclock=clock()-timerclock;
  timersec=timerclock/(long double)CLOCKS_PER_SEC;
  timermillis=(long long)(timersec*1000);

#elif defined(ISUNIX) // clock() doesn't seem to be work on linux systems when using system() to start process
  // 1 seconds = 1000 millisecond
  // 1 millisecond = 1 000 000 nanosecond ( *.tv_nsec )
  // there is CLOCK_BOOTTIME better than CLOCK_MONOTONIC in some case
  // but it's only available on linux

  struct timespec timer;
  clock_gettime(CLOCK_REALTIME,&timer);
  timermillis=(long long)timer.tv_sec*1000+timer.tv_nsec/1000000;
  // printf("%d %d\n",timer.tv_sec,timer.tv_nsec);

  successed=start(program,argument,&returnvalue,inputfile,outputfile);

  clock_gettime(CLOCK_REALTIME,&timer);
  timermillis=(long long)timer.tv_sec*1000+timer.tv_nsec/1000000 - timermillis;
  timersec=timermillis/(long double)1000;

#endif


  free(program);
  if(inputfile!=NULL) free(inputfile);
  if(outputfile!=NULL) free(outputfile);
  
  printf_c("" ENDLINE);
  printf_c("--------------------------------" ENDLINE);
  printf_c("Process exited with return value %d",returnvalue);
  if(!opt[NoTimer]) printf_c(" after %.3fs",(double)timersec);
  printf_c("." ENDLINE);


  if(!opt[ExitOnFinished])
  {
    printf_c("Press enter key to continue . . .");
    getchar();
  }
  

  if(opt[ReturnTimer]) return timermillis;
  else if(opt[ReturnReturn]) return returnvalue;
  else if(opt[ReturnSuccess]) return EXIT_SUCCESS;

  return returnvalue;
}

bool start(char_t *program,char_t **argument,int *returnvalue,char_t *inputfile,char_t *outputfile)
{
#if defined(ISWINDOWS)
  STARTUPINFOW si;
  PROCESS_INFORMATION pi;
  HANDLE hinputfile=INVALID_HANDLE_VALUE;
  HANDLE houtputfile=INVALID_HANDLE_VALUE;
  DWORD ret=-1;
  bool successed=true;
  
  char_t *command=NULL;
  size_t commandsize=1; // with \0 char
  for(int i=0;argument[i]!=NULL;i++)
  {
    if(i!=0) commandsize++;
    bool hasspace=false;
    for(int j=0;argument[i][j]!=L'\0';j++)
    {
      commandsize++;
      if(argument[i][j]==L' ') hasspace=true;
    }
    if(hasspace) commandsize+=2;
  }
  command=(char_t*)calloc(commandsize,sizeof(char_t));
  command[0]=L'\0';
  for(int i=0;argument[i]!=NULL;i++)
  {
    bool hasspace=false;
    for(int j=0;argument[i][j]!=L'\0';j++)
    {
      if(argument[i][j]==L' ')
      {
        hasspace=true;
        break;
      }
    }
    if(i!=0) strcat_c(command,L" ");
    if(hasspace) strcat_c(command,L"\"");
    strcat_c(command,argument[i]);
    if(hasspace) strcat_c(command,L"\"");
  }

  ZeroMemory(&si,sizeof(si));
  si.cb=sizeof(si);
  ZeroMemory(&pi,sizeof(pi));
  
  
  si.dwFlags |= STARTF_USESTDHANDLES;
  si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
  si.hStdOutput= GetStdHandle(STD_OUTPUT_HANDLE);
  si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
  
  SECURITY_ATTRIBUTES attributes;
  attributes.nLength=sizeof(SECURITY_ATTRIBUTES);
  attributes.lpSecurityDescriptor=NULL;
  attributes.bInheritHandle=TRUE; // enable handle to be inherited, must have
  
  if(inputfile!=NULL)
  {
    hinputfile=CreateFileW(inputfile,GENERIC_READ,0,&attributes,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    if(hinputfile==INVALID_HANDLE_VALUE)
    {
      DWORD errorcode=GetLastError();
      LPWSTR errorstring=NULL;
      FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL,errorcode,MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),(LPWSTR)&errorstring,0,NULL);
      perror_c("Couldn't open input file: %s%s",inputfile,ENDLINE);
      perror_c("Error: %s",errorstring); // message have \r\n in the end
  
      LocalFree(errorstring);
      successed=false;
      goto exit;
    }
    si.hStdInput=hinputfile;
  }
  if(outputfile!=NULL)
  {
    houtputfile=CreateFileW(outputfile,GENERIC_WRITE,0,&attributes,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
    if(houtputfile==INVALID_HANDLE_VALUE)
    {
      DWORD errorcode=GetLastError();
      LPWSTR errorstring=NULL;
      FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL,errorcode,MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),(LPWSTR)&errorstring,0,NULL);
      perror_c("Couldn't open output file: %s%s",outputfile,ENDLINE);
      perror_c("Error: %s",errorstring); // message have \r\n in the end
  
      LocalFree(errorstring);
      successed=false;
      goto exit;
    }
    si.hStdOutput=houtputfile;
  }
  
  
  if(0==CreateProcessW(NULL,command,NULL,NULL,TRUE,0,NULL,NULL,&si,&pi))
  {
    DWORD errorcode=GetLastError();
    LPWSTR errorstring=NULL;
    FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                  NULL,errorcode,MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),(LPWSTR)&errorstring,0,NULL);
    perror_c("Couldn't start command line: %s%s",command,ENDLINE);
    perror_c("Error: %s",errorstring); // message have \r\n in the end

    LocalFree(errorstring);
    successed=false;
    goto exit;
  }

  WaitForSingleObject(pi.hProcess,INFINITE);

  GetExitCodeProcess(pi.hProcess,&ret);
  
exit:
  
  
  if(hinputfile!=INVALID_HANDLE_VALUE) CloseHandle(hinputfile);
  if(houtputfile!=INVALID_HANDLE_VALUE) CloseHandle(houtputfile);
  if(pi.hProcess!=NULL) CloseHandle(pi.hProcess); // we ZeroMemory before so pi.hProcess must be NULL
  if(pi.hThread!=NULL) CloseHandle(pi.hThread); // pi.hThread as well

  if(returnvalue!=NULL) *returnvalue=ret;
  return successed;
#elif defined(ISUNIX)
  pid_t newpid=fork();
  if(newpid<0)
  {
    perror_c("Error: Couldn't required system to create new process.");
    return false;
  }
  if(newpid==0)
  {
    if(inputfile!=0) freopen(inputfile,"r",stdin);
    if(outputfile!=0) freopen(outputfile,"w",stdout);
    if(execvp(program,argument)==-1)
    {
      perror_c("Error: Couldn't start program %s." ENDLINE,program);
      exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
  }
  else
  {
    // printf_c("Debug");
    int status;
    int wpid;
    do
    {
      wpid=waitpid(newpid,&status,WUNTRACED);
      if(wpid==-1)
      {
        perror_c("Error: Couldn't required system to wait for process with pid %d." ENDLINE,newpid);
        return EXIT_FAILURE;
      }
      if(WIFEXITED(status)) *returnvalue=WEXITSTATUS(status);
      else if(WIFSIGNALED(status)) *returnvalue=EXIT_FAILURE;
    } while(!WIFEXITED(status)&&WIFSIGNALED(status));
    return true;
  }
#endif
}

#if defined(ISWINDOWS)
//need to convert %s into %ls to support wide string
int _printf_c(FILE *stream,const wchar_t* format,...)
{
  wchar_t *_format;
  size_t _formatsize=0;
  for(int i=0;format[i]!=L'\0';i++)
  {
    if(format[i]==L'%'&&format[i+1]==L'%') _formatsize+=2,i++;
    else if(format[i]==L'%'&&format[i+1]==L's') _formatsize+=3,i++;
    else _formatsize++;
  }
  _format=calloc(_formatsize+1,sizeof(wchar_t));
  for(int i=0,j=0;format[i]!=L'\0';i++)
  {
    if(format[i]==L'%'&&format[i+1]==L'%')
    {
      _format[j++]=L'%';
      _format[j++]=L'%';
      i++;
    }
    else if(format[i]==L'%'&&format[i+1]==L's')
    {
      _format[j++]=L'%';
      _format[j++]=L'l';
      _format[j++]=L's';
      i++;
    }
    else _format[j++]=format[i];
  }
  _format[_formatsize]=L'\0';
  va_list args;
  va_start(args,format);
  int ret=vfwprintf(stream,_format,args);
  va_end(args);
  free(_format);
  return ret;
}
#endif
