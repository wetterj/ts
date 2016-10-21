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
      fracs.append(float(r[s].closed.fails) / float(b.closed.fails))
  return sum(fracs) / float(len(fracs))

def closedTime(s):
  fracs=[]
  for (i,(b,r)) in results.iteritems():
    if b.HasField("closed") and r[s].HasField("closed"):
      fracs.append(float(r[s].closed.time) / float(b.closed.time))
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
    print "looking at " + str(i)
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
  return r.point[-1].fails

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
  return r.point[-1].time

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
    # TODO: Take an average if needed
    solverResultsFile = lines[idx].strip()
    idx=idx+1
    solverResults = results_pb2.Results()
    with open(solverResultsFile, "rb") as f:
      solverResults.ParseFromString(f.read())

    if len(baseLineR.point) > 0:
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

outfile = open(sys.argv[1] + '/closed-fails-data','w')
outfile.write('base')
for s in solvers:
  outfile.write(' ' + s)
outfile.write('\n')
for (i,(b,rs)) in results.iteritems():
  if b.HasField('closed'):
    outfile.write(str(b.closed.fails))

    for s in solvers:
      outfile.write(' ' + str(rs[s].closed.fails))
    outfile.write('\n')
outfile.close()

outfile = open(sys.argv[1] + '/closed-fails-plot','w')
outfile.write('library("ggplot2")\n\n')
outfile.write('d <- read.csv("' + sys.argv[1] + '/closed-fails-data"' + ', sep=" ")\n')
outfile.write('plt <- ggplot(d) + scale_x_log10() + scale_y_log10() + geom_abline() + geom_point(aes(x=base, y=ttf, col="upper bound q")) + geom_point(aes(x=base, y=fft, col="upper bound x")) + geom_point(aes(x=base, y=ttt, col="both"))\n')
outfile.write('ggsave("' + sys.argv[1] + '/closed-fails.pdf", plt)\n')
outfile.close()
