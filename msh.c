
                /*
       
                            Name: BALAJI PAIYUR MOHAN
                            UTA ID: 1001576836

                  */



#define _GNU_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 5     // Mav shell only supports five arguments 

//Function to handle signals from Ctrl-C & Ctrl-Z
static void signal_handler(int s)
{
   if( s == 2)  //si_signo returns 2 if a process gets terminated
      printf("\n Process Terminated\n");
   else
      printf("\n Proces Suspended\n");
}

int main()
{

  char * cmd_str = (char*) malloc( MAX_COMMAND_SIZE );//Dynamically allocating pointer memory
 
  struct sigaction act; //creating struct variable for sigaction
  memset (&act, '\0', sizeof(act)); //assigning null value to each member
   act.sa_handler = &signal_handler;//function call by reference
  if( sigaction( SIGINT, &act, NULL) < 0)
  {
   //passing SIGINT interrupt using Ctrl-C to terminate process 
     perror("sigaction");//print error message
     return 1;
  }
  else if( sigaction( SIGTSTP,&act, NULL) <0) 
  {
     //passing SIGTSTP interrupt using Ctrl-Z to suspend process
     perror("sigaction");//print error message
     return 1;
  } 
  
  int a,k=0;
  //int i=0,j;
  pid_t pid[10];//initializing array of  pid variables
  //char* cmd_history;//storing array of strings 
  //cmd_history = (char *) calloc(15,sizeof(char));
  int status;

  while( 1 )//infinite loop
  {
    start:
    // Print out the msh prompt
    printf ("msh> ");

    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    
    fgets (cmd_str, MAX_COMMAND_SIZE, stdin);
    if (strcmp (cmd_str, "\n") ==0)//To check if there is an input
           goto start;//transfers the line of control to start
        
/* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    int   token_count = 0;                                 
                                                           
    // Pointer to point to the token
    // parsed by strsep
    char *arg_ptr;                                         
                                                           
    char *working_str  = strdup( cmd_str );                

    // we are going to move the working_str pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
  /* 
    strcpy(cmd_history[i], working_str);//copies the commands and stores in array to retrieve for command history
    i++;   
       if( i==14)//resetting the array pointer to avoid overflow
                i=0;*/
    // Tokenize the input stringswith whitespace used as the delimiter
    while ( ( (arg_ptr = strsep(&working_str, WHITESPACE ) ) != NULL) && 
              (token_count<MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup( arg_ptr, MAX_COMMAND_SIZE );
      if( strlen( token[token_count] ) == 0 )
      {
        token[token_count] = NULL;
      }
        token_count++;
    }
     pid_t  pid1;    //creating a new process
     pid[k]= pid1; //store the pids of processes as history
     k++;
     if(k==9)//resetting the array pointerto avoid overflow
         k=0;
      
     pid1 = fork(); //forking the process
    
    if ((strcmp(*token,"exit")) == 0)//Checking if the command is exit
            exit(0);
    else if ((strcmp(*token,"quit")) == 0)//Checking if the command is quit
            return 0;
    else if (strcmp(*token, "bg") == 0)//Checking if the command is bg
    {
          pid_t bg_pid = getpid();//instantiating the process to current pid
          waitpid(bg_pid,&status,0);//suspends execution of current process and returns the pid of child whose state has changed
    }
 /*   else if( strcmp(*token,"history" ) == 0)//Check if the command is "history"
    {
          for(j=0;j<15;j++)
          {
            printf("%s  ",cmd_history[j]);//displaying the array of commands in history
          }
          free(cmd_history);
    }
*/
    else if (strcmp(*token,"showpids") ==0)//Check if the command is "showpids"
    {
          printf("\n");          
          for(a=0;a<10;a++)
          {
                 printf("%d: %d \n",a,pid[a]);//displays the array of last 10 pids
          }
          continue;          
    }
    else if (strcmp(*token, "kill") == 0)//check if the command is "kill"
    {
 	 if( kill(pid_t)(uintptr_t)token[1],SIGKILL) < 0)//kills the process id mentioned and throws error on failure
		 perror("msh:");
    }
    else if (strcmp(*token, "killall") == 0)//check if the command is "killall"
    {
	 kill(0,SIGKILL);//kills all the running processes
    }
    else if (strcmp(*token, "cd") ==0)//check if the command is "cd"
    {
          if( token[1] == NULL)//check if no arguments are provided
              printf("No argument provided for \"cd\".\n");
          else if( chdir( token[1]) != 0)//chdir() command changes the current working directory and throws an error message on invalid argument 
              perror("msh");
          continue;
    }
    else
    {
     if ( pid1 < 0)      //Check if forking a child process
     {
          printf("Forking a Child Process FAILED");//print failure message
          exit(1);
     }
     else if( pid1 ==0)
     {
          if( execvp(*token,token) < 0) //as belonging to exec() family performs execution of command as in the directory and throws an error message on failure
         { 
          printf("msh: Command not found.\n");//printing error message       
         }
          exit(1);
     }
     else //for all the other processes
     {      //wait  for the completion of parent process      
          while (wait(&status) != pid1)       
                   ;
     }
     }
    }
   return 0; // return void
}
