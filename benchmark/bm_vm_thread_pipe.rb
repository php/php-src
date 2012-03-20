# Mesure small and plenty pipe read/write.
# A performance may depend on GVL implementation.

lmax = 100_000
r, w = IO.pipe
[Thread.new{
  lmax.times{
    w.write('a')
  }
  p "w:exit"
}, Thread.new{
  lmax.times{
    r.read(1)
  }
  p "r:exit"
}].each{|t| t.join}

