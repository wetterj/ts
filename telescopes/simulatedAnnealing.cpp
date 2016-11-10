#include <fstream>
#include <chrono>
#include <limits>
#include <math.h>

#include "constraint_solver/constraint_solver.h"

#include "results.pb.h"
#include "instance.pb.h"
#include "Schedule.hpp"
#include "FixObservation.hpp"

using namespace std;
using namespace operations_research;
using namespace chrono;

bool accept(int newQ,int oldQ,float tmp) {
  if(newQ > oldQ)
    return true;

  float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
  float p = exp((float) (newQ - oldQ) / tmp);
  return p > r;
}

int main(int argc,char **argv) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  if(argc != 10) {
    cerr << "usage: simulatedAnnealing1 <seed> <instance> <timeout> <init-search-timeout> <nh-timeout> <phi> <init-temp> <cool-rate> <outfile>\n";
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
  solver.ReSeed(atoi(argv[1]));
  Instance instance(proto);
  Schedule schedule(instance, solver);

  int64 timeout = atoi(argv[3]);
  int64 inittimeout = atoi(argv[4]);
  int64 nhtimeout = atoi(argv[5]);
  int64 phi        = atoi(argv[6]);
  float initTemp   = atof(argv[7]);
  float coolRate   = 1.f - atof(argv[8]);
  FitnessProp db(phi,schedule,true,true,true);

  auto col      = schedule.lastSol(solver);
  auto nhcol    = schedule.firstSol(solver);
  auto nhlim    = solver.MakeTimeLimit(nhtimeout);
  auto firstlim = solver.MakeTimeLimit(inittimeout);

  auto obj = solver.MakeMaximize(schedule.getQuality(),1);

  int64 bestQ = numeric_limits<int64>::min();

  auto start = high_resolution_clock::now();
  bool r = solver.Solve(&db,nhcol,firstlim);

  initTemp = initTemp*nhcol->Value(0, schedule.getQuality());

  Solution sol;
  schedule.assignmentToSolution( nhcol->solution(0), sol );
  int iter=0;
  while(duration_cast<milliseconds>(high_resolution_clock::now() - start).count() < timeout) {
    float currentTmp = initTemp * pow(coolRate, iter);
    // do we accept it?
    if(r && nhcol->solution_count() == 1 && accept(nhcol->Value(0, schedule.getQuality()), sol.quality, currentTmp)) {
      schedule.assignmentToSolution( nhcol->solution(0), sol );
      if(sol.quality > bestQ) {
        auto now = high_resolution_clock::now();
        auto r = results.add_point();
        r->set_time( duration_cast<microseconds>(now - start).count() / 1000000.f );
        r->set_fails( solver.failures() );
        r->set_qual( sol.quality );
        //cout << "t: " << duration_cast<microseconds>(now - start).count() / 1000000.f << endl;
        //cout << "q: " << sol.quality << endl;
        bestQ = sol.quality;
      }
    }
    ConstructNeighbour nhSearch(0, sol, db, solver);

    r = solver.Solve(&nhSearch, nhcol, nhlim);
    ++iter;
  }
  fstream output(argv[9], ios::out | ios::trunc | ios::binary);
  results.SerializeToOstream(&output);
  output.close();

  return 0;
}
