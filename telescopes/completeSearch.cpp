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
  if(argc != 8 && argc != 9) {
    cerr << "usage: completeSearch <instance> <timeout> [o|r|b|f <power>] [t|f] [t|f] [t|f] <outfile>\n";
    exit(1);
  }

  GOOGLE_PROTOBUF_VERIFY_VERSION;

  InstanceProto proto;
  Results results;

  fstream input(argv[1], ios::in | ios::binary);
  if(!proto.ParseFromIstream(&input)) {
    cerr << "Could not parse the protobuf\n";
    exit(1);
  }
  input.close();

  Solver solver("");
  solver.ReSeed(time(NULL));
  Instance instance(proto);
  //for(int tele=0;tele<instance.nTelescopes;++tele) {
  //  for(int targ=0;targ<instance.nTargets;++targ)
  //    cout << instance.getGain(tele, targ) << " ";
  //  cout << endl;
  //}
  //cout << endl;
  //for(int tele=0;tele<instance.nTelescopes;++tele) {
  //  for(int targ=0;targ<instance.nTargets;++targ)
  //    cout << instance.getPeriod(tele, targ) << " ";
  //  cout << endl;
  //}
  //cout << endl;
  //for(int targ=0;targ<instance.nTargets;++targ)
  //  cout << instance.getCadence(targ) << " ";
  //cout << endl;
  //cout << endl;
  Schedule schedule(instance, solver);

  int64 timeout = atoi(argv[2]);
  int power = 1;
  int offset = argv[3][0] == 'f' ? 1 : 0;
  bool propQual  = (argv[4+offset][0] == 't');
  bool propTQual = (argv[5+offset][0] == 't');
  bool propXQual = (argv[6+offset][0] == 't');

  DecisionBuilder *db = nullptr;

  if(argv[3][0] == 'b')
    db = new PrefixQual(schedule,propQual,propTQual,propXQual);
  else if(argv[3][0] == 'p')
    db = new PrePostQual(schedule,propQual,propTQual,propXQual);
  else if(argv[3][0] == 'o')
    db = new InOrder(schedule,propQual,propTQual,propXQual);
  else if(argv[3][0] == 'f') {
    power = atoi(argv[4]);
    db = new FitnessProp(power,schedule,propQual,propTQual,propXQual);
  } else
    db = new RandomTarget(schedule,propQual,propTQual,propXQual);

  auto col = schedule.lastSol(solver);
  auto lim = solver.MakeTimeLimit(timeout);

  auto obj = solver.MakeMaximize(schedule.getQuality(),1);

  auto start = high_resolution_clock::now();
  solver.NewSearch(db,col,lim,obj);

  while(solver.NextSolution()) {
    auto now = high_resolution_clock::now();
    auto r = results.add_point();
    r->set_time( duration_cast<microseconds>(now - start).count() / 1000000.f );
    r->set_fails( solver.failures() );
    r->set_qual( schedule.getQuality()->Value() );
  }
  if(!lim->Check()) {
    auto now = high_resolution_clock::now();
    auto c = results.mutable_closed();
    c->set_time(duration_cast<microseconds>(now - start).count() / 1000000.f);
    c->set_fails(solver.failures());
  }
  fstream output(argv[7+offset], ios::out | ios::trunc | ios::binary);
  results.SerializeToOstream(&output);
  output.close();

  delete db;

  return 0;
}
