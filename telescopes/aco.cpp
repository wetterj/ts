#include <fstream>
#include <chrono>

#include "constraint_solver/constraint_solver.h"

#include "instance.pb.h"
#include "results.pb.h"
#include "Schedule.hpp"
#include "FixObservation.hpp"

using namespace std;
using namespace operations_research;
using namespace chrono;

class Ant {
public:
  Ant(Instance const &,int,Pheromone const &,int antTo,int antID);
  void solve(int n);

  vector<Solution*> solutions;
protected:
  Solver solver;
  Schedule schedule;
  ACOBrancher acoBrancher;
  SearchLimit *antLimit;
  SolutionCollector *col;
};


Ant::Ant(Instance const &instance,int phi,Pheromone const &phero,int antTo,int seed) : solver("ant"), schedule(instance,solver), acoBrancher(phi,schedule,phero)
{
  antLimit = solver.MakeTimeLimit(antTo);
  col      = schedule.firstSol(solver);
  solver.ReSeed(seed);
}

void Ant::solve(int n) {
  for(int i=0;i<n;++i) {
    bool r = solver.Solve(&acoBrancher, antLimit, col);
    if(r && col->solution_count() == 1) {
      Solution *sol = new Solution();
      schedule.assignmentToSolution(col->solution(0), *sol);
      solutions.push_back(sol);
    }
  }
}

int main(int argc,char **argv) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  if(argc != 9) {
    cerr << "usage: aco <seed> <instance> <timeout> <ant-timeout> <phi> <n-ants> <rho> <outfile>\n";
    exit(1);
  }

  InstanceProto proto;
  Results results;

  fstream input(argv[2], ios::in | ios::binary);
  if(!proto.ParseFromIstream(&input)) {
    cerr << "Could not parse the protobuf\n";
    exit(1);
  }

  Instance instance(proto);

  int64 seed       = atoi(argv[1]);
  int64 timeout    = atoi(argv[3]);
  int64 anttimeout = atoi(argv[4]);
  int64 phi        = atoi(argv[5]);
  int64 nAnts      = atoi(argv[6]);
  float rho        = atof(argv[7]);

  Pheromone pheromone(instance);

  Ant ant(instance,phi,pheromone,anttimeout,seed);

  vector<Solution*> solutions;
  //solutions.reserve(nAnts);
  Solution best;
  best.quality = -1;

  auto start = high_resolution_clock::now();

  while(duration_cast<milliseconds>(high_resolution_clock::now() - start).count() < timeout) {
    // TODO: run the ants in different threads
    ant.solve(nAnts);
    solutions.insert(solutions.begin(), ant.solutions.begin(), ant.solutions.end());
    ant.solutions.clear();
    // update best solution
    for(auto s : solutions) {
      if(s->quality > best.quality) {
        best = *s;
        auto now = high_resolution_clock::now();
        cout << "t: " << duration_cast<microseconds>(now - start).count() / 1000000.f << endl;
        cout << "q: " << s->quality << endl;
      }
    }
    // update pheromones
    // evaporate
    for(int tele=0;tele<instance.nTelescopes;++tele)
      for(int slot=0;slot<instance.nSlots;++slot)
        for(int targ=0;targ<instance.nTargets;++targ)
          pheromone.updatePeromone( tele, slot, targ,
                                    pheromone.getPheromone( tele, slot, targ )*(1.f - rho) );
    // increase for solutions
    for(auto sol : solutions)
      for(int tele=0;tele<instance.nTelescopes;++tele)
        for(int slot=0;slot<instance.nSlots;++slot) {
          int targ = sol->target[ slot*instance.nTelescopes + tele ];
          pheromone.updatePeromone( tele, slot, targ,
                                    pheromone.getPheromone( tele, slot, targ ) + rho * (float) sol->quality / (float) best.quality );
        }

    // increase for best solution
    if(best.quality > 0)
      for(int tele=0;tele<instance.nTelescopes;++tele)
        for(int slot=0;slot<instance.nSlots;++slot) {
          int targ = best.target[ slot*instance.nTelescopes + tele ];
          pheromone.updatePeromone( tele, slot, targ,
                                    pheromone.getPheromone( tele, slot, targ ) + rho );
        }

    for(auto s : solutions) delete s;
    solutions.clear();
  }

  //for(auto a : ants) delete a;

  return 0;
}
