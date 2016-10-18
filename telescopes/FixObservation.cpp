
#include "FixObservation.hpp"

using namespace std;
using namespace operations_research;

int64 selectRandomValue(const IntVar* v) {
  const uint64 span = v->Max() - v->Min() + 1;
  const uint64 size = v->Size();
  Solver* const s = v->solver();
  if (size > span / 4) {
    for (;;) {
      const int64 value = v->Min() + s->Rand64(span);
      if (v->Contains(value)) {
        return value;
      }
    }
  } else {
    int64 index = s->Rand64(size);
    if (index <= size / 2) {
      for (int64 i = v->Min(); i <= v->Max(); ++i) {
        if (v->Contains(i)) {
          if (--index == 0) {
            return i;
          }
        }
      }
      CHECK_LE(index, 0);
    } else {
      for (int64 i = v->Max(); i > v->Min(); --i) {
        if (v->Contains(i)) {
          if (--index == 0) {
            return i;
          }
        }
      }
      CHECK_LE(index, 0);
    }
  }
  return 0LL;
}

CheckBound::CheckBound(Instance const &instance,bool o) {
  on = o;
  preFixQual = 0;
  if(!on) return;
  for(int target=0;target<instance.nTargets;++target)
    evals.emplace_back( target, (float)instance.maxGainTarg(target) / (float)instance.minPeriodTarg(target) );

  sort( evals.begin(), evals.end(),
        [](Eval const &l, Eval const &r) { return l.qual > r.qual; } );
}

void CheckBound::prop(Schedule &schedule,Solver &solver,int contrib) {
  if(!on || schedule.quality->Bound()) return;
  // Save prefix quality
  solver.SaveValue( &preFixQual );
  // update prefix quality
  preFixQual += contrib;
  // Computer upper bound
  // The total time remaining for observations, and the earliest availble time
  float timeRemaining=0.f;
  int64 earliest=schedule.instance.horizon;
  for(int tele=0;tele<schedule.instance.nTelescopes;++tele) {
    auto nextTime = schedule.instance.horizon;
    if(schedule.nextSlot[tele] >= 0)
      nextTime = schedule.getTime( schedule.nextSlot[tele], tele )->Value();

    timeRemaining += max<int64>(0, schedule.instance.horizon - nextTime);
    earliest = min<int64>(earliest, nextTime);
  }
  int idx=0;
  float upperBound = 0.f;
  while(timeRemaining > 0. && idx < evals.size()) {
    int targ = evals[idx].target;
    int idealCadence = schedule.instance.getCadence(targ);
    float initObs=1.f;
    //if(schedule.lastTime[targ] >= 0) {
    //  int actualC = earliest - schedule.lastTime[targ];
    //  initObs = (float) (idealCadence - abs(idealCadence - actualC)) / idealCadence;
    //}
    float nObs = min(timeRemaining, initObs + (float) (schedule.instance.getVisTo(targ) - earliest) / (float) idealCadence );
    timeRemaining -= nObs * schedule.instance.minPeriodTarg(targ);
    upperBound    += nObs * schedule.instance.maxGainTarg(targ);
    ++idx;
  }
  //if(schedule.quality->Max() > preFixQual + (int64) ceil(upperBound) ) {
  //  cout << "change upper bound\n";
  //}
  schedule.quality->SetMax( preFixQual + (int64) ceil(upperBound) );

  //float slotsRemain=0.f;
  //for(int tele=0;tele<schedule.instance.nTelescopes;++tele) {
  //  auto nextTime = schedule.instance.horizon;
  //  if(schedule.nextSlot[tele] >= 0)
  //    nextTime = schedule.getTime( schedule.nextSlot[tele], tele )->Value();

  //  slotsRemain += (float) max<int64>(0, schedule.instance.horizon - nextTime) / (float) schedule.instance.minPeriodTele(tele);
  //}
  //idx = 0;
  //int64 upperBound2 = 0;
  //while(slotsRemain > 0. && idx < evals.size()) {
  //  int64 targ = evals[idx].target;
  //  float nObs = min(slotsRemain, (float) (schedule.instance.getVisTo(targ) - earliest) / (float) schedule.instance.getCadence(targ));
  //  slotsRemain -= nObs;
  //  upperBound2 += nObs * schedule.instance.maxGainTarg(targ);
  //  ++idx;
  //}

  ////if(schedule.quality->Max() > preFixQual + (int64) ceil(upperBound2) ) {
  ////  cout << "change upper bound slots\n";
  ////}
  //schedule.quality->SetMax( preFixQual + upperBound2 );
}

CheckBoundTele::CheckBoundTele(int t,Instance const &instance,bool o) {
  telescope = t;
  on = o;
  preFixQual = 0;
  if(!on) return;
  for(int target=0;target<instance.nTargets;++target)
    evals.emplace_back( target, (float)instance.getGain(telescope,target) / (float)instance.getPeriod(telescope,target) );

  sort( evals.begin(), evals.end(),
        [](Eval const &l, Eval const &r) { return l.qual > r.qual; } );
}

void CheckBoundTele::prop(Schedule &schedule,Solver &solver,int contrib) {
  if(!on || schedule.teleQuality[telescope]->Bound()) return;
  // Save prefix quality
  solver.SaveValue( &preFixQual );
  // update prefix quality
  preFixQual += contrib;
  // Computer upper bound
  // The total time remaining for observations, and the earliest availble time
  float timeRemaining=0.f;
  int64 earliest=schedule.instance.horizon;

  {
    auto nextTime = schedule.instance.horizon;
    if(schedule.nextSlot[telescope] >= 0)
      nextTime = schedule.getTime( schedule.nextSlot[telescope], telescope )->Value();

    timeRemaining += max<int64>(0, schedule.instance.horizon - nextTime);
    earliest = min<int64>(earliest, nextTime);
  }

  int idx=0;
  float upperBound = 0.f;
  while(timeRemaining > 0. && idx < evals.size()) {
    int targ = evals[idx].target;

    int idealCadence = schedule.instance.getCadence(targ);
    float initObs=1.f;
    //if(schedule.lastTime[targ] >= 0) {
    //  int actualC = earliest - schedule.lastTime[targ];
    //  initObs = (float) (idealCadence - abs(idealCadence - actualC)) / idealCadence;
    //}
    float nObs = min(timeRemaining, initObs + (float) (schedule.instance.getVisTo(targ) - earliest) / (float) idealCadence);
    timeRemaining -= nObs * schedule.instance.getPeriod(telescope, targ);
    upperBound    += nObs * schedule.instance.getGain(telescope, targ);
    ++idx;
  }
  schedule.teleQuality[telescope]->SetMax( preFixQual + (int64) ceil(upperBound) );

  //int64 slotsRemain=0;
  //{
  //  auto nextTime = schedule.instance.horizon;
  //  if(schedule.nextSlot[telescope] >= 0)
  //    nextTime = schedule.getTime( schedule.nextSlot[telescope], telescope )->Value();

  //  slotsRemain += (int64) ( (float) max<int64>(0, schedule.instance.horizon - nextTime) / (float) schedule.instance.minPeriodTele(telescope) );
  //}
  //idx = 0;
  //int64 upperBound2 = 0;
  //while(slotsRemain > 0 && idx < evals.size()) {
  //  int64 targ = evals[idx].target;
  //  int64 nObs = max(slotsRemain, (int64) ceil((float) (schedule.instance.getVisTo(targ) - earliest) / (float) schedule.instance.getCadence(targ)) + 1);
  //  slotsRemain -= nObs;
  //  upperBound2 += nObs * schedule.instance.getGain(telescope,targ);
  //  ++idx;
  //}

  //schedule.teleQuality[telescope]->SetMax( preFixQual + upperBound2 );
}

ScheduleBrancher::ScheduleBrancher(Schedule &s,bool o,bool to,bool xo)
: schedule(s), checkBound(s.instance,o), upperBoundX(s,xo) {
  for(int tele=0;tele<s.instance.nTelescopes;++tele) {
    checkBoundTele.emplace_back(tele, s.instance, to);
  }
}

ScheduleBrancher::~ScheduleBrancher() {}

FixObservation::FixObservation(ScheduleBrancher *s,int tar,int tele,int contrib)
: scheduleBrancher(s), target(tar), telescope(tele), contribution(contrib), slot(scheduleBrancher->schedule.nextSlot[telescope]) {
}

void FixObservation::Apply(operations_research::Solver* const s) {
  auto &schedule = scheduleBrancher->schedule;

  //cout << "gonna fix tele: " << telescope << " slot " << slot << " target " << target << endl;
  //schedule.printCurrentState();

  // save the relevant cached info
  s->SaveValue(&schedule.nextSlot[telescope]);
  s->SaveValue(&schedule.lastTele[target]);
  s->SaveValue(&schedule.lastSlot[target]);
  s->SaveValue(&schedule.lastTime[target]);
  s->SaveValue(&schedule.fixedObservations);
  // update the cached info
  schedule.lastTele[target] = telescope;
  schedule.lastSlot[target] = slot;
  schedule.lastTime[target] = schedule.getTime( slot, telescope )->Value();
  ++schedule.nextSlot[telescope];
  ++schedule.fixedObservations;
  // Fix the variable value
  schedule.getTarget( slot, telescope )->SetValue(target);
  schedule.getContribution( slot, telescope )->SetValue(contribution);

  // check the global bound
  scheduleBrancher->checkBound.prop(schedule, *s, contribution);
  // Check bound for each telescope
  scheduleBrancher->checkBoundTele[telescope].prop(schedule, *s, contribution);
  // check bound for each observation
  scheduleBrancher->upperBoundX.prop(target, schedule.lastTime[target], *s);
}

void FixObservation::Refute(operations_research::Solver* const s) {
  // remove the failed value
  scheduleBrancher->schedule.getTarget( slot, telescope )->RemoveValue(target);
}

InOrder::InOrder(Schedule &s,bool on,bool teleOn,bool xOn)
: ScheduleBrancher(s,on,teleOn,xOn) {
}

InOrder::~InOrder() {}

Decision* InOrder::Next(Solver* const s) {
  //schedule.printCurrentState();

  // The telescope
  int tele = schedule.chooseTelescope();
  if(tele < 0) {
    //schedule.printCurrentState();
    return nullptr;
  }

  // the target
  int targ = schedule.getTarget(schedule.nextSlot[tele], tele )->Min();
  int eval = 0;
  //if(schedule.getTime(schedule.nextSlot[tele], tele)->Value() + schedule.instance.getPeriod(tele, targ) < schedule.instance.horizon)
    eval = schedule.evalChoice(tele, targ);
  return s->RevAlloc( new FixObservation(this, targ, tele, eval) );
}

RandomTarget::RandomTarget(Schedule &s,bool on,bool teleOn,bool xOn)
: ScheduleBrancher(s,on,teleOn,xOn) {
}

RandomTarget::~RandomTarget() {}

Decision* RandomTarget::Next(Solver* const s) {
  //schedule.printCurrentState();

  // The telescope
  int tele = schedule.chooseTelescope();
  if(tele < 0) {
    //schedule.printCurrentState();
    return nullptr;
  }

  // the target
  int targ = selectRandomValue( schedule.getTarget(schedule.nextSlot[tele], tele ) );
  int eval = schedule.evalChoice(tele, targ);
  return s->RevAlloc( new FixObservation(this, targ, tele, eval) );
}

BestTarget::Eval::Eval(int t,float e) : target(t), qual(e) {}

BestTarget::BestTarget(Schedule &s,bool on,bool teleOn,bool xOn) : ScheduleBrancher(s,on,teleOn,xOn) {
}

operations_research::Decision* BestTarget::Next(operations_research::Solver* const s) {
  // Choose the telescope
  int tele = schedule.chooseTelescope();
  if(tele < 0)
    return nullptr;

  // Choose the target
  // We don't have the evaluations here yet
  if(evals.size() == schedule.fixedObservations)
    evalTargets(tele);
  else if(evals.size() < schedule.fixedObservations) {
    cerr << "opps\n";
    exit(1);
  }
  while(evals.size() > schedule.fixedObservations+1)
    evals.pop_back();
  // iterate until you find a value that can be used
  for(int jdx=evals.back().size()-1;jdx >= 0;--jdx)
    if(schedule.getTarget( schedule.nextSlot[tele], tele )->Contains( evals.back()[jdx].target )) {
      auto eval = evals.back()[jdx];
      int qual = schedule.evalChoice(tele, eval.target);
      return s->RevAlloc( new FixObservation(this, eval.target, tele, qual ) );
    }
    else
      evals.back().pop_back();
  // We ran out of choices at this slot
  return s->MakeFailDecision();
}

BestTarget::~BestTarget() {}

PrefixQual::PrefixQual(Schedule &sched,bool propQ,bool propQR,bool propX) : BestTarget(sched,propQ,propQR,propX) {
}

PrefixQual::~PrefixQual() {}

bool PrefixQual::evalTargets(int tele) {
  evals.emplace_back();
  auto it = schedule.getTarget( schedule.nextSlot[tele], tele )->MakeDomainIterator( false );
  it->Init();
  while(it->Ok()) {
    auto target = it->Value();
    float eval   = schedule.evalChoice(tele, target);
    evals.back().emplace_back(target, eval);
    it->Next();
  }
  delete it;
  sort( evals.back().begin(), evals.back().end(),
        [](Eval const &l,Eval const &r) { return l.qual < r.qual; } );
}

PrePostQual::PrePostQual(Schedule &s,bool propQ,bool propQr,bool propX) : BestTarget(s,propQ,propQr,propX) {}

PrePostQual::~PrePostQual() {}
  
bool PrePostQual::evalTargets(int tele) {
  evals.emplace_back();
  auto it = schedule.getTarget( schedule.nextSlot[tele], tele )->MakeDomainIterator( false );
  it->Init();
  int minP = schedule.instance.minPeriodTele(tele);
  while(it->Ok()) {
    auto target = it->Value();
    float loss = 0.f;
    if(schedule.lastTime[target] < 0)//schedule.lastTime[target] + schedule.instance.getCadence(target) < schedule.getTime( schedule.nextSlot[tele], tele )->Value()) {
      loss = (float) schedule.instance.maxGainTarg(target) * ((float) minP / (float) schedule.instance.getCadence(target) );
    float eval  = (float) schedule.evalChoice(tele, target) + loss;
    evals.back().emplace_back(target, eval);
    it->Next();
  }
  delete it;
  sort( evals.back().begin(), evals.back().end(),
        [](Eval const &l,Eval const &r) { return l.qual < r.qual; } );
}

class LowerBound : public Decision {
public:
  LowerBound(int64 b,IntVar *v) : bound(b), var(v) {}

  void Apply(Solver* const s) {
    var->SetMin(bound);
  }

  void Refute(Solver* const s) {
    s->Fail();
  }

protected:
  int64 bound;
  IntVar *var;
};

ConstructNeighbour::ConstructNeighbour(int64 bnd,Solution const &sol,BestTarget *b,Solver &solver) 
: bound(bnd), solution(sol), brancher(b)
{
  // select a time uniformly
  time = solver.Rand64( brancher->schedule.instance.horizon );
  boundPosted = false;
  prefixSet = false;
}

operations_research::Decision* ConstructNeighbour::Next(operations_research::Solver* const s) {
  Schedule &schedule = brancher->schedule;
  if(!boundPosted) {
    boundPosted = true;
    return s->RevAlloc( new LowerBound( bound, schedule.quality ) );
  }

  if(!prefixSet) {
    auto tele = schedule.chooseTelescope();
    if(tele<0) return nullptr;
    auto slot = schedule.nextSlot[tele];
    if(slot<0) return nullptr;
    auto now  = schedule.getTime( slot, tele )->Value();
    if(now < time) {
      brancher->evalTargets(tele);
      auto targ = solution.target[ slot*schedule.instance.nTelescopes + tele ];
      auto qual = solution.contribution[ slot*schedule.instance.nTelescopes + tele ];
      return s->RevAlloc( new FixObservation(brancher, targ, tele, qual) );
    }
    prefixSet = true;

    vector<BestTarget::Eval> evals;
    int64 minQual = numeric_limits<int64>::max();
    int64 maxQual = numeric_limits<int64>::min();
    evals.reserve(schedule.instance.nTargets);
    auto it = schedule.getTarget( slot, tele )->MakeDomainIterator( false );
    it->Init();
    while(it->Ok()) {
      auto target = it->Value();
      // Exclude current value
      if(target != solution.target[ slot*schedule.instance.nTelescopes + tele ]) {
        auto eval   = schedule.evalChoice(tele, target);
        evals.emplace_back(target, eval);
        if(eval < minQual)
          minQual = eval;
        if(eval > maxQual)
          maxQual = eval;
      }
      it->Next();
    }
    delete it;
    sort( evals.begin(), evals.end(),
          [](BestTarget::Eval const &l,BestTarget::Eval const &r) { return l.qual > r.qual; } );

    // Assign probabilities
    vector<float> ps;
    ps.reserve(evals.size());
    for(auto const &e : evals)
      ps.push_back( (float) (e.qual - minQual + 1) / (float) (maxQual - minQual + 1) );
    float sum=0.f;
    for(auto p : ps) sum += p;
    // choose a random target
    float c = sum * ((float) s->Rand64( numeric_limits<int64>::max() ) / (float) numeric_limits<int64>::max());
    int idx=0;
    for(;idx < ps.size() && ps[idx] < c;++idx)
      c -= ps[idx];

    brancher->evalTargets(tele);
    return s->RevAlloc( new FixObservation(brancher, evals[idx].target, tele, evals[idx].qual ) );
  }
  return brancher->Next(s);
}

FitnessProp::Eval::Eval(int64 t,int64 e,float q) : target(t), eval(e), fitness(q) {}

FitnessProp::FitnessProp(int p,Schedule &s,bool on,bool teleOn,bool xOn) : ScheduleBrancher(s,on,teleOn,xOn), power(p) {
}

operations_research::Decision* FitnessProp::Next(operations_research::Solver* const s) {
  // Choose the telescope
  int tele = schedule.chooseTelescope();
  if(tele < 0)
    return nullptr;

  // Choose the target
  normaliseFitness(tele);

  float rand = totalFitness * ((float) s->Rand64( numeric_limits<int64>::max() ) / (float) numeric_limits<int64>::max());
  int targ=fitness.begin()->target;
  int eval=fitness.begin()->eval;
  float acc=0.f;
  for(auto it=fitness.begin();it != fitness.end();++it) {
    acc += it->fitness;
    if(rand < acc && schedule.getTarget( schedule.nextSlot[tele], tele )->Contains( it->target )) {
      targ = it->target;
      eval = it->eval;
      break;
    }
  }
  return s->RevAlloc( new FixObservation(this, targ, tele, eval) );
}

void FitnessProp::normaliseFitness(int tele) {
  fitness.clear();

  auto it = schedule.getTarget( schedule.nextSlot[tele], tele )->MakeDomainIterator( false );
  it->Init();
  while(it->Ok()) {
    auto target = it->Value();
    auto eval   = schedule.evalChoice(tele, target);
    fitness.emplace_back(target, eval, (float)eval);
    it->Next();
  }
  delete it;

  int64 minEval = numeric_limits<int64>::max();
  int64 maxEval = numeric_limits<int64>::min();

  for(auto const &e : fitness) {
    minEval = min(minEval, e.eval);
    maxEval = max(maxEval, e.eval);
  }

  for(auto &e : fitness) { 
    e.fitness = (float) (e.eval - minEval + 1) / (float) (maxEval - minEval + 1);
    e.fitness = pow(e.fitness,power);
  }

  totalFitness = 0.f;
  for(auto const &e : fitness)
    totalFitness += e.fitness;
}

FitnessProp::~FitnessProp() {}
