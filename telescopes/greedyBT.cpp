#include <fstream>
#include <chrono>

#include "constraint_solver/constraint_solver.h"

#include "results.pb.h"
#include "instance.pb.h"
#include "Schedule.hpp"
#include "FixObservation.hpp"

using namespace std;
using namespace operations_research;
using namespace chrono;

// A greedy search without backtracking
int main(int argc,char **argv) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  if(argc != 8) {
    cerr << "usage: greedy <seed> <instance> <timeout> <phi> <init-search-timeout> <nh-timeout> <outfile>\n";
    exit(1);
  }


  InstanceProto proto;
  Results results;

  fstream input(argv[2], ios::in | ios::binary);
  if(!proto.ParseFromIstream(&input)) {
    cerr << "Could not parse the protobuf\n";
    exit(1);
  }

  Solver solver("");
  Instance instance(proto);
  Schedule schedule(instance, solver);

  int64 seed    = atoi(argv[1]);
  int64 timeout = atoi(argv[3]);
  int64 phi     = atoi(argv[4]);
  int64 inittimeout = atoi(argv[5]);
  int64 nhtimeout = atoi(argv[6]);

  solver.ReSeed(seed);

  FitnessProp db(phi,schedule,true,true,true);

  auto col = schedule.lastSol(solver);
  auto nhlim = solver.MakeTimeLimit(nhtimeout);
  auto firstlim = solver.MakeTimeLimit(inittimeout);

  auto obj = solver.MakeMaximize(schedule.getQuality(),1);

  auto start = high_resolution_clock::now();
  bool r = solver.Solve(&db,col,firstlim,obj);

  Solution sol;
  while(duration_cast<milliseconds>(high_resolution_clock::now() - start).count() < timeout) {
    if(r && col->solution_count() == 1) {
      schedule.assignmentToSolution( col->solution(0), sol );
      auto now = high_resolution_clock::now();
      auto r = results.add_point();
      r->set_time( duration_cast<microseconds>(now - start).count() / 1000000.f );
      r->set_fails( solver.failures() );
      r->set_qual( sol.quality );
      //cout << sol.quality << endl;
    }
    ConstructNeighbour nhSearch(sol.quality+1, sol, db, solver);

    r = solver.Solve(&nhSearch, col, nhlim, obj);
  }
  fstream output(argv[7], ios::out | ios::trunc | ios::binary);
  results.SerializeToOstream(&output);
  output.close();

  return 0;
}
