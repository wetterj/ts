import sys
import results_pb2

# input should be
#instance-name
#base-line results
#[solver identifier
# solver results]*
#
#base-line results
#[solver identifier
# solver results]*
#

# this will be instance -> (baseline, solver -> result)
results={}
solvers=set()
lines = sys.stdin.readlines()

def baseClosed():
  closed=0
  for (i,(r,_)) in results.iteritems():
    if r.HasField("closed"):
      closed=closed+1
  return closed

def nClosed(s):
  closed=0
  for (i,(_,r)) in results.iteritems():
    if r[s].HasField("closed"):
      closed=closed+1
  return closed

def closedFails(s):
  fracs=[]
  for (i,(b,r)) in results.iteritems():
    if b.HasField("closed") and r[s].HasField("closed"):
      fracs.append(float(b.closed.fails) / float(r[s].closed.fails))
  return sum(fracs) / float(len(fracs))

def closedTime(s):
  fracs=[]
  for (i,(b,r)) in results.iteritems():
    if b.HasField("closed") and r[s].HasField("closed"):
      fracs.append(float(b.closed.time) / float(r[s].closed.time))
  return sum(fracs) / float(len(fracs))

def getBestQ(i):
  bestQ=results[i][0].point[-1].qual
  for (_,r) in results[i][1].iteritems():
    if r.point[-1].qual > bestQ:
      bestQ = r.point[-1].qual
  return bestQ

def baseBestQ():
  bestQ=0
  for (i,(r,_)) in results.iteritems():
    if r.point[-1].qual == getBestQ(i):
      bestQ = bestQ+1
  return float(bestQ) / float(len(results))

def bestQ(s):
  bestQ=0
  for (i,(_,r)) in results.iteritems():
    if r[s].point[-1].qual == getBestQ(i):
      bestQ = bestQ+1
  return float(bestQ) / float(len(results))

def bestQImprove(s):
  bestQ=[]
  for (i,(b,r)) in results.iteritems():
    bestQ.append( float(r[s].point[-1].qual) / float(b.point[-1].qual) )
  return sum(bestQ) / float(len(bestQ))

def failsTo(q,r):
  for p in r.point:
    if p.qual >= q:
      return p.fails
  return r[-1].fails

def failsToBaseQ(s):
  fracs=[]
  for (i,(b,r)) in results.iteritems():
    baseR = b.point[-1]
    fracs.append( float(failsTo(baseR.qual,r[s])) / float(baseR.fails) )
  return sum(fracs) / float(len(fracs))

def timeTo(q,r):
  for p in r.point:
    if p.qual >= q:
      return p.time
  return r[-1].time

def timeToBaseQ(s):
  fracs=[]
  for (i,(b,r)) in results.iteritems():
    baseR = b.point[-1]
    fracs.append( float(timeTo(baseR.qual,r[s])) / float(baseR.time) )
  return sum(fracs) / float(len(fracs))

idx=0
while idx < len(lines):
  # read an instance
  instanceName = lines[idx].strip()
  idx=idx+1
  instanceBaseLineFile = lines[idx].strip()
  idx=idx+1
  baseLineR = results_pb2.Results()
  with open(instanceBaseLineFile, "rb") as f:
    baseLineR.ParseFromString(f.read())

  results[instanceName] = (baseLineR,{})

  # read all alternative methods results
  while idx < len(lines) and lines[idx].strip() != '':
    solverName = lines[idx].strip()
    solvers.add(solverName)
    idx=idx+1
    # TODO: Take an average if needed
    solverResultsFile = lines[idx].strip()
    idx=idx+1
    solverResults = results_pb2.Results()
    with open(solverResultsFile, "rb") as f:
      solverResults.ParseFromString(f.read())
    results[instanceName][1][solverName] = solverResults

  while idx < len(lines) and lines[idx].strip() == '':
    idx=idx+1

# Count the closed instances
closeFile=open(sys.argv[1] + '/stats',"w")
closeFile.write("solver\tn-Closed\tclose-fails\tclosed-time\tbest-q\tbest-q-improve\tfails-to-base-q\ttime-to-base-q\n")
closeFile.write("baseLine\t" + str(baseClosed()) + "\t1.0\t1.0\t" + str(baseBestQ()) + "\t1.0\t1.0\t1.0\n")
for s in solvers:
  closeFile.write(s + '\t' + str(nClosed(s)) + "\t" + str(closedFails(s)) + "\t" + str(closedTime(s)) + "\t" + str(bestQ(s)) + "\t" + str(bestQImprove(s)) + "\t" + str(failsToBaseQ(s)) + "\t" + str(timeToBaseQ(s)) + "\n")

closeFile.close()
