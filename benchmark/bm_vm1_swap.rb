a = 1
b = 2
i=0
while i<30_000_000 # while loop 1
  i+=1
  a, b = b, a
end

