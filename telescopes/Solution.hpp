#ifndef SOLUTION_H
#define SOLUTION_H

#include <vector>

#include "base/integral_types.h"

#include "solution.pb.h"

struct Solution {
  Solution();
  Solution(SolutionProto const &);

  int64 quality;

  std::vector<int64> target;
  std::vector<int64> time;
  std::vector<int64> contribution;
};

#endif
