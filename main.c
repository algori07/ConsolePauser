#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

// use \r\n for windows and \n for unix
#if defined(_WIN32)
	#define ENDLINE "\r\n"
#elif defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__))
	#define ENDLINE "\n"
#endif

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
	
	clock_t timer_start,timermilis;
	float timer;
	int returnvalue;
	
	timer_start=clock();
	returnvalue=system(program);
	timer=(float)(clock()-timer_start)/CLOCKS_PER_SEC;
	timermilis=timer*1000;
	
	
	// if(opt[ExitOnFinished])
	// 	if(opt[ReturnTimer]) return timer;
	// 	else if(opt[ReturnReturn]) return returnvalue;
	// 	else if(opt[ReturnSuccess]) return EXIT_SUCCESS;
	
	printf("%s",ENDLINE);
	printf("--------------------------------%s",ENDLINE);
	printf("Process exited with return value %d",returnvalue);
	if(!opt[NoTimer]) printf(" after %.3fs",timer);
	printf(".%s",ENDLINE);
	

	if(!opt[ExitOnFinished])
	{
		printf("Press enter to continue . . .");
		fflush(stdin);
		getchar();
	}
	
	
	if(opt[ReturnTimer]) return timermilis;
	else if(opt[ReturnReturn]) return returnvalue;
	else if(opt[ReturnSuccess]) return EXIT_SUCCESS;
	
	return returnvalue;
}