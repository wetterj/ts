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
  if(argc != 7) {
    cerr << "usage: completeSearch <instance> <timeout> [o|r|b] [t|f] [t|f] [t|f]\n";
    exit(1);
  }

  GOOGLE_PROTOBUF_VERIFY_VERSION;

  InstanceProto proto;

  fstream input(argv[1], ios::in | ios::binary);
  if(!proto.ParseFromIstream(&input)) {
    cerr << "Could not parse the protobuf\n";
    exit(1);
  }

  Solver solver("");
  solver.ReSeed(time(NULL));
  Instance instance(proto);
  //for(int tele=0;tele<instance.nTelescopes;++tele) {
  //  for(int slot=0;slot<instance.nSlots;++slot)
  //    cout << instance.getGain(tele, slot) << " ";
  //  cout << endl;
  //}
  Schedule schedule(instance, solver);

  int64 timeout = atoi(argv[2]);
  bool propQual  = (argv[4][0] == 't');
  bool propTQual = (argv[5][0] == 't');
  bool propXQual = (argv[6][0] == 't');
  DecisionBuilder *db = nullptr;
  if(argv[3][0] == 'b')
    db = new BestTarget(schedule,propQual,propTQual,propXQual);
  else if(argv[3][0] == 'o')
    db = new InOrder(schedule,propQual,propTQual,propXQual);
  else
    db = new RandomTarget(schedule,propQual,propTQual,propXQual);

  auto col = schedule.lastSol(solver);
  auto lim = solver.MakeTimeLimit(timeout);

  auto obj = solver.MakeMaximize(schedule.getQuality(),1);

  auto start = high_resolution_clock::now();
  solver.NewSearch(db,col,lim,obj);

  while(solver.NextSolution()) {
    auto now = high_resolution_clock::now();
    cout << "time: " << duration_cast<microseconds>(now - start).count() / 1000000.f << endl;
    cout << "branches: " << solver.branches() << endl;
    cout << "quality: " << schedule.getQuality()->Value() << endl;
  }

  delete db;

  return 0;
}
