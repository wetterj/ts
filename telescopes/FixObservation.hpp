#ifndef FIX_OBSERVATION_H
#define FIX_OBSERVATION_H

#include "constraint_solver/constraint_solver.h"

#include "Schedule.hpp"
#include "UpperBoundX.hpp"

class CheckBound {
public:
  // eval and sort targets
  // Must be called before any of the schedule is fixed
  CheckBound(Instance const &,bool on = true);

  // Save prefix quality
  // update prefix quality
  // Computer upper bound
  void prop(Schedule &,operations_research::Solver &,int contrib);
protected:
  class Eval {
  public:
    Eval(int64 t,float q)
      : target(t), qual(q) {}

    int64 target;
    float qual;
  };
  bool on;
  int preFixQual;
  std::vector<Eval> evals;
};

// The quality bound on a single telescope
class CheckBoundTele {
public:
  // eval and sort targets
  // Must be called before any of the schedule is fixed
  CheckBoundTele(int,Instance const &,bool on = true);

  // Save prefix quality
  // update prefix quality
  // Computer upper bound
  void prop(Schedule &,operations_research::Solver &,int contrib);
protected:
  class Eval {
  public:
    Eval(int64 t,float q)
      : target(t), qual(q) {}

    int64 target;
    float qual;
  };
  bool on;
  int telescope;
  int preFixQual;
  std::vector<Eval> evals;
};

class ScheduleBrancher : public operations_research::DecisionBuilder {
public:
  ScheduleBrancher(Schedule &,bool propQual=true,bool propQualTele=true,bool propX=true);
  virtual ~ScheduleBrancher();
  virtual operations_research::Decision* Next(operations_research::Solver* const s) = 0;
  virtual void reportRefute();
//protected:
  Schedule &schedule;
  CheckBound checkBound;
  std::vector<CheckBoundTele> checkBoundTele;
  UpperBoundX upperBoundX;
};

class FixObservation : public operations_research::Decision {
public:
  FixObservation(ScheduleBrancher *s,int target,int telescope,int contrib);
  void Apply(operations_research::Solver* const s);
  void Refute(operations_research::Solver* const s);

protected:
  ScheduleBrancher *scheduleBrancher;
  int target;
  int telescope;
  int contribution;
  int slot;
};

class InOrder : public ScheduleBrancher {
public:
  InOrder(Schedule &s,bool propQual=true,bool propQualTele=true, bool propX=true);
  ~InOrder();
  operations_research::Decision* Next(operations_research::Solver* const s);
};

class RandomTarget : public ScheduleBrancher {
public:
  RandomTarget(Schedule &s,bool propQual=true,bool propQualTele=true, bool propX=true);
  ~RandomTarget();
  operations_research::Decision* Next(operations_research::Solver* const s);

protected:
};

class BestTarget : public ScheduleBrancher {
public:
  struct Eval {
    Eval(int,int);
    int target;
    int qual;
  };

  BestTarget(Schedule &s,bool propQual=true,bool propQualTele=true, bool propX=true);
  ~BestTarget();
  operations_research::Decision* Next(operations_research::Solver* const s);
  void reportRefute();

protected:
  std::vector<std::vector<Eval>> evals;
};

// Uses the given solution as a prefix, then the given brancher to finish it
class ConstructNeighbour : public operations_research::DecisionBuilder {
public:
  ConstructNeighbour(int64,Solution const &,ScheduleBrancher *,operations_research::Solver &);
  operations_research::Decision* Next(operations_research::Solver* const s);
protected:
  int64 bound;
  Solution const &solution;
  ScheduleBrancher *brancher;
  // The time selected to reschedule
  int64 time;
  // What mode are we in
  bool boundPosted;
  bool prefixSet;
};

#endif
