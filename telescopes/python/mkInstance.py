#! /usr/bin/python

import instance_pb2
import sys
import random

if len(sys.argv) != 16:
  print 'usage:  <script> seed nTelescopes nTargets horizon minGain maxGain minCadence maxCadence minPeriod maxPeriod percentVis visSize balanceNumerator balanceDenominator output'
  sys.exit(1)

random.seed(int(sys.argv[1]))

inst = instance_pb2.InstanceProto()
inst.nTelescopes = int(sys.argv[2])
inst.nTargets = int(sys.argv[3])
inst.horizon = int(sys.argv[4])

minGain = int(sys.argv[5])
maxGain = int(sys.argv[6])

minCadence = int(sys.argv[7])
maxCadence = int(sys.argv[8])

minPeriod = int(sys.argv[9])
maxPeriod = int(sys.argv[10])

percentOffset = int(sys.argv[11])
offsetSize    = int(sys.argv[12])

inst.balanceNumerator   = int(sys.argv[13])
inst.balanceDenominator = int(sys.argv[14])

rawGains=[]
for target in range(inst.nTargets):
  rawGains.append( random.randrange(minGain, maxGain+1) )
rawPeriods=[]
for target in range(inst.nTargets):
  rawPeriods.append( random.randrange(minPeriod, maxPeriod+1) )

for telescope in range(inst.nTelescopes):
  for target in range(inst.nTargets):
    g = rawGains[target]
    inst.gain.append( max( minGain, min( maxGain, g + random.randrange( int(-0.1 * g), int(0.1 * g)+1 ) ) ) )

for telescope in range(inst.nTelescopes):
  for target in range(inst.nTargets):
    p = rawPeriods[target]
    inst.period.append( max( minPeriod, min( maxGain, p + random.randrange( int(-0.1 * p), int(0.1 * p)+1 ) ) ) )

for target in range(inst.nTargets):
  inst.cadence.append( random.randrange(minCadence, maxCadence+1) )

for target in range(inst.nTargets):
  # is this target offset?
  if random.randrange(0,100) < percentOffset:
    # is it offset in the evening?
    if random.randrange(0,2) == 0:
      inst.visFrom.append( random.randrange( 1, offsetSize+1 ) )
      inst.visTo.append(inst.horizon)
    # then it must be morning
    else:
      inst.visTo.append( inst.horizon - random.randrange( 1, offsetSize+1 ) )
      inst.visFrom.append(0)
  else:
    inst.visFrom.append(0)
    inst.visTo.append(inst.horizon)

inst.nSlots = int(round(float(inst.horizon) / min( inst.period )))

f = open(sys.argv[15], "wb")
f.write(inst.SerializeToString())
f.close()
