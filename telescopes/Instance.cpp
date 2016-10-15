#include <limits>
#include <algorithm>

#include "Instance.hpp"

using namespace std;

Instance::Instance(InstanceProto const &proto) 
: nTelescopes(proto.ntelescopes()), nTargets(proto.ntargets()), nSlots(proto.nslots()), horizon(proto.horizon()), balanceNumerator(proto.balancenumerator()), balanceDenominator(proto.balancedenominator())
{
  gain.reserve(proto.gain_size());
  for(auto const g : proto.gain()) {
    assert(g >= 0);
    gain.push_back(g);
  }
  assert(gain.size() == nTelescopes*nTargets);

  cadence.reserve(proto.cadence_size());
  for(auto const c : proto.cadence()) {
    assert(c > 0);
    cadence.push_back(c);
  }
  assert(cadence.size() == nTargets);

  period.reserve(proto.period_size());
  for(auto const p : proto.period()) {
    assert(p > 0);
    period.push_back(p);
  }
  assert(period.size() == nTelescopes*nTargets);

  visFrom.reserve(proto.visfrom_size());
  for(auto const v : proto.visfrom()) {
    assert(v >= 0 && v < horizon);
    visFrom.push_back(v);
  }
  assert(visFrom.size() == nTargets);
  
  visTo.reserve(proto.visto_size());
  for(auto const v : proto.visto()) {
    assert(v > 0 && v <= horizon);
    visTo.push_back(v);
  }
  assert(visTo.size() == nTargets);
}

Instance::~Instance() {
}

int64 Instance::getGain(int64 telescope,int64 target) const {
  assert(telescope >= 0 && telescope < nTelescopes && target >= 0 && target < nTargets);
  return gain[telescope*nTargets + target];
}

int64 Instance::getCadence(int64 target) const {
  assert(target >= 0 && target < nTargets);
  return cadence[target];
}

int64 Instance::getPeriod(int64 telescope,int64 target) const {
  assert(telescope >= 0 && telescope < nTelescopes && target >= 0 && target < nTargets);
  return period[telescope*nTargets + target];
}

int64 Instance::getVisFrom(int64 target) const {
  assert(target >= 0 && target < nTargets);
  return visFrom[target];
}

int64 Instance::getVisTo(int64 target) const {
  assert(target >= 0 && target < nTargets);
  return visTo[target];
}

vector<int64> Instance::getGainTele(int64 tele) const {
  vector<int64> gs;
  gs.reserve(nTargets);
  for(int target=0;target<nTargets;++target)
    gs.push_back( getGain(tele, target) );
  return gs;
}

vector<int64> Instance::getPeriodTele(int64 tele) const {
  vector<int64> ps;
  ps.reserve(nTargets);
  for(int target=0;target<nTargets;++target)
    ps.push_back( getPeriod(tele, target) );
  return ps;
}

vector<int64> Instance::getCadence() const {
  vector<int64> cs(cadence.begin(), cadence.end());
  return cs;
}

vector<int64> Instance::getVisFrom() const {
  vector<int64> vs(visFrom.begin(), visFrom.end());
  return vs;
}

vector<int64> Instance::getVisTo() const {
  vector<int64> vs(visTo.begin(), visTo.end());
  return vs;
}

int64 Instance::minGain() const {
  return *min_element(gain.begin(), gain.end());
}

int64 Instance::maxGain() const {
  return *max_element(gain.begin(), gain.end());
}

int64 Instance::minGainTele(int64 tele) const {
  int64 mn = numeric_limits<int64>::max();
  for(int t=0;t<nTargets;++t)
    if(getGain(tele,t) < mn)
      mn = getGain(tele,t);
  return mn;
}

int64 Instance::maxGainTele(int64 tele) const {
  int64 mx = numeric_limits<int64>::min();
  for(int t=0;t<nTargets;++t)
    if(getGain(tele,t) > mx)
      mx = getGain(tele,t);
  return mx;
}

int64 Instance::minGainTarg(int64 targ) const {
  int64 mn = numeric_limits<int64>::max();
  for(int t=0;t<nTelescopes;++t)
    if(getGain(t,targ) < mn)
      mn = getGain(t,targ);
  return mn;
}

int64 Instance::maxGainTarg(int64 targ) const {
  int64 mx = numeric_limits<int64>::min();
  for(int t=0;t<nTelescopes;++t)
    if(getGain(t,targ) > mx)
      mx = getGain(t,targ);
  return mx;
}

int64 Instance::minPeriodTele(int64 tele) const {
  int64 mn = numeric_limits<int64>::max();
  for(int t=0;t<nTargets;++t)
    if(getPeriod(tele,t) < mn)
      mn = getPeriod(tele,t);
  return mn;
}

int64 Instance::maxPeriodTele(int64 tele) const {
  int64 mx = numeric_limits<int64>::min();
  for(int t=0;t<nTargets;++t)
    if(getPeriod(tele,t) > mx)
      mx = getPeriod(tele,t);
  return mx;
}

int64 Instance::minPeriodTarg(int64 targ) const {
  int64 mn = numeric_limits<int64>::max();
  for(int t=0;t<nTelescopes;++t)
    if(getPeriod(t,targ) < mn)
      mn = getPeriod(t,targ);
  return mn;
}

int64 Instance::maxPeriodTarg(int64 targ) const {
  int64 mx = numeric_limits<int64>::min();
  for(int t=0;t<nTelescopes;++t)
    if(getPeriod(t,targ) > mx)
      mx = getPeriod(t,targ);
  return mx;
}
