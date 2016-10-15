
#include "Solution.hpp"

using namespace std;

Solution::Solution() {
  quality = 0;
}

Solution::Solution(SolutionProto const &proto) {
  quality = proto.quality();
  target.reserve(proto.target_size());
  for(auto x : proto.target())
    target.push_back(x);
  time.reserve(proto.time_size());
  for(auto x : proto.time())
    time.push_back(x);
  contribution.reserve(proto.contribution_size());
  for(auto x : proto.contribution())
    contribution.push_back(x);
}
