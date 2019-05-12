#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_LENGTH 20
#define NUM_PARAMS 11 //number of parameters we intend to read (except for floats)


const char *conf_types[NUM_PARAMS] = {"SEED", "INIT_TIME", "FIN_TIME", "ARRIVE_MIN", "ARRIVE_MAX", "CPU_MIN", "CPU_MAX", "DISK1_MIN", "DISK1_MAX", "DISK2_MIN", "DISK2_MAX"};

int conf_vals[NUM_PARAMS] = {};

void getInput(char* FILE_NAME)
{

  // opens the config file
  FILE * fp;
  fp = fopen( FILE_NAME, "r" );

  // buffers for config file line reading
  char search_str[LINE_LENGTH];
  char line[LINE_LENGTH];

  // for line in config file
  int i;
  while ( fgets( line, LINE_LENGTH, fp ) != NULL )
    {

      for (i=0;i<NUM_PARAMS;i++)
  	{
  	  strcpy( search_str, conf_types[i] );
  	  strcat( search_str, " %d\n" );
  	  sscanf( line, search_str, &conf_vals[i] );
  	}
    }


  fclose(fp);



}
