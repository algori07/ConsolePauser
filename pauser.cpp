#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <conio.h>
enum Option
{
	ExitOnFinished,
	NoTimer,
	ReturnTimer,
	ReturnSuccess,
	Help,
	NumberOfOptions
};
const char *options[NumberOfOptions][3]
{
	{"-e","--exit","Exit on finished. This will not show any messages about timer and exit status. \
By default, a message will be displayed and you need to press a key to exit."},
	{"-n","--no-timer","Do not display program execution time. \
By default, the program execution time will be displayed after finished."},
	{"-t","--return-timer","Returns the program execution time in milliseconds on exited. (Not recommended) \
By default, the return value is the one of the program."},
	{"-s","--return-success","Always return success on exited."},
	{"-h","--help","Show this."}
};

int main(int argc,char **argv)
{
	bool opt[NumberOfOptions];
	memset(opt,false,sizeof(opt));
	for(int i=1;i<argc-1;i++)
	{
		for(int j=0;j<NumberOfOptions;j++)
		{
			if(strcmp(argv[i],options[j][0])==0||strcmp(argv[i],options[j][1])==0)
			{
				opt[j]=true;
			}
		}
	}
	if(opt[Help]||argc==1)
	{
		printf("Usage: %s [option] \"<program> arguments ...\"\n",argv[0]);
		for(int j=0;j<NumberOfOptions;j++)
		{
			printf("%s %s \t    %s \r\n",options[j][0],options[j][1],options[j][2]);
		}
		return 0;
	}
	const char *program=argv[argc-1];
	
	clock_t timer_start,timer_end,timer;
	int returnvalue;
	
	
	timer_start=clock();
	returnvalue=system(program);
	timer_end=clock();
	timer=timer_end-timer_start;
	
	
	if(opt[ExitOnFinished])
		if(opt[ReturnTimer]) return timer;
		else if(opt[ReturnSuccess]) return 0;
	
	printf("--------------------------------\r\n");
	printf("Process exited after %.3fs",(float)timer/1000);
	if(!opt[NoTimer]) printf(" with return value %d",returnvalue);
	printf("\r\n");
	

	printf("Press any key to continue . . .");
	getch();
	
	
	if(opt[ReturnTimer]) return timer;
	else if(opt[ReturnSuccess]) return 0;
	
	return returnvalue;
}