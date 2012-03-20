# from http://www.bagley.org/~doug/shootout/bench/random/random.ruby

IM = 139968.0
IA = 3877.0
IC = 29573.0

$last = 42.0

def gen_random(max)
  (max * ($last = ($last * IA + IC) % IM)) / IM
end

N = 3_000_000

i=0
while i<N
  i+=1
  gen_random(100.0)
end
# "%.9f" % gen_random(100.0)
