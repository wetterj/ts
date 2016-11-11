import sys
import results_pb2

# this will be (instance,withBT,nh,phi) -> [results]
results={}
nAnts=set()
rhos=set()
instances=set()
lines = sys.stdin.readlines()

idx=0
# read the results
while idx < len(lines):
  words = lines[idx].strip().split()
  n=float(words[0])
  rho=float(words[1])
  inst=words[2]
  nAnts.add(n)
  rhos.add(rho)
  instances.add(inst)
  rs=[]
  for rf in words[3:]:
    rb = results_pb2.Results()
    with open(rf, "rb") as f:
      rb.ParseFromString(f.read())
      rs.append(rb)
  results[(inst,n,rho)] = rs
  idx=idx+1

outfile = open(sys.argv[1] + '/aco-heatmap','w')
outfile.write('nAnts rho')
for inst in range(len(instances)):
  outfile.write(' ' + 'inst' + str(inst))
outfile.write('\n')

for n in nAnts:
  for rho in rhos:
    outfile.write(str(n) + ' ' + str(rho))
    for inst in instances:
      rs=[]
      for r in results[(inst,n,rho)]:
        rs.append(r.point[-1].qual)
      outfile.write(' ' + str( float(sum(rs)) / float(len(rs)) ) )
    outfile.write('\n')
outfile.close()
