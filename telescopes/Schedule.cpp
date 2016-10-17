#include <limits>

#include "Schedule.hpp"

using namespace std;
using namespace operations_research;

// Create the model
Schedule::Schedule(Instance const &i,operations_research::Solver &s) : instance(i) {
  /****************
   * Variables
   ****************/
  // Target target variables
  x.reserve(instance.nSlots*instance.nTelescopes);
  s.MakeIntVarArray(instance.nSlots * instance.nTelescopes, 0, instance.nTargets-1, &x);
  // The time variables
  w.reserve((instance.nSlots+1)*instance.nTelescopes);
  for(int slot=0;slot<=instance.nSlots;++slot)
    for(int tele=0;tele<instance.nTelescopes;++tele)
      w.push_back( s.MakeIntVar(slot*instance.minPeriodTele(tele), slot*instance.maxPeriodTele(tele)) );
  // The contribution variables
  c.reserve(instance.nSlots*instance.nTelescopes);
  for(int slot=0;slot<instance.nSlots;++slot)
    for(int tele=0;tele<instance.nTelescopes;++tele)
      c.push_back( s.MakeIntVar(0, instance.maxGainTele(tele)) );

  // Quality variables
  teleQuality.reserve(instance.nTelescopes);
  for(int tele=0;tele<instance.nTelescopes;++tele)
    teleQuality.push_back( s.MakeIntVar(0, instance.maxGainTele(tele)*instance.nSlots) );
  quality = s.MakeIntVar(0, instance.maxGain()*instance.nSlots*instance.nTelescopes );

  //printCurrentState();

  /****************
   * Constraints
   ****************/
  // Cache the propeties of the observed targets
  targetGain.resize(instance.nSlots*instance.nTelescopes);
  for(int tele=0;tele<instance.nTelescopes;++tele) {
    vector<int64> gs = instance.getGainTele(tele);
    for(int slot=0;slot<instance.nSlots;++slot)
      targetGain[slot*instance.nTelescopes + tele] = s.MakeElement( gs, getTarget( slot, tele ) );
  }

  targetPeriod.resize(instance.nSlots*instance.nTelescopes);
  for(int tele=0;tele<instance.nTelescopes;++tele) {
    vector<int64> ps = instance.getPeriodTele(tele);
    for(int slot=0;slot<instance.nSlots;++slot)
      targetPeriod[slot*instance.nTelescopes + tele] = s.MakeElement( ps, getTarget( slot, tele ) );
  }

  targetCadence.resize(instance.nSlots*instance.nTelescopes);
  vector<int64> cs = instance.getCadence();
  for(int tele=0;tele<instance.nTelescopes;++tele) {
    for(int slot=0;slot<instance.nSlots;++slot)
      targetCadence[slot*instance.nTelescopes + tele] = s.MakeElement( cs, getTarget( slot, tele ) );
  }

  targetFrom.resize(instance.nSlots*instance.nTelescopes);
  vector<int64> fs = instance.getVisFrom();
  for(int tele=0;tele<instance.nTelescopes;++tele) {
    for(int slot=0;slot<instance.nSlots;++slot)
      targetFrom[slot*instance.nTelescopes + tele] = s.MakeElement( fs, getTarget( slot, tele ) );
  }

  targetTo.resize(instance.nSlots*instance.nTelescopes);
  vector<int64> ts = instance.getVisTo();
  for(int tele=0;tele<instance.nTelescopes;++tele) {
    for(int slot=0;slot<instance.nSlots;++slot)
      targetTo[slot*instance.nTelescopes + tele] = s.MakeElement( ts, getTarget( slot, tele ) );
  }

  // Enforce the start times of observations
  for(int slot=1;slot<=instance.nSlots;++slot)
    for(int tele=0;tele<instance.nTelescopes;++tele)
      s.AddConstraint( s.MakeEquality( getTime( slot, tele ),
                                       s.MakeSum( getTime( slot-1, tele ), getTargetPeriod( slot-1, tele ) ) ) );

  // Anything finishing after the horizon has no contribution, and observes target 0
  // But before the horizon, targets must be visible
  // w[slot + 1, tele] > horizon -> (c[slot, tele] = 0 AND x[slot, tele] = 0)
  // w[slot + 1, tele] <= horizon -> (w[slot, tele] >= from[slot, target] AND w[slot, tele] <= to[slot, target]
  for(int slot=0;slot<instance.nSlots;++slot)
    for(int tele=0;tele<instance.nTelescopes;++tele) {
      auto late = s.MakeIsGreaterCstVar( getTime( slot+1, tele ), instance.horizon );
      s.AddConstraint( s.MakeLessOrEqual( late, s.MakeIsEqualCstVar( getContribution( slot, tele ), 0 ) ) );
      s.AddConstraint( s.MakeLessOrEqual( late, s.MakeIsEqualCstVar( getTarget( slot, tele ), 0 ) ) );
      auto notLate = s.MakeSum( s.MakeOpposite( late ), 1 );
      s.AddConstraint( s.MakeLessOrEqual( notLate, s.MakeIsLessOrEqualVar( getTargetVisFrom(slot, tele), getTime(slot, tele) ) ) );
      s.AddConstraint( s.MakeLessOrEqual( notLate, s.MakeIsLessOrEqualVar( getTime(slot, tele), getTargetVisTo(slot, tele) ) ) );
    }

  // Accumulate qualities
  for(int tele=0;tele<instance.nTelescopes;++tele) {
    vector<IntVar*> contribs;
    contribs.reserve(instance.nSlots);
    for(int slot=0;slot<instance.nSlots;++slot) contribs.push_back( getContribution( slot, tele ) );

    s.AddConstraint( s.MakeEquality( teleQuality[tele],
                                     s.MakeSum( contribs ) ) );
  }
  s.AddConstraint( s.MakeEquality( quality,
                                   s.MakeSum( teleQuality ) ) );

  // The balance constraint
  auto maxQual = s.MakeMax( teleQuality );
  auto minQual = s.MakeDiv( s.MakeProd( maxQual, instance.balanceNumerator), instance.balanceDenominator );
  for(int tele=0;tele<instance.nTelescopes;++tele) {
    s.AddConstraint( s.MakeLessOrEqual( minQual, teleQuality[tele] ) );
  }

  /*****************************
   * Initialise the cahced data
   *****************************/
  lastTele.resize(instance.nTargets);
  lastSlot.resize(instance.nTargets);
  lastTime.resize(instance.nTargets);
  for(int t=0;t<instance.nTargets;++t) {
    lastTele[t] = -1;
    lastSlot[t] = -1;
    lastTime[t] = -1;
  }

  nextSlot.resize(instance.nTelescopes);
  for(int t=0;t<instance.nTelescopes;++t) {
    nextSlot[t] = 0;
  }

  fixedObservations = 0;
}

operations_research::IntVar *Schedule::getTarget(int slot,int tele) {
  return x[slot*instance.nTelescopes+tele];
}

operations_research::IntVar *Schedule::getTime(int slot,int tele) {
  return w[slot*instance.nTelescopes+tele];
}

operations_research::IntVar *Schedule::getContribution(int slot,int tele) {
  return c[slot*instance.nTelescopes+tele];
}

operations_research::IntExpr *Schedule::getTargetGain(int slot,int tele) {
  return targetGain[slot*instance.nTelescopes+tele];
}

operations_research::IntExpr *Schedule::getTargetCadence(int slot,int tele) {
  return targetCadence[slot*instance.nTelescopes+tele];
}

operations_research::IntExpr *Schedule::getTargetPeriod(int slot,int tele) {
  return targetPeriod[slot*instance.nTelescopes+tele];
}

operations_research::IntExpr *Schedule::getTargetVisFrom(int slot,int tele) {
  return targetFrom[slot*instance.nTelescopes+tele];
}

operations_research::IntExpr *Schedule::getTargetVisTo(int slot,int tele) {
  return targetTo[slot*instance.nTelescopes+tele];
}

int Schedule::chooseTelescope() {
  //cout << "Choosing tele\n";
  // collect the telescopes that have an unbound slot, with earliest time
  vector<int> teles;
  teles.reserve(instance.nTelescopes);
  int64 earliest = numeric_limits<int64>::max();
  for(int tele=0;tele<instance.nTelescopes;++tele) {
    if(nextSlot[tele] >= instance.nSlots) continue;
    auto time = getTime(nextSlot[tele], tele)->Value();
    if(time >= instance.horizon) continue;
    if(time == earliest) {
      //cout << "tele " << tele << " is equal at " << time << endl;
      teles.push_back(tele);
    }
    else if(time < earliest) {
      //cout << "tele " << tele << " is earlier at " << time << endl;
      teles.clear();
      teles.push_back(tele);
      earliest = time;
    }
  }
  if(teles.size() == 0) return -1;
  // return the telescope with lowest quality so far
  int64 lowest = numeric_limits<int64>::max();
  int64 tele = -1;
  for(auto t : teles) {
    auto q = teleQuality[t]->Min();
    if(q < lowest) {
      tele = t;
      lowest = q;
    }
  }
  return tele;
}

int Schedule::evalChoice(int tele,int targ) {
  int t = getTime( nextSlot[tele], tele )->Value();
  // If the observation will be too late, make it zero
  if(t + instance.getPeriod(tele,targ) > instance.horizon)
    return 0;
  // If it has not been observed, perfect observation
  if(lastTime[targ] < 0)
    return instance.getGain(tele,targ);
  // linearly decrease it otherwise
  int diff = t - lastTime[targ];
  float scale = max(0.f, (float) (instance.getCadence(targ) - abs(instance.getCadence(targ) - diff)) / instance.getCadence(targ));
  return scale*instance.getGain(tele,targ);
}

void Schedule::printCurrentState() {
  cout << "~~~~~~~~~~~~~~~~~~\n";
  cout << "quality: ";
  if(quality->Bound())
    cout << quality->Value() << endl;
  else
    cout << quality->Min() << ".." << quality->Max() << endl;
  for(int tele=0;tele<instance.nTelescopes;++tele)
    cout << "t" << tele << ": " << teleQuality[tele] << endl;
  cout << endl;
  for(int tele=0;tele<instance.nTelescopes;++tele) {
    cout << "x" << tele << ": ";
    for(int slot=0;slot<instance.nSlots;++slot) {
      auto target = getTarget( slot, tele );
      if(target->Bound())
        cout << target->Value() << " ";
      else
        cout << "-1 ";
    }
    cout << endl;
  }
  cout << endl;
  for(int tele=0;tele<instance.nTelescopes;++tele) {
    cout << "w" << tele << ": ";
    for(int slot=0;slot<instance.nSlots;++slot) {
      auto time = getTime( slot, tele );
      if(time->Bound())
        cout << time->Value() << " ";
      else
        cout << "-1 ";
    }
    cout << endl;
  }
  cout << endl;
  for(int tele=0;tele<instance.nTelescopes;++tele) {
    cout << "c" << tele << ": ";
    for(int slot=0;slot<instance.nSlots;++slot) {
      auto cont = getContribution( slot, tele );
      if(cont->Bound())
        cout << cont->Value() << " ";
      else
        cout << cont << " ";
    }
    cout << endl;
  }
  cout << "~~~~~~~~~~~~~~~~~~\n\n";
}

IntVar *Schedule::getQuality() {
  return quality;
}

SolutionCollector *Schedule::firstSol(Solver &solver) {
  auto col = solver.MakeFirstSolutionCollector();
  col->Add(x);
  col->Add(w);
  col->Add(c);
  col->Add(quality);
  return col;
}

SolutionCollector *Schedule::lastSol(Solver &solver) {
  auto col = solver.MakeLastSolutionCollector();
  col->Add(x);
  col->Add(w);
  col->Add(c);
  col->Add(quality);
  return col;
}

void Schedule::assignmentToSolution(operations_research::Assignment *assignment,Solution &solution) {
  solution.quality = assignment->Value(quality);

  solution.target.clear();
  solution.target.reserve(x.size());
  solution.time.clear();
  solution.time.reserve(w.size());
  solution.contribution.clear();
  solution.contribution.reserve(c.size());

  for(auto var : x)
    solution.target.push_back( assignment->Value(var) );
  for(auto var : w)
    solution.time.push_back( assignment->Value(var) );
  for(auto var : c)
    solution.contribution.push_back( assignment->Value(var) );
}
