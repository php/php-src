#import sys
#sys.setrecursionlimit(1000)

def factL(n):
	r = 1
	for x in range(2, n):
		r *= x
	return r

def factR(n):
	if n < 2:
		return 1
	else:
		return n * factR(n-1)

for i in range(10000):
	factR(100)

