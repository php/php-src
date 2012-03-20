
class C
  def m
    1
  end
end

class CC < C
  def m
    super()
  end
end

obj = CC.new

i = 0
while i<6_000_000 # benchmark loop 2
  obj.m
  i+=1
end
