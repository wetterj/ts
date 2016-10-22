import sys
import results_pb2
import numpy as np

# input should be
#instance-name
#base-line results
#[solver identifier
# solver results]*
#

# this will be instance -> (baseline, solver -> [result])
results={}
solvers=set()
lines = sys.stdin.readlines()

# A list of the instances the base case can close
def baseClosed():
  closed=[]
  for (i,(r,_)) in results.iteritems():
    if r.HasField("closed"):
      closed.append(i)
  return closed

idx=0
while idx < len(lines):
  # read an instance
  instanceName = lines[idx].strip()
  print "reading " + instanceName
  idx=idx+1
  instanceBaseLineFile = lines[idx].strip()
  idx=idx+1
  baseLineR = results_pb2.Results()
  with open(instanceBaseLineFile, "rb") as f:
    baseLineR.ParseFromString(f.read())

  if len(baseLineR.point) > 0:
    results[instanceName] = (baseLineR,{})

  # read all alternative methods results
  while idx < len(lines) and lines[idx].strip() != '':
    solverName = lines[idx].strip()
    solvers.add(solverName)
    idx=idx+1
    sFiles = lines[idx].strip().split()[1:]
    sResults = []
    for f in sFiles:
      solverResults = results_pb2.Results()
      with open(f, "rb") as f:
        solverResults.ParseFromString(f.read())
      sResults.append(solverResults)
    idx=idx+1

    if len(baseLineR.point) > 0:
      results[instanceName][1][solverName] = sResults

  while idx < len(lines) and lines[idx].strip() == '':
    idx=idx+1

# For the closed instance, plot the base nodes vs other nodes
outfile=open(sys.argv[1] + '/closed-fails','w')
outfile.write('bestResult')
for s in solvers:
  outfile.write('\t' + s + 'Mean\t' + s + 'std')
outfile.write('\n')
for (i,(b,rs)) in results.iteritems():
  if b.HasField('closed'):
    outfile.write(str(b.closed.fails))
    for s in solvers:
      sRs=[]
      for x in rs[s]:
        sRs.append(x.closed.fails)
      outfile.write('\t' + str( float(sum(sRs))/ float(len(sRs)) ))
      arr = np.array(sRs)
      outfile.write('\t' + str( np.std(arr) ) )
    outfile.write('\n')
outfile.close()

## Count the closed instances
#closeFile=open(sys.argv[1] + '/stats',"w")
#closeFile.write("solver\tn-Closed\tclose-fails\tclosed-time\tbest-q\tbest-q-improve\tfails-to-base-q\ttime-to-base-q\n")
#closeFile.write("baseLine\t" + str(baseClosed()) + "\t1.0\t1.0\t" + str(baseBestQ()) + "\t1.0\t1.0\t1.0\n")
#for s in solvers:
#  closeFile.write(s + '\t' + str(nClosed(s)) + "\t" + str(closedFails(s)) + "\t" + str(closedTime(s)) + "\t" + str(bestQ(s)) + "\t" + str(bestQImprove(s)) + "\t" + str(failsToBaseQ(s)) + "\t" + str(timeToBaseQ(s)) + "\n")
#
#closeFile.close()
