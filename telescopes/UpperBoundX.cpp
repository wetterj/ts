#include "UpperBoundX.hpp"

using namespace std;
using namespace operations_research;

UpperBoundX::UpperBoundX(Schedule &s,bool o) : on(o), schedule(s) {
  if(!o) return;

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
}

void UpperBoundX::checkLoc(int64 tele,int64 slot,int64 latest,Solver &solver) {
  saveSupport(tele,slot,solver);
  int64 bestObs = 0;
  int64 newSupport = 0;
  auto it = schedule.getTarget(slot,tele)->MakeDomainIterator(false);
  it->Init();
  while(it->Ok()) {
    auto s = it->Value();
    auto c = schedule.lastTime[s];
    auto cadence = schedule.instance.getCadence(s);
    auto diff = latest - c;
    if(c < 0 || diff >= cadence)
      if(bestObs < schedule.instance.getGain(tele,s)) {
        bestObs = schedule.instance.getGain(tele,s);
        newSupport = s;
      }
    else {
      float scale = max(0.f, (float) (cadence - abs(cadence - diff)) / (float) cadence);
      //cout << scale << endl;
      if(bestObs < scale*schedule.instance.getGain(tele,s)) {
        bestObs = scale*schedule.instance.getGain(tele,s);
        newSupport = s;
      }
    }
    it->Next();
  }
  delete it;
  setSupport(tele,slot,newSupport);
  //if(schedule.getContribution(slot,tele)->Max() > bestObs)
  //  cout << "changed bound\n";
  schedule.getContribution(slot,tele)->SetMax(bestObs);
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
