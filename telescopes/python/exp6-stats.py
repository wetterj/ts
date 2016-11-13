import sys
import results_pb2
import numpy as np

# this will be (instance,withBT,nh,phi) -> [results]
results={}
nLocals=set()
nIters=set()
instances=set()
lines = sys.stdin.readlines()

idx=0
# read the results
while idx < len(lines):
  words = lines[idx].strip().split()
  nLocal=float(words[0])
  nIter=float(words[1])
  inst=words[2]
  nLocals.add(nLocal)
  nIters.add(nIter)
  instances.add(inst)
  rs=[]
  for rf in words[3:]:
    rb = results_pb2.Results()
    with open(rf, "rb") as f:
      rb.ParseFromString(f.read())
      rs.append(rb)
  results[(inst,nLocal,nIter)] = rs
  idx=idx+1

outfile = open(sys.argv[1] + '/aco-greedy-heatmap','w')
outfile.write('nLocal nIter')
for inst in range(len(instances)):
  outfile.write(' ' + 'inst' + str(inst) + 'Mean' + ' inst' + str(inst) + 'StdDev' )
outfile.write('\n')

for n in nLocals:
  for m in nIters:
    outfile.write(str(n) + ' ' + str(m))
    for inst in instances:
      rs=[]
      for r in results[(inst,n,m)]:
        rs.append(r.point[-1].qual)
      outfile.write(' ' + str( float(sum(rs)) / float(len(rs)) ) )
      a = np.array(rs)
      outfile.write(' ' + str( np.std(a) ) )
    outfile.write('\n')
outfile.close()
