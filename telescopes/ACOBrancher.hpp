#ifndef ACO_BRANCHER_H
#define ACO_BRANCHER_H

#include <stack>
#include <list>

#include "FixObservation.hpp"
#include "Pheromone.hpp"

class ACOBrancher : public ScheduleBrancher {
public:
  struct Eval {
    Eval(int t,int e,float f) : target(t), eval(e), fitness(f) {}
    Eval() : target(0), eval(0), fitness(0.f) {}

    int64 target;
    int64 eval;
    float fitness;
  };

  ACOBrancher(int,Schedule &,Pheromone const &);
  ~ACOBrancher();
  operations_research::Decision* Next(operations_research::Solver* const s);
  void reportRefute();
protected:
  void normaliseFitness(int);

  int power;
  Pheromone const &pheromone;
  std::stack<float> totalEval;
  std::stack<std::vector<Eval>, 
             std::vector<std::vector<Eval>>> evals;
};

#endif
