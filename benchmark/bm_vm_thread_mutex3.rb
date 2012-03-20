# 1000 threads, one mutex

require 'thread'
m = Mutex.new
r = 0
max = 2000
(1..max).map{
  Thread.new{
    i=0
    while i<max
      i+=1
      m.synchronize{
        r += 1
      }
    end
  }
}.each{|e|
  e.join
}
raise r.to_s if r != max * max
