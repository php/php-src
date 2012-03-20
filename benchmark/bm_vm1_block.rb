def m
  yield
end

i=0
while i<30_000_000 # while loop 1
  i+=1
  m{
  }
end
