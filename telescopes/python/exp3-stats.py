import sys
import results_pb2

# this will be (instance,withBT,nh,phi) -> [results]
results={}
nhs=set()
phis=set()
instances=set()
lines = sys.stdin.readlines()

idx=0
# read the greedy results
if not lines[idx].startswith('greedy'):
  sys.exit(1)
idx=idx+1
bt=False
while idx < len(lines):
  if lines[idx].startswith('greedyBT'):
    bt=True
  else:
    words = lines[idx].strip().split()
    nh=int(words[1])
    phi=int(words[2])
    inst=words[3]
    nhs.add(nh)
    phis.add(phi)
    instances.add(inst)
    rs=[]
    for rf in words[4:]:
      rb = results_pb2.Results()
      with open(rf, "rb") as f:
        rb.ParseFromString(f.read())
        rs.append(rb)
    results[(inst,bt,nh,phi)] = rs
  idx=idx+1

outfile = open(sys.argv[1] + '/greedy-heatmap','w')
outfile.write('nh phi')
for bt in ['greedy','greedyBT']:
  for inst in range(len(instances)):
    outfile.write(' ' + 'inst' + str(inst) + '_' + bt)
outfile.write('\n')

for nh in nhs:
  for phi in phis:
    outfile.write(str(nh) + ' ' + str(phi))
    for bt in [False,True]:
      for inst in instances:
        rs=[]
        for r in results[(inst,bt,nh,phi)]:
          rs.append(r.point[-1].qual)
        outfile.write(' ' + str( float(sum(rs)) / float(len(rs)) ) )
    outfile.write('\n')
outfile.close()
