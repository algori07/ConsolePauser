#include <stdio.h> // io
#include <string.h> // for parsing argument
#include <stdbool.h> // bool

// use \r\n for windows and \n for unix
#if defined(_WIN32)
  #define ISWINDOWS
  #define ENDLINE "\r\n"
  #include <windows.h> // for CreateProcess() and getch()
  #include <time.h> // for clock() and CLOCKS_PER_SEC
#elif defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__))
  #define ISUNIX
  #define ENDLINE "\n"
  #include <stdlib.h> // for system()
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
      program=(char*)realloc(program,programsize);
      strcat(program," ");
      strcat(program,argv[i]);
    }
    if(!kt)
    {
      if(program==NULL)
      {
        programsize=strlen(argv[i])+1; //with \0 character
        program=(char*)malloc(programsize*sizeof(char));
        strcpy(program,argv[i]);
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
  
  unsigned long long timer_start,timermilis;
  long double timersec;
  int returnvalue;
  
#if defined(ISWINDOWS)
  timersec=clock(); // not in seconds unit
  returnvalue=start(program);
  timersec=(long double)(clock()-timersec)/CLOCKS_PER_SEC; // in seconds unit
  timermilis=timersec*1000;
#elif defined(ISUNIX) // clock() doesn't seem to be work on linux systems when using system() to start process
  timeval tv;
  gettimeofday(&tv, NULL);
  timermilis = tv.tv_sec*1000 + tv.tv_usec/1000;
  timersec = tv.tv_sec + (double)tv.tv_usec/1000000;
  
  returnvalue=start(program);
  
  gettimeofday(&tv, NULL);
  timermilis = tv.tv_sec*1000 + tv.tv_usec/1000 - timermilis;
  timersec = tv.tv_sec + (double)tv.tv_usec/1000000 - timersec;
  
  
#endif
  
  
  
  // if(opt[ExitOnFinished])
  //  if(opt[ReturnTimer]) return timer;
  //  else if(opt[ReturnReturn]) return returnvalue;
  //  else if(opt[ReturnSuccess]) return EXIT_SUCCESS;
  
  printf("%s",ENDLINE);
  printf("--------------------------------%s",ENDLINE);
  printf("Process exited with return value %d",returnvalue);
  if(!opt[NoTimer]) printf(" after %.3fs",timersec);
  printf(".%s",ENDLINE);
  

  if(!opt[ExitOnFinished])
  {
#if defined(ISWINDOWS)
    printf("Press any key to continue . . .");
    fflush(stdin);
    getchar();
#else
    printf("Press enter key to continue . . .");
    getch();
#endif
  }
  
  
  if(opt[ReturnTimer]) return timermilis;
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
  
  char *command2=malloc(strlen(command)*sizeof(char));
  strcpy(command2,command); // remove "const"
  
  CreateProcess(NULL,command2,NULL,NULL,FALSE,0,NULL,NULL,&si,&pi);
  WaitForSingleObject(pi.hProcess,INFINITE);
  
  DWORD ret;
  GetExitCodeProcess(pi.hProcess,&ret);
  
  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);
  free(command2);
  
  return ret;
#elif defined(ISUNIX)
  int ret=system("command");
  return ret;
#endif
}