def m &b
  b
end

pr = m{
  a = 1
}

i=0
while i<6_000_000 # benchmark loop 2
  i+=1
  pr.call
end

