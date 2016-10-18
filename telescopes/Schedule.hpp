#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <vector>

#include "constraint_solver/constraint_solver.h"

#include "Instance.hpp"
#include "Solution.hpp"

// The schedule being constructed
class Schedule {
public:
  // Create the model
  Schedule(Instance const &,operations_research::Solver &);

  operations_research::IntVar *getTarget(int,int);
  operations_research::IntVar *getTime(int,int);
  operations_research::IntVar *getContribution(int,int);

  operations_research::IntExpr *getTargetGain(int,int);
  operations_research::IntExpr *getTargetCadence(int,int);
  operations_research::IntExpr *getTargetPeriod(int,int);
  operations_research::IntExpr *getTargetVisFrom(int,int);
  operations_research::IntExpr *getTargetVisTo(int,int);

  operations_research::IntVar *getQuality();

  operations_research::SolutionCollector *firstSol(operations_research::Solver &);
  operations_research::SolutionCollector *lastSol(operations_research::Solver &);

  void assignmentToSolution(operations_research::Assignment *,Solution &);

  friend class FixObservation;
  friend class ScheduleBrancher;
  friend class InOrder;
  friend class RandomTarget;
  friend class BestTarget;
  friend class PrefixQual;
  friend class PrePostQual;
  friend class FitnessProp;
  friend class ACOBrancher;
  friend class CheckBound;
  friend class CheckBoundTele;
  friend class UpperBoundX;
  friend class ConstructNeighbour;

protected:
  int chooseTelescope();
  // What would be the gain contribution of target targ observed next on telescope tele
  int evalChoice(int tele,int targ);
  void printCurrentState();

  Instance const &instance;
  /*************************************
   * Decision Variables
   *************************************/
  // The target observed at each slot
  std::vector<operations_research::IntVar*> x;

  /*************************************
   * Auxilairy Variables
   *************************************/
  // The time each observation begins
  std::vector<operations_research::IntVar*> w;
  // The contribution each observation makes to the quality
  std::vector<operations_research::IntVar*> c;
  // The total quality of observations on each telescope
  std::vector<operations_research::IntVar*> teleQuality;
  // The total quality of the schedule
  operations_research::IntVar* quality;
  // The propeties of the target at slot/tele
  std::vector<operations_research::IntExpr*> targetGain;
  std::vector<operations_research::IntExpr*> targetCadence;
  std::vector<operations_research::IntExpr*> targetPeriod;
  std::vector<operations_research::IntExpr*> targetFrom;
  std::vector<operations_research::IntExpr*> targetTo;


  /*************************************
   * Useful data to cache during search
   *************************************/
  // The last scheduled telescope/slot/start time for each target (-1 if not seen)
  std::vector<int> lastTele;
  std::vector<int> lastSlot;
  std::vector<int> lastTime;
  // The next unbound slot for each telescope
  std::vector<int> nextSlot;
  int fixedObservations;
};

#endif
