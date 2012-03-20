i = 0
obj1 = Object.new
obj2 = Object.new

while i<30_000_000 # while loop 1
  i+= 1
  obj1 != obj2
end
