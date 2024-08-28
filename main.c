#include <stdio.h> // io
#include <stdlib.h> // for malloc() and realloc()
#include <string.h> // for parsing argument
#include <stdbool.h> // bool
#include <ctype.h> // isspace()

// use \r\n for windows and \n for unix
#if defined(_WIN32)
  #define ISWINDOWS
  #define ENDLINE "\r\n"
  #include <windows.h> // for CreateProcess()
  #include <conio.h> // for getch()
  #include <time.h> // for clock() and CLOCKS_PER_SEC
#elif defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__))
  #define ISUNIX
  #define ENDLINE "\n"
  #include <stdlib.h> // for system()
  #include <time.h> // for clock_gettime() (only available on posix)
#endif

int start(const char *command);

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
const char *options[NumberOfOptions][3] =
{
  {"-e","--exit","Exit on finished without press enter key to exit. \n\
By default, you need to press enter key to exit."},
  {"-n","--no-timer","Do not display execution time. \n\
By default, the execution time will be displayed after finished."},
  {"-t","--return-timer","Returns the execution time in milliseconds on exited. \n\
By default, the return value is the one of the program."},
  {"-r","--return-return","Return the \"return value\" from the execution."},
  {"-s","--return-success","Always return success on exited."},
  {"-h","--help","Show this."}
};

int main(int argc,char **argv)
{
  bool opt[NumberOfOptions];
  memset(opt,false,sizeof(opt));
  size_t programsize=0; // for realloc
  char *program=NULL;
  for(int i=1;i<argc;i++)
  {
    bool kt=false;
    if(program==NULL)
    {
      for(int j=0;j<NumberOfOptions;j++)
      {
        if(strcmp(argv[i],options[j][0])==0||strcmp(argv[i],options[j][1])==0)
        {
          opt[j]=true;
          kt=true;
        }
      }
    }
    else
    {
      programsize+=strlen(argv[i])+1; // with a space character
      bool hasspace=false;
      for(int idx=0;argv[i][idx]!='\0';++idx)
      {
        if(isspace(argv[i][idx]))
        {
          hasspace=true;
          programsize+=2; // and this -> ""
          break;
        }
      }
      program=(char*)realloc(program,programsize);
      strcat(program,(hasspace?" \"":" "));
      strcat(program,argv[i]);
      if(hasspace) strcat(program,"\"");
    }
    if(!kt)
    {
      if(program==NULL)
      {
        programsize=strlen(argv[i])+1; //with \0 character
        bool hasspace=false;
        for(int idx=0;argv[i][idx]!='\0';++idx)
        {
          if(isspace(argv[i][idx]))
          {
            hasspace=true;
            programsize+=2; // and this -> ""
            break;
          }
        }
        program=(char*)malloc(programsize*sizeof(char));
        program[0]='\0';
        if(hasspace) strcat(program,"\"");
        strcat(program,argv[i]);
        if(hasspace) strcat(program,"\"");
      }
    }
  }
  if(opt[Help]||argc==1)
  {
    printf("Usage: %s [option] \"<program> arguments ...\"%s",argv[0],ENDLINE);
    printf("Options: %s",ENDLINE);
    for(int j=0;j<NumberOfOptions;j++)
    {
      printf("  [%s/%s] %s \r%s",options[j][0],options[j][1],options[j][2],ENDLINE);
    }
    return EXIT_SUCCESS;
  }
  
  
  long long timermillis; // use long long to handle large number of milliseconds since epoch
  float timersec;
  int returnvalue;
  
#if defined(ISWINDOWS)
  
  clock_t timerclock=clock();
  
  returnvalue=start(program);
  
  timerclock=clock()-timerclock;
  timersec=timerclock/(float)CLOCKS_PER_SEC;
  timermillis=timersec*1000;
  
#elif defined(ISUNIX) // clock() doesn't seem to be work on linux systems when using system() to start process
  // 1 seconds = 1000 millisecond
  // 1 millisecond = 1 000 000 nanosecond ( *.tv_nsec )
  // there is CLOCK_BOOTTIME better than CLOCK_MONOTONIC in some case
  // but it's only available on linux
  
  struct timespec timer;
  clock_gettime(CLOCK_REALTIME,&timer);
  timermillis=(long long)timer.tv_sec*1000+timer.tv_nsec/1000000;
  // printf("%d %d\n",timer.tv_sec,timer.tv_nsec);
  
  returnvalue=start(program);
  
  clock_gettime(CLOCK_REALTIME,&timer);
  timermillis=(long long)timer.tv_sec*1000+timer.tv_nsec/1000000 - timermillis;
  // printf("%d %d\n",timer.tv_sec,timer.tv_nsec);
  timersec=(float)timermillis/1000;
  
#endif
  
  
  
  
  printf("%s",ENDLINE);
  printf("--------------------------------%s",ENDLINE);
  printf("Process exited with return value %d",returnvalue);
  if(!opt[NoTimer]) printf(" after %.3fs",timersec);
  printf(".%s",ENDLINE);
  

  if(!opt[ExitOnFinished])
  {
#if defined(ISWINDOWS)
    printf("Press any key to continue . . .");
    _getch();
#else
    printf("Press enter key to continue . . .");
    fflush(stdin);
    getchar();
#endif
  }
  
  
  if(opt[ReturnTimer]) return timermillis;
  else if(opt[ReturnReturn]) return returnvalue;
  else if(opt[ReturnSuccess]) return EXIT_SUCCESS;
  
  return returnvalue;
}



int start(const char *command)
{
#if defined(ISWINDOWS)
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  
  ZeroMemory( &si, sizeof(si) );
  si.cb = sizeof(si);
  ZeroMemory( &pi, sizeof(pi) );
  
  char *command2=(char*)malloc((strlen(command)+1)*sizeof(char)); // +1 for \0 at the end string
  strcpy(command2,command); // remove "const"
  
  // bool successed=;
  if((0==CreateProcess(NULL,command2,NULL,NULL,FALSE,0,NULL,NULL,&si,&pi)))
  {
    DWORD errorcode=GetLastError();
    LPSTR errorstring=NULL;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                  NULL,errorcode,MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),(LPSTR)&errorstring,0,NULL);
    printf("Couldn't start command line: %s%s",command,ENDLINE);
    printf("Error: %s",errorstring); // message have \r\n in the end
    
    LocalFree(errorstring);
  }
  
  WaitForSingleObject(pi.hProcess,INFINITE);
  
  DWORD ret;
  GetExitCodeProcess(pi.hProcess,&ret);
  
  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);
  free(command2);
  
  return ret;
#elif defined(ISUNIX)
  int ret=system(command);
  return ret;
#endif
}