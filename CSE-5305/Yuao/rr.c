#include<stdio.h>
#include<stdlib.h>

int main (int argc, char *argv[])
{
  int job_num = argc - 1;
  int *ptr = (int *) malloc (job_num * sizeof (int));
  int run = 0;
  for (int i = 0; i < job_num; i++)
    {
      ptr[i] = 0;
    }
  int i = 0;
  while (run <= job_num)
    {
      if (ptr[i % job_num] == -1)
	{ i++;
	  continue;
	}
      char c = argv[i % job_num + 1][ptr[i % job_num]];
      ptr[i % job_num]++;
      if (c != '\0')
	{
	  printf ("job %d printing %c\n", i % job_num, c);
	}
      else
	{
	  ptr[i % job_num] = -1;
	  run++;
	  printf ("job %d finished\n", i % job_num);
	}
      i++;
    }
}
