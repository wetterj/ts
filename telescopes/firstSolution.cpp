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

int main(int argc,char **argv) {
  if(argc != 6 && argc != 7) {
    cerr << "usage: firstSolution <seed> <instance> <timeout> [o|r|b|f <power>] <outfile>\n";
    exit(1);
  }

  GOOGLE_PROTOBUF_VERIFY_VERSION;

  InstanceProto proto;
  Results results;

  fstream input(argv[2], ios::in | ios::binary);
  if(!proto.ParseFromIstream(&input)) {
    cerr << "Could not parse the protobuf\n";
    exit(1);
  }
  input.close();

  Solver *solver = new Solver("");
  solver->ReSeed(atoi(argv[1]));
  Instance instance(proto);
  Schedule schedule(instance, *solver);

  int64 timeout = atoi(argv[3]);
  int power = 1;
  int offset = argv[4][0] == 'f' ? 1 : 0;

  DecisionBuilder *db = nullptr;

  if(argv[4][0] == 'b')
    db = new PrefixQual(schedule,true,true,true);
  else if(argv[4][0] == 'p')
    db = new PrePostQual(schedule,true,true,true);
  else if(argv[4][0] == 'o')
    db = new InOrder(schedule,true,true,true);
  else if(argv[4][0] == 'f') {
    power = atoi(argv[5]);
    db = new FitnessProp(power,schedule,true,true,true);
  } else
    db = new RandomTarget(schedule,true,true,true);

  auto col = schedule.firstSol(*solver);
  auto lim = solver->MakeTimeLimit(timeout);

  auto start = high_resolution_clock::now();
  solver->NewSearch(db,col,lim);

  if(solver->NextSolution()) {
    auto now = high_resolution_clock::now();
    auto r = results.add_point();
    r->set_time( duration_cast<microseconds>(now - start).count() / 1000000.f );
    r->set_fails( solver->failures() );
    r->set_qual( schedule.getQuality()->Value() );
    cout << schedule.getQuality()->Value() << endl;
  }
  fstream output(argv[6+offset], ios::out | ios::trunc | ios::binary);
  results.SerializeToOstream(&output);
  output.close();

  delete solver;
  delete db;

  return 0;
}
