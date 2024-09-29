#if defined(_WIN32)
  #define ISWINDOWS
  #define _CRT_SECURE_NO_WARNINGS
  #define ENDLINE "\r\n"
  #include <windows.h> // for CreateProcess()
  #include <conio.h> // for getch()
  #include <time.h> // for clock() and CLOCKS_PER_SEC
#elif defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__))
  #define ISUNIX
  #define ENDLINE "\n"
  #include <unistd.h>
  #include <time.h> // for clock_gettime() (only available on posix)
  #include <sys/wait.h>
#else
  #define ENDLINE "\n"
#endif

#include <stdio.h> // io
#include <stdlib.h> // for malloc() and realloc()
#include <string.h> // for parsing argument
#include <stdbool.h> // bool
#include <ctype.h> // isspace()

// use \r\n for windows and \n for unix

bool start(char *program,char **argument,int *returnvalue,char *inputfile,char *outputfile);

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
  {"-e","--exit","Exit on finished without press enter key to exit." ENDLINE
"By default, you need to press enter key to exit."},
  {"-n","--no-timer","Do not display execution time." ENDLINE
"By default, the execution time will be displayed after finished."},
  {"-t","--return-timer","Returns the execution time in milliseconds on exited." ENDLINE
"By default, the return value is the one of the program."},
  {"-r","--return-return","Return the \"return value\" from the execution."},
  {"-s","--return-success","Always return success on exited."},
  {"-h","--help","Show this."}
};


int main(int argc,char **argv)
{
  bool opt[(size_t)NumberOfOptions];
  memset(opt,false,sizeof(opt));
  // size_t programsize=0; // for realloc
  char *program=NULL;
  
  size_t argumentsize=0; // for realloc
  char **argument=NULL;
  
  char *inputfile=NULL;
  char *outputfile=NULL;
  bool endofoptions=false;
  for(int i=1;i<argc;i++)
  {
    if(!endofoptions)
    {
      if(argv[i][0]!='-'||strcmp(argv[i],"--")==0)
      {
        endofoptions=true;
        if(i+1>=argc)
        {
          printf("Missing <program> argument after --.");
          return EXIT_FAILURE;
        }
        
        if(argv[i][0]=='-') i++;
        size_t programsize=strlen(argv[i])+1; //with \0 character
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
        program=(char*)calloc(programsize,sizeof(char));
        if(program==NULL) // check for error if couldn't allocate memory
        {
          printf("Error: Couldn't allocate enough memory. Try shorter command line.%s",ENDLINE);
          return EXIT_FAILURE;
        }
        program[0]='\0';
        if(hasspace) strcat(program,"\"");
        strcat(program,argv[i]);
        if(hasspace) strcat(program,"\"");
        
        argumentsize=1;
        argument=(char**)realloc(argument,argumentsize*sizeof(char*));
        argument[0]=(char*)calloc(programsize,sizeof(char));
        strcpy(argument[0],program);
      }
      else if(strcmp(argv[i],"-i")==0)
      {
        if(i+1>=argc)
        {
          printf("Missing <inputfile> argument after -i.");
          return EXIT_FAILURE;
        }
        i++;
        inputfile=calloc(strlen(argv[i]+1),sizeof(char)); // with \0 char
        strcpy(inputfile,argv[i]);
      }
      else if(strcmp(argv[i],"-o")==0)
      {
        if(i+1>=argc)
        {
          printf("Missing <outputfile> argument after -o.");
          return EXIT_FAILURE;
        }
        i++;
        outputfile=calloc(strlen(argv[i]+1),sizeof(char)); // with \0 char
        strcpy(outputfile,argv[i]);
      }
      else if(argv[i][0]=='-')
      {
        bool unknowed=true;
        for(int j=0;j<NumberOfOptions;j++)
        {
          if(strcmp(argv[i],options[j][0])==0||strcmp(argv[i],options[j][1])==0)
          {
            opt[j]=true;
            unknowed=false;
            // break;
          }
        }
        if(unknowed)
        {
          printf("Unknowed option %s. See %s --help for available options.",argv[i],argv[0]);
          return EXIT_FAILURE;
        }
      }
    }
    else
    {
      argumentsize++;
      argument=(char**)realloc(argument,argumentsize*sizeof(char*));
      argument[argumentsize-1]=(char*)calloc(strlen(argv[i])+1,sizeof(char)); // with \0 character
      strcpy(argument[argumentsize-1],argv[i]);
    }
  }
  
  argumentsize++;
  argument=(char**)realloc(argument,argumentsize*sizeof(char*));
  argument[argumentsize-1]=NULL;
  
  
  if(opt[Help]||argc==1)
  {
    printf("Usage:" ENDLINE);
    printf("  %s [option] [-i <inputfile>] [-o <outputfile>] [--] <program> [arguments...]" ENDLINE,argv[0]);
    printf("Options:" ENDLINE);
    printf("  [--] Everything after -- will be <program> [arguments...]. Use for program" ENDLINE
           "has name start with \"-\". By default, <program> is the first argument which" ENDLINE
           "don't start with \"-\" and [arguments...] is the left over." ENDLINE);
    printf("  [-i/-o <file>] Parse <file>'s content as stdin/stdout when starting process." ENDLINE);
    for(int j=0;j<NumberOfOptions;j++)
    {
      printf("  [%s/%s] %s" ENDLINE,options[j][0],options[j][1],options[j][2]);
    }
    return EXIT_SUCCESS;
  }

  // if(program==NULL)
  // {
  //   printf("Missing argument <program>. Exit!%s",ENDLINE);
  //   return EXIT_FAILURE;
  // }


  long long timermillis; // unix: use long long to handle large number of milliseconds since epoch
  long double timersec; // (not anymore) i don't know why, but use long double lead to overflows error
  int returnvalue=EXIT_FAILURE;
  bool successed;

#if defined(ISWINDOWS)

  long long timerclock=(long long)clock();

  start(program,&returnvalue,inputfile,outputfile);

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
  
  printf(ENDLINE);
  printf("--------------------------------" ENDLINE);
  printf("Process exited with return value %d",returnvalue);
  if(!opt[NoTimer]) printf(" after %.3fs",(double)timersec);
  printf("." ENDLINE);


  if(!opt[ExitOnFinished])
  {
#if defined(ISWINDOWS)
    printf("Press any key to continue . . .");
    _getch();
#else
    printf("Press enter key to continue . . .");
    getchar();
#endif
  }
  

  if(opt[ReturnTimer]) return timermillis;
  else if(opt[ReturnReturn]) return returnvalue;
  else if(opt[ReturnSuccess]) return EXIT_SUCCESS;

  return returnvalue;
}

bool start(char *program,char **argument,int *returnvalue,char *inputfile,char *outputfile)
{
#if defined(ISWINDOWS)
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  HANDLE hinputfile=NULL;
  HANDLE houtputfile=NULL;
  DWORD ret=-1;
  bool successed=true;

  ZeroMemory(&si,sizeof(si));
  si.cb=sizeof(si);
  
  
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
    hinputfile=CreateFile(inputfile,GENERIC_READ,0,&attributes,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    if(hinputfile==INVALID_HANDLE_VALUE)
    {
      DWORD errorcode=GetLastError();
      LPSTR errorstring=NULL;
      FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL,errorcode,MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),(LPSTR)&errorstring,0,NULL);
      printf("Couldn't open input file: %s%s",inputfile,ENDLINE);
      printf("Error: %s",errorstring); // message have \r\n in the end
  
      LocalFree(errorstring);
      successed=false;
      goto exit;
    }
    si.hStdInput=hinputfile;
  }
  if(outputfile!=NULL)
  {
    houtputfile=CreateFile(outputfile,GENERIC_WRITE,0,&attributes,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
    if(houtputfile==INVALID_HANDLE_VALUE)
    {
      DWORD errorcode=GetLastError();
      LPSTR errorstring=NULL;
      FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL,errorcode,MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),(LPSTR)&errorstring,0,NULL);
      printf("Couldn't open output file: %s%s",outputfile,ENDLINE);
      printf("Error: %s",errorstring); // message have \r\n in the end
  
      LocalFree(errorstring);
      successed=false;
      goto exit;
    }
    si.hStdOutput=houtputfile;
  }
  
  
  ZeroMemory(&pi,sizeof(pi));
  if(0==CreateProcess(NULL,command,NULL,NULL,TRUE,0,NULL,NULL,&si,&pi))
  {
    DWORD errorcode=GetLastError();
    LPSTR errorstring=NULL;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                  NULL,errorcode,MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),(LPSTR)&errorstring,0,NULL);
    printf("Couldn't start command line: %s%s",command,ENDLINE);
    printf("Error: %s",errorstring); // message have \r\n in the end

    LocalFree(errorstring);
    successed=false;
    goto exit;
  }

  WaitForSingleObject(pi.hProcess,INFINITE);

  GetExitCodeProcess(pi.hProcess,&ret);
  
exit:
  
  

  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);

  if(returnvalue!=NULL) *returnvalue=ret;
  return successed;
#elif defined(ISUNIX)
  pid_t pid=getpid();
  pid_t newpid=vfork(); // vfork let child process change stdin/stdout without change parent's one
  if(newpid<0)
  {
    printf("Couldn't required system to create new process.");
    return false;
  }
  if(newpid==0)
  {
    if(inputfile!=0) freopen(inputfile,"r",stdin);
    if(outputfile!=0) freopen(outputfile,"w",stdout);
    if(execvp(program,argument)==-1)
    {
      printf("Couldn't start program %s." ENDLINE,program);
      exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
  }
  else
  {
    // printf("Debug");
    int status;
    int wpid;
    do
    {
      wpid=waitpid(newpid,&status,WUNTRACED);
      if(wpid==-1)
      {
        printf("Couldn't required system to wait for process with pid %d." ENDLINE,newpid);
        return EXIT_FAILURE;
      }
      if(WIFEXITED(status)) *returnvalue=WEXITSTATUS(status);
      else if(WIFSIGNALED(status)) *returnvalue=EXIT_FAILURE;
    } while(!WIFEXITED(status)&&WIFSIGNALED(status));
    return true;
  }
#endif
}
