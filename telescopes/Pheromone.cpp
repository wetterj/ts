#include "Pheromone.hpp"

Pheromone::Pheromone(Instance const &instance) : instance(instance) {
  pheromone.resize(instance.nTelescopes);
  for(auto &tele : pheromone) {
    tele.resize(instance.nSlots);
    for(auto &slot : tele) {
      slot.resize(instance.nTargets);
      for(auto &targ : slot)
        targ = 1.f;
    }
  }
}

float Pheromone::getPheromone(int telescope,int slot,int target) const {
  return pheromone[telescope][slot][target];
}

void Pheromone::updatePeromone(int telescope,int slot,int target,float val) {
  pheromone[telescope][slot][target] = val;
}
