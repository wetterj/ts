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
  Ant(Instance const &,int,Pheromone const &,int antTo,int nhTimeout,int antID,int seed);
  void solve(int n,vector<Solution> &);
  void greedy(Solution &sol,int ss);

  //vector<Solution*> solutions;
protected:
  int antID;
  Solver solver;
  Schedule schedule;
  ACOBrancher acoBrancher;
  SearchLimit *antLimit;
  SearchLimit *nhLimit;
  SolutionCollector *col;
};


Ant::Ant(Instance const &instance,int phi,Pheromone const &phero,int antTo,int nhTo,int id,int seed) : antID(id), solver("ant"), schedule(instance,solver), acoBrancher(phi,schedule,phero)
{
  antLimit = solver.MakeTimeLimit(antTo);
  nhLimit  = solver.MakeTimeLimit(nhTo);
  col      = schedule.firstSol(solver);
  solver.ReSeed(seed + antID);
}

void Ant::solve(int n,vector<Solution> &solutions) {
  for(int i=0;i<n;++i) {
    bool r = solver.Solve(&acoBrancher, antLimit, col);
    if(r && col->solution_count() == 1) {
      Solution *sol = new Solution();
      schedule.assignmentToSolution(col->solution(0), solutions[n*antID + i]);
    } else {
      cerr << "opps\n";
    }
  }
}

void Ant::greedy(Solution &sol,int n) {
  for(int i=0;i<n;++i) {
    ConstructNeighbour nhSearch(sol.quality+1, sol, acoBrancher, solver);
    bool r = solver.Solve(&nhSearch, col, nhLimit);
    if(r && col->solution_count() == 1) {
      schedule.assignmentToSolution( col->solution(0), sol );
      cout << "greedy got " << sol.quality << endl;
    }
  }
}

int main(int argc,char **argv) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  if(argc != 12) {
    cerr << "usage: aco <seed> <instance> <timeout> <ant-timeout> <phi> <n-ants> <rho> <n-greedy> <nhTimeout> <nhSearches> <outfile>\n";
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
  int64 nGreedy    = atoi(argv[8]);
  int64 nhTimeout  = atoi(argv[9]);
  int64 nhSearches = atoi(argv[10]);

  Pheromone pheromone(instance);

  Ant ant(instance,phi,pheromone,anttimeout,nhTimeout,0,seed);

  vector<Solution> solutions(nAnts);
  //solutions.reserve(nAnts);
  Solution best;
  best.quality = -1;

  auto start = high_resolution_clock::now();

  while(duration_cast<milliseconds>(high_resolution_clock::now() - start).count() < timeout) {
    solutions.resize(nAnts);
    // TODO: run the ants in different threads
    ant.solve(nAnts,solutions);
    //solutions.insert(solutions.begin(), ant.solutions.begin(), ant.solutions.end());
    if(best.quality >= 0)
      solutions.push_back(best);
    //ant.solutions.clear();
    sort(solutions.begin(), solutions.end(),
         [](Solution const &l, Solution const &r) { return l.quality > r.quality; });
    // apply greedy to the best nGreedy solutions
    for(int i=0;i<nGreedy && i<solutions.size();++i) {
      ant.greedy(solutions[i], nhSearches);
    }
    // update best solution
    for(auto &s : solutions) {
      if(s.quality > best.quality) {
        best = s;
        auto now = high_resolution_clock::now();
        auto r = results.add_point();
        r->set_time( duration_cast<microseconds>(now - start).count() / 1000000.f );
        r->set_fails( 0 );
        r->set_qual( best.quality );
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
    for(auto &sol : solutions)
      for(int tele=0;tele<instance.nTelescopes;++tele)
        for(int slot=0;slot<instance.nSlots;++slot) {
          int targ = sol.target[ slot*instance.nTelescopes + tele ];
          pheromone.updatePeromone( tele, slot, targ,
                                    pheromone.getPheromone( tele, slot, targ ) + rho * (float) sol.quality / (float) best.quality );
        }

    // increase for best solution
    if(best.quality > 0)
      for(int tele=0;tele<instance.nTelescopes;++tele)
        for(int slot=0;slot<instance.nSlots;++slot) {
          int targ = best.target[ slot*instance.nTelescopes + tele ];
          pheromone.updatePeromone( tele, slot, targ,
                                    pheromone.getPheromone( tele, slot, targ ) + rho );
        }

    //for(auto s : solutions) delete s;
    //solutions.clear();
  }

  fstream output(argv[8], ios::out | ios::trunc | ios::binary);
  results.SerializeToOstream(&output);
  output.close();

  //for(auto a : ants) delete a;

  return 0;
}
