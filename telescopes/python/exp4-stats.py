import sys
import results_pb2

# this will be (instance,withBT,nh,phi) -> [results]
results={}
initTs=set()
CRs=set()
instances=set()
lines = sys.stdin.readlines()

idx=0
# read the results
while idx < len(lines):
  words = lines[idx].strip().split()
  initT=float(words[0])
  cr=float(words[1])
  inst=words[2]
  initTs.add(initT)
  CRs.add(cr)
  instances.add(inst)
  rs=[]
  for rf in words[3:]:
    rb = results_pb2.Results()
    with open(rf, "rb") as f:
      rb.ParseFromString(f.read())
      rs.append(rb)
  results[(inst,initT,cr)] = rs
  idx=idx+1

outfile = open(sys.argv[1] + '/sa-heatmap','w')
outfile.write('initTemp coolingRate')
for inst in range(len(instances)):
  outfile.write(' ' + 'inst' + str(inst))
outfile.write('\n')

for initT in initTs:
  for cr in CRs:
    outfile.write(str(initT) + ' ' + str(cr))
    for inst in instances:
      rs=[]
      for r in results[(inst,initT,cr)]:
        rs.append(r.point[-1].qual)
      outfile.write(' ' + str( float(sum(rs)) / float(len(rs)) ) )
    outfile.write('\n')
outfile.close()
