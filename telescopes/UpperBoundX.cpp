#include "UpperBoundX.hpp"

using namespace std;
using namespace operations_research;

UpperBoundX::UpperBoundX(Schedule &s,bool o) : on(o), schedule(s) {
  if(!o) return;

  //cout << "instance data\n";
  //cout << "teles: " << s.instance.nTelescopes << endl;
  //cout << "targs: " << s.instance.nTargets << endl;
  //cout << "gains:\n";
  //for(int tele=0;tele<s.instance.nTelescopes;++tele) {
  //  for(int targ=0;targ<s.instance.nTargets;++targ)
  //    cout << s.instance.getGain(tele,targ) << " ";
  //  cout << endl;
  //}
  //cout << "period:\n";
  //for(int tele=0;tele<s.instance.nTelescopes;++tele) {
  //  for(int targ=0;targ<s.instance.nTargets;++targ)
  //    cout << s.instance.getPeriod(tele,targ) << " ";
  //  cout << endl;
  //}
  //cout << "cadence:\n";
  //for(int targ=0;targ<s.instance.nTargets;++targ)
  //  cout << s.instance.getCadence(targ) << " ";
  //cout << endl;

  supportingTarget.resize(schedule.instance.nTelescopes*schedule.instance.nSlots);

  // Set the initial supports
  for(int64 tele=0;tele<schedule.instance.nTelescopes;++tele) {
    int64 bestT = 0;
    int64 gain = schedule.instance.getGain(tele,bestT);
    for(int64 targ=1;targ<schedule.instance.nTargets;++targ)
      if(schedule.instance.getGain(tele,targ) > gain) {
        bestT = targ;
        gain = schedule.instance.getGain(tele,targ);
      }

    for(int64 slot=0;slot<schedule.instance.nSlots;++slot)
      setSupport(tele,slot,bestT);
  }
}

void UpperBoundX::prop(int64 tar,int64 time,Solver &solver) {
  if(!on) return;

  //cout << "checking xUpper\n";
  //schedule.printCurrentState();

  // Target was just set to be observed at time, prop on everything within range using it as a support
  for(int64 tele=0;tele<schedule.instance.nTelescopes;++tele) {
    if(schedule.nextSlot[tele] < 0) continue;
    for(int64 slot=schedule.nextSlot[tele]; slot < schedule.instance.nSlots; ++slot) {
      auto latest = schedule.getTime( slot, tele )->Max();
      if(latest >= time + schedule.instance.getCadence(tar) || latest > schedule.instance.horizon)
        break;

      if(getSupport(tele,slot) == tar)
        checkLoc(tele,slot,latest,solver);
    }
  }

  //for(int64 tele=0;tele<schedule.instance.nTelescopes;++tele) {
  //  for(int64 slot=schedule.nextSlot[tele]; slot < schedule.instance.nSlots; ++slot) {
  //    cout << getSupport(tele,slot) << " ";
  //  }
  //  cout << endl;
  //}
  //cout << "done\n";
  //schedule.printCurrentState();
}

void UpperBoundX::checkLoc(int64 tele,int64 slot,int64 latest,Solver &solver) {
  //cout << "checking tele " << tele << " slot " << slot << endl;
  int64 bestObs = 0;
  int64 newSupport = 0;
  int64 bTarg = 0;
  auto it = schedule.getTarget(slot,tele)->MakeDomainIterator(false);
  it->Init();
  while(it->Ok()) {
    auto s = it->Value();
    auto c = schedule.lastTime[s];
    auto cadence = schedule.instance.getCadence(s);
    auto diff = latest - c;
    if(c < 0 || diff >= cadence) {
      //cout << s << " ~> " << schedule.instance.getGain(tele,s) << endl;
      if(bestObs < schedule.instance.getGain(tele,s)) {
        bTarg = s;
        bestObs = schedule.instance.getGain(tele,s);
        newSupport = s;
      }
    }
    else {
      float scale = max(0.f, (float) (cadence - abs(cadence - diff)) / (float) cadence);
      //cout << s << " ~> " << scale*schedule.instance.getGain(tele,s) << endl;
      if(bestObs < scale*schedule.instance.getGain(tele,s)) {
        bTarg = s;
        bestObs = scale*schedule.instance.getGain(tele,s);
        newSupport = s;
      }
    }
    it->Next();
  }
  delete it;
  saveSupport(tele,slot,solver);
  setSupport(tele,slot,newSupport);
  //cout << "best was " << bTarg << endl;
  if(schedule.getContribution(slot,tele)->Max() > bestObs) {
    //cout << "changed bound " << schedule.getContribution(slot,tele)->Max() << " to " << bestObs << endl;
    schedule.getContribution(slot,tele)->SetMax(bestObs+1);
  }
  //schedule.getContribution(slot,tele)->SetMax(bestObs);
}

int64 UpperBoundX::getSupport(int64 telescope,int64 slot) {
  return supportingTarget[telescope*schedule.instance.nSlots + slot];
}

void UpperBoundX::setSupport(int64 telescope,int64 slot,int64 target) {
  supportingTarget[telescope*schedule.instance.nSlots + slot] = target;
}

void UpperBoundX::saveSupport(int64 telescope,int64 slot,Solver &s) {
  s.SaveValue( &supportingTarget[telescope*schedule.instance.nSlots + slot] );
}
