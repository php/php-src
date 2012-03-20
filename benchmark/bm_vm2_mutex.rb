require 'thread'

m = Mutex.new

i=0
while i<6_000_000 # benchmark loop 2
  i+=1
  m.synchronize{}
end
