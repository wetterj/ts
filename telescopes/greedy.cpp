#include <fstream>
#include <chrono>

#include "constraint_solver/constraint_solver.h"

#include "instance.pb.h"
#include "Schedule.hpp"
#include "FixObservation.hpp"

using namespace std;
using namespace operations_research;
using namespace chrono;

int main(int argc,char **argv) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  if(argc != 5) {
    cerr << "usage: greedy <instance> <timeout> <init-search-timeout> <nh-timeout>\n";
    exit(1);
  }


  InstanceProto proto;

  fstream input(argv[1], ios::in | ios::binary);
  if(!proto.ParseFromIstream(&input)) {
    cerr << "Could not parse the protobuf\n";
    exit(1);
  }

  Solver solver("");
  solver.ReSeed(time(NULL));
  Instance instance(proto);
  Schedule schedule(instance, solver);

  int64 timeout = atoi(argv[2]);
  int64 inittimeout = atoi(argv[3]);
  int64 nhtimeout = atoi(argv[4]);
  BestTarget db(schedule,true,true,true);

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
      cout << "t: " << duration_cast<microseconds>(now - start).count() / 1000000.f << endl;
      cout << "q: " << sol.quality << endl;
    }
    ConstructNeighbour nhSearch(sol.quality+1, sol, &db, solver);

    r = solver.Solve(&nhSearch, col, nhlim, obj);
  }

  return 0;
}
