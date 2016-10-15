#ifndef UPPERBOUNDX_H_
#define UPPERBOUNDX_H_

#include "Schedule.hpp"

class UpperBoundX {
public:
  UpperBoundX(Schedule &,bool);
  // We just set target to be observed at time, update unset observations that may be affected
  void prop(int64 tar,int64 time,operations_research::Solver &);
protected:
  void checkLoc(int64 tele,int64 slot,int64 latest,operations_research::Solver &);
  int64 getSupport(int64 telescope,int64 slot);
  void setSupport(int64 telescope,int64 slot,int64 target);
  void saveSupport(int64 telescope,int64 slot,operations_research::Solver &);

  bool on;
  Schedule &schedule;
  // The target that supports the upperbound on each x variable
  std::vector<int64> supportingTarget;
};

#endif
