#ifndef PHEROMONE_H
#define PHEROMONE_H

#include "Instance.hpp"

class Pheromone {
public:
  Pheromone(Instance const &);

  float getPheromone(int telescope,int slot,int target) const;
  void updatePeromone(int telescope,int slot,int target,float);
protected:
  Instance const &instance;
  std::vector<std::vector<std::vector<float>>> pheromone;
};

#endif
