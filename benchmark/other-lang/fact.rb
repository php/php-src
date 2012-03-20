def fact(n)
  if n < 2
    1
  else
    n * fact(n-1)
  end
end

i=0
while i<10000
  i+=1
  fact(100)
end
