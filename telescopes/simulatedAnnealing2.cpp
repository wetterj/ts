#include <fstream>
#include <chrono>
#include <limits>
#include <math.h>

#include "constraint_solver/constraint_solver.h"

#include "instance.pb.h"
#include "Schedule.hpp"
#include "FixObservation.hpp"

using namespace std;
using namespace operations_research;
using namespace chrono;

int main(int argc,char **argv) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  if(argc != 7) {
    cerr << "usage: greedy <instance> <timeout> <init-search-timeout> <nh-timeout> <init-temp> <cool-rate>\n";
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
  float initTemp   = atof(argv[5]);
  float coolRate   = 1.f - atof(argv[6]);
  BestTarget db(schedule,true,true,true);

  auto col      = schedule.lastSol(solver);
  auto nhlim    = solver.MakeTimeLimit(nhtimeout);
  auto firstlim = solver.MakeTimeLimit(inittimeout);

  auto obj = solver.MakeMaximize(schedule.getQuality(),1);

  int64 bestQ = numeric_limits<int64>::min();

  auto start = high_resolution_clock::now();
  bool r = solver.Solve(&db,col,firstlim,obj);

  initTemp = initTemp*col->Value(0, schedule.getQuality());

  Solution sol;
  schedule.assignmentToSolution( col->solution(0), sol );
  int iter=0;
  while(duration_cast<milliseconds>(high_resolution_clock::now() - start).count() < timeout) {
    if(r && col->solution_count() == 1) {
      schedule.assignmentToSolution( col->solution(0), sol );
      if(sol.quality > bestQ) {
        auto now = high_resolution_clock::now();
        cout << "t: " << duration_cast<microseconds>(now - start).count() / 1000000.f << endl;
        cout << "q: " << sol.quality << endl;
        bestQ = sol.quality;
      }
    }
    float currentTmp = initTemp * pow(coolRate, iter);
    float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    int64 bound = (int64) (sol.quality + log(r)*currentTmp);
    cout << sol.quality << " " << bound << endl;
    ConstructNeighbour nhSearch(bound, sol, &db, solver);

    r = solver.Solve(&nhSearch, col, nhlim, obj);
    ++iter;
  }

  return 0;
}
