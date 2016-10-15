#include <limits>

#include "ACOBrancher.hpp"

using namespace std;
using namespace operations_research;

ACOBrancher::ACOBrancher(int pw,Schedule &s,Pheromone const &p) 
: power(pw), ScheduleBrancher(s,true,true,true), pheromone(p) {
  evals.emplace();
  totalEval.emplace(0.f);
}

ACOBrancher::~ACOBrancher() {}

operations_research::Decision* ACOBrancher::Next(operations_research::Solver* const s) {
  while(evals.size() != 0) {
    // Choose the telescope
    int tele = schedule.chooseTelescope();
    if(tele < 0) {
      //schedule.printCurrentState();
      return nullptr;
    }

    // Choose the target
    auto &currentEvals = evals.top();
    // We don't have the evaluations here yet, build them
    if(currentEvals.size() == 0) {
      // First use the base heuristic
      auto it = schedule.getTarget( schedule.nextSlot[tele], tele )->MakeDomainIterator( false );
      it->Init();
      while(it->Ok()) {
        auto target = it->Value();
        auto eval   = schedule.evalChoice(tele, target);
        currentEvals.emplace_back(target, eval, (float)eval);
        it->Next();
      }
      delete it;
    }
    if(currentEvals.size() > 0) {
      normaliseFitness(tele);

      //cout << "tele " << tele << " slot " << schedule.nextSlot[tele] << endl;
      //cout << "t vals: ";
      //float acc1=0.f;
      //for(auto &e : currentEvals) {
      //  acc1 += e.fitness;
      //  cout << e.target << "-" << e.eval << "-" << acc1 << " ";
      //}
      //cout << "\n";

      float rand = totalEval.top() * ((float) s->Rand64( numeric_limits<int64>::max() ) / (float) numeric_limits<int64>::max());
      //cout << "tot is " << totalEval.top() << " rand is  " << rand << endl;
      int targ=currentEvals.begin()->target;
      int eval=currentEvals.begin()->eval;
      float acc=0.f;
      for(auto it=currentEvals.begin();it != currentEvals.end();++it) {
        acc += it->fitness;
        if(rand < acc && schedule.getTarget( schedule.nextSlot[tele], tele )->Contains( it->target )) {
          targ = it->target;
          eval = it->eval;
          totalEval.top() -= it->fitness;
          currentEvals.erase( it );
          break;
        }
      }
      //cout << "depth " << evals.size() << " target " << targ << endl;
      //schedule.printCurrentState();
      evals.emplace();
      totalEval.emplace(0.f);
      //cout << "choose " << targ << endl;
      return s->RevAlloc( new FixObservation(this, targ, tele, eval) );
    }

    // We ran out of choices at this slot
    reportRefute();
  }
  //schedule.printCurrentState();
  return nullptr;
}

void ACOBrancher::reportRefute() {
  evals.pop();
  totalEval.pop();
}

void ACOBrancher::normaliseFitness(int tele) {
  if(evals.size() == 0) return;

  auto &currentEvals = evals.top();

  int64 minEval = numeric_limits<int64>::max();
  int64 maxEval = numeric_limits<int64>::min();

  for(auto const &e : currentEvals) {
    minEval = min(minEval, e.eval);
    maxEval = max(minEval, e.eval);
  }

  for(auto &e : currentEvals) { 
    e.fitness = (float) (e.eval - minEval + 1) / (float) (maxEval - minEval + 1);
    e.fitness = pow(e.fitness,power);
  }

  // Now multiply by the phermone
  for(auto &e : currentEvals) {
    e.fitness *= pheromone.getPheromone( tele, schedule.nextSlot[tele], e.target );
  }

  totalEval.top() = 0.f;
  for(auto &e : currentEvals) totalEval.top() += e.fitness;
}

