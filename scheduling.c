// must compile with: gcc  -std=c99 -Wall -o scheduling scheduling.c

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// state 0: not arrived, 1: ready, 2: running, 3: blocked 4: finished
struct Process
{
  int pid;
  int cpu_time;
  int io_time;
  int arrival_time;
  int remain_cpu;
  int remain_io;
  int state;
};

const char *state_string[5] = {"not arrived", "ready", "running", "blocked", "finished"};

struct Process *readInput(FILE *fp, int *count)
{
  fscanf(fp, "%d\n", count);

  struct Process *processes = (struct Process *)malloc(sizeof(struct Process) * *count);

  for (int i = 0; i < *count; i++)
  {
    fscanf(fp, "%d %d %d %d\n", &processes[i].pid, &processes[i].cpu_time, &processes[i].io_time, &processes[i].arrival_time);
    processes[i].state = 0;
    processes[i].cpu_time += processes[i].cpu_time % 2;
    processes[i].remain_cpu = processes[i].cpu_time;
    processes[i].remain_io = processes[i].io_time;
  }

  return processes;
}

void shiftQueue(int *queue, int *que_count)
{
  for (int i = 1; i < *que_count; i++)
  {
    queue[i - 1] = queue[i];
  }
  *que_count = *que_count - 1;
}

void fcfs(struct Process *processes, int count, char *out_filename)
{
  int time = 0;
  int running_pid = -1;
  int finished_count = 0;

  int *ready_queue = (int *)malloc(sizeof(int) * count);
  int que_count = 0;

  while (time < 10)
  {
    if(finished_count == count) {
      break;
    }
    // check arrival
    for (int i = 0; i < count; i++)
    {
      if (processes[i].arrival_time == time)
      {
        processes[i].state = 1;
        ready_queue[count] = processes[i].pid;
        que_count++;
      }
    }

    if (running_pid == -1 && que_count > 0)
    {
      running_pid = ready_queue[0];
      shiftQueue(ready_queue, &que_count);
      processes[running_pid].state = 2;
    }
    

    if (running_pid == -1)
    {
      // running process not exist
    }
    else
    {
      // running process exist
      //cpu burst
      processes[running_pid].remain_cpu -= 1;
    }

    //io burst
    for(int i =0; i < count; i++) {
      if(processes[i].state == 3) {
        processes[i].remain_io -= 1;
      }
    }

    printf("%d ", time);
    for(int i = 0; i < count; i++) {
      if(processes[i].state != 0 && processes[i].state != 4) {
        printf("%d:%s ", processes[i].pid, state_string[processes[i].state]);
      }
    }
    printf("\n");

    if(running_pid != -1 && processes[running_pid].remain_cpu == processes[running_pid].cpu_time / 2) {
      processes[running_pid].state = 3;
      ready_queue[count] = running_pid;
      count += 1;
      running_pid = -1;
    } else if (running_pid != -1 && processes[running_pid].remain_cpu == 0) {
      finished_count+= 1;
      processes[running_pid].state =4;
      running_pid = -1;
    } else {
      for(int i =0; i < count; i++) {
        if(processes[i].state == 3 && processes[i].remain_io == 0) {
          ready_queue[que_count] = processes[i].pid;
          que_count++;
          processes[i].state = 1;
        }
      }
    }



    time++;


  }
}

int main(int argc, char *argv[])
{

  int scheduling;
  FILE *fp;                // for creating the output file
  char filename[100] = ""; // the file name

  // Check that the command line is correct
  if (argc != 3)
  {

    printf("usage:  ./scheduling alg input\n");
    printf("alg: the scheduling algorithm: 0, 1, or 2\n");
    printf("input: the processes inut file\n");
    exit(1);
  }

  scheduling = (int)atoi(argv[1]); // the scheduling algorithm

  // Check that the file specified by the user exists and open it
  if (!(fp = fopen(argv[2], "r")))
  {
    printf("Cannot open file %s\n", argv[2]);
    exit(1);
  }

  int count = 0;
  struct Process *processes = readInput(fp, &count);

  // form the output file name
  sprintf(filename, "%d-%s", scheduling, argv[2]);

  fcfs(processes, count, filename);

  // close the processes file
  fclose(fp);
  free(processes);

  return 0;
}
