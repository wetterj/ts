import sys

nums=[]
for a in sys.argv[1:]:
  nums.append( float(a) )

print str( sum(nums) / float(len(nums)) )
