# Plenty Thtread.pass
# A performance may depend on GVL implementation.

tmax = (ARGV.shift || 2).to_i
lmax = 200_000 / tmax

(1..tmax).map{
  Thread.new{
    lmax.times{
      Thread.pass
    }
  }
}.each{|t| t.join}


