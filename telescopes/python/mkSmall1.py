#! /usr/bin/python

import instance_pb2
import sys
import random

inst = instance_pb2.InstanceProto()
inst.nTelescopes = 1
inst.nTargets = 10
inst.nSlots = 5
inst.horizon = 5

for telescope in range(inst.nTelescopes):
  for target in range(inst.nTargets):
    inst.gain.append( random.randrange(5,20) )
    inst.cadence.append( random.randrange(2,5) )
    inst.period.append( 1 )
    inst.visFrom.append( 0 )
    inst.visTo.append( 5 )

inst.balanceNumerator = 1
inst.balanceDenominator = 2

f = open("small1", "wb")
f.write(inst.SerializeToString())
f.close()
