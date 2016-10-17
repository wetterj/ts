import sys
import numpy as np

nums=[]
for n in sys.argv[1:]:
  nums.append(float(n))

a = np.array(nums)
print str(np.std(a))
