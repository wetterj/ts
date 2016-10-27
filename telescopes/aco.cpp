#include <fstream>
#include <chrono>

#include "constraint_solver/constraint_solver.h"

#include "instance.pb.h"
#include "Schedule.hpp"
#include "ACOBrancher.hpp"

using namespace std;
using namespace operations_research;
using namespace chrono;

class Ant {
public:
  Ant(Instance const &,int,Pheromone const &,int to,int nhTo,int antID);
  void solve(int n);
  void greedy(Solution &,int);

  vector<Solution*> solutions;
protected:
  Solver solver;
  Schedule schedule;
  ACOBrancher acoBrancher;
  BestTarget greedyBrancher;
  SearchLimit *initLimit;
  SearchLimit *nhLimit;
  OptimizeVar *obj;
  SolutionCollector *col;
  SolutionCollector *firstSol;
};


Ant::Ant(Instance const &instance,int power,Pheromone const &phero,int to,int nhto,int seed) : solver("ant"), schedule(instance,solver), acoBrancher(power,schedule,phero), greedyBrancher(schedule,true,true,true)
{
  initLimit = solver.MakeTimeLimit(to);
  nhLimit   = solver.MakeTimeLimit(nhto);
  obj = solver.MakeMaximize(schedule.getQuality(),1);
  col = schedule.lastSol(solver);
  firstSol = schedule.firstSol(solver);
  solver.ReSeed(seed);
}


Solution *Ant::solve() {
  bool r = solver.Solve(&acoBrancher, limit, obj, col);
  if(r && col->solution_count() == 1) {
    Solution *sol = new Solution();
    schedule.assignmentToSolution(col->solution(0), *sol);
    cout << "got " << sol->quality << endl;
    return sol;
  }
  return nullptr;
}

int main(int argc,char **argv) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  if(argc != 8) {
    cerr << "usage: aco <seed> <instance> <timeout> <ant-timeout> <phi> <n-ants> <rho> <threads>\n";
    exit(1);
  }


  InstanceProto proto;

  fstream input(argv[1], ios::in | ios::binary);
  if(!proto.ParseFromIstream(&input)) {
    cerr << "Could not parse the protobuf\n";
    exit(1);
  }

  Instance instance(proto);

  int64 timeout    = atoi(argv[2]);
  int64 anttimeout = atoi(argv[3]);
  int64 pow        = atoi(argv[4]);
  int64 nAnts      = atoi(argv[5]);
  float rho        = atof(argv[6]);

  Pheromone pheromone(instance);

  Ant ant(instance,phi,pheromone,anttimeout);

  vector<Solution*> solutions;
  solutions.reserve(nAnts);
  Solution best;
  best.quality = -1;

  auto start = high_resolution_clock::now();

  while(duration_cast<milliseconds>(high_resolution_clock::now() - start).count() < timeout) {
    // TODO: run the ants in different threads
    for(int a=0;a<nAnts;++a) {
      auto s = ants[0]->solve();
      if(s != nullptr)
        solutions.push_back(s);
    }
    //// apply the greedy nh to the best solution
    //// Should be in a different function
    //if(solutions.size() > 0) {
    //  int bIdx = 0;
    //  int best = solutions[0]->quality;
    //  for(int i=0;i<solutions.size();++i)
    //    if(solutions[i]->quality > best) {
    //      bIdx = i;
    //      best = solutions[i]->quality;
    //    }
    //  bool r=false;
    //  for(int i=0;i<10;++i) {
    //    if(r && col->solution_count() == 1) {
    //      schedule.assignmentToSolution( col->solution(0), *solutions[bIdx] );
    //    }
    //    ConstructNeighbour nhSearch(solutions[bIdx]->quality+1, solutions[bIdx], &ants[0]->db, ants[0]->solver);

    //    r = ants[0]->solver.Solve(&nhSearch, ants[0]->col, nhlim, obj);
    //  }
    //}
    // update best solution
    for(auto s : solutions)
      if(s->quality > best.quality) {
        best = *s;
        auto now = high_resolution_clock::now();
        cout << "t: " << duration_cast<microseconds>(now - start).count() / 1000000.f << endl;
        cout << "q: " << best.quality << endl;
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

  for(auto a : ants) delete a;

  return 0;
}
