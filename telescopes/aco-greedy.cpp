#include <fstream>
#include <chrono>
#include <thread>

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

Ant::Ant(Instance const &instance,int power,Pheromone const &phero,int to,int nhto,int antID) : solver("ant"), schedule(instance,solver), acoBrancher(power,schedule,phero), greedyBrancher(schedule,true,true,true)
{
  initLimit = solver.MakeTimeLimit(to);
  nhLimit   = solver.MakeTimeLimit(nhto);
  obj = solver.MakeMaximize(schedule.getQuality(),1);
  col = schedule.lastSol(solver);
  firstSol = schedule.firstSol(solver);
  solver.ReSeed(time(NULL) + antID);
}

// run the solver n times
void Ant::solve(int n) {
  for(int i=0;i<n;++i) {
    //bool r = solver.Solve(&acoBrancher, initLimit, obj, col);
    //if(r && col->solution_count() == 1) {
    //  solutions.push_back( new Solution() );
    //  schedule.assignmentToSolution(col->solution(0), *solutions.back());
    //  cout << "got " << solutions.back()->quality << endl;
    //}
    bool r = solver.Solve(&acoBrancher, initLimit, firstSol);
    if(r && firstSol->solution_count() == 1) {
      solutions.push_back( new Solution() );
      schedule.assignmentToSolution(firstSol->solution(0), *solutions.back());
      cout << "got " << solutions.back()->quality << endl;
    }
  }
}

// search its greedy iterations on sol
void Ant::greedy(Solution &sol, int its) {
  bool r = false;
  int i=0;
  while(i < its) {
    //cout << sol.quality << endl;
    //cout << sol.target[0] << endl;
    ConstructNeighbour nhSearch(sol.quality+1, sol, &greedyBrancher, solver);

    r = solver.Solve(&nhSearch, col, nhLimit, obj);
    ++i;

    if(r && col->solution_count() == 1) {
      schedule.assignmentToSolution( col->solution(0), sol );
      cout << "nh got " << sol.quality << endl;
    }
  }
}

int main(int argc,char **argv) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  if(argc != 9) {
    cerr << "usage: aco-greedy <instance> <timeout> <init-timeout> <nh-timeout> <power> <n-ants> <rho> <n-threads>\n";
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
  int64 initimeout = atoi(argv[3]);
  int64 nhtimeout  = atoi(argv[4]);
  int64 pow        = atoi(argv[5]);
  int64 nAnts      = atoi(argv[6]);
  float rho        = atof(argv[7]);
  int64 nThreads   = atoi(argv[8]);

  Pheromone pheromone(instance);

  vector<Ant*> ants;
  ants.reserve(nThreads);
  for(int a=0;a<nThreads;++a)
    ants[a] = new Ant(instance,pow,pheromone,initimeout,nhtimeout,a);

  vector<Solution*> solutions;
  solutions.reserve(nAnts);
  Solution best;
  best.quality = -1;

  auto start = high_resolution_clock::now();

  vector<thread> threads;

  while(duration_cast<milliseconds>(high_resolution_clock::now() - start).count() < timeout) {
    // Run the ants
    for(int thread=0;thread < nThreads;++thread) {
      threads.emplace_back( &Ant::solve, ants[thread], (int) ceil((float) nAnts/nThreads) );
    }
    for(int thread=0;thread < nThreads;++thread) {
      threads[thread].join();
      solutions.insert(solutions.end(), ants[thread]->solutions.begin(), ants[thread]->solutions.end());
      ants[thread]->solutions.clear();
    }
    threads.clear();
    if(best.quality > 0) solutions.push_back(new Solution(best));
    // apply the greedy nh to the best solutions
    sort(solutions.begin(), solutions.end(),
         [](Solution const *l, Solution const *r) { return l->quality > r->quality; });

    if(solutions.size() > 0) {
      for(int thread=0;thread < nThreads && thread < solutions.size();++thread) {
        threads.emplace_back( &Ant::greedy, ants[thread], ref(*solutions[thread]), 10 );
      }
      for(int thread=0;thread < nThreads;++thread) {
        threads[thread].join();
      }
      threads.clear();
    }
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
    for(auto sol : solutions) {
      for(int tele=0;tele<instance.nTelescopes;++tele) {
        for(int slot=0;slot<instance.nSlots;++slot) {
          int targ = sol->target[ slot*instance.nTelescopes + tele ];
          pheromone.updatePeromone( tele, slot, targ,
                                    pheromone.getPheromone( tele, slot, targ ) + (float) sol->quality / (float) best.quality );
        }
      }
    }

    //// increase for best solution
    //if(best.quality > 0)
    //  for(int tele=0;tele<instance.nTelescopes;++tele)
    //    for(int slot=0;slot<instance.nSlots;++slot) {
    //      int targ = best.target[ slot*instance.nTelescopes + tele ];
    //      pheromone.updatePeromone( tele, slot, targ,
    //                                pheromone.getPheromone( tele, slot, targ ) + 1.f );
    //    }

    for(auto s : solutions) delete s;
    solutions.clear();

    //for(int64 tele=0;tele < instance.nTelescopes;++tele) {
    //  cout << "tele " << tele << endl;
    //  for(int64 slot=0;slot < instance.nSlots;++slot) {
    //    cout << "slot " << slot << endl;
    //    for(int64 target=0;target < instance.nTargets;++target)
    //      cout << pheromone.getPheromone(tele, slot, target) << " ";
    //    cout << endl;
    //  }
    //  cout << endl;
    //}
  }

  for(auto a : ants) delete a;

  return 0;
}
