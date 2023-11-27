#ifndef __FPROCESS_H_
#define __FPROCESS_H_

struct FProcess
{
  int id;
  int arrivalTime;
  int burstTime;

  FProcess(int id, int arrivalTime, int burstTime): id(id), arrivalTime(arrivalTime), burstTime(burstTime) {}
};

#endif