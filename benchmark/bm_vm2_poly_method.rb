class C1
  def m
    1
  end
end
class C2
  def m
    2
  end
end

o1 = C1.new
o2 = C2.new

i=0
while i<6_000_000 # benchmark loop 2
  o = (i % 2 == 0) ? o1 : o2
  o.m; o.m; o.m; o.m; o.m; o.m; o.m; o.m
  i+=1
end
