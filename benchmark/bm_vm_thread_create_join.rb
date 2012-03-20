i=0
while i<100_000 # benchmark loop 3
  i+=1
  Thread.new{
  }.join
end
