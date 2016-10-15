#ifndef INSTANCE_H
#define INSTANCE_H

#include <vector>

#include "base/integral_types.h"

#include "instance.pb.h"
#include "Instance.hpp"

// Instance data
class Instance {
public:
  Instance(InstanceProto const &);
  ~Instance();

  // Targets are labelled 0 to nTargets-1.
  const int64 nTargets;
  const int64 nTelescopes;
  const int64 nSlots;

  const int64 horizon;
  const int64 balanceNumerator;
  const int64 balanceDenominator;

  int64 getGain(int64,int64) const;
  int64 getCadence(int64) const;
  int64 getPeriod(int64,int64) const;
  int64 getVisFrom(int64) const;
  int64 getVisTo(int64) const;

  std::vector<int64> getGainTele(int64) const;
  std::vector<int64> getPeriodTele(int64) const;
  std::vector<int64> getCadence() const;
  std::vector<int64> getVisFrom() const;
  std::vector<int64> getVisTo() const;

  int64 minGain() const;
  int64 maxGain() const;
  int64 minGainTele(int64) const;
  int64 maxGainTele(int64) const;
  int64 minGainTarg(int64) const;
  int64 maxGainTarg(int64) const;

  int64 minPeriodTele(int64) const;
  int64 maxPeriodTele(int64) const;
  int64 minPeriodTarg(int64) const;
  int64 maxPeriodTarg(int64) const;

private:
  std::vector<int64> gain;
  std::vector<int64> cadence;
  std::vector<int64> period;
  std::vector<int64> visFrom;
  std::vector<int64> visTo;
};

#endif
