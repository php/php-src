class Object
  define_method(:m){}
end

i=0
while i<6_000_000 # benchmark loop 2
  i+=1
  m; m; m; m; m; m; m; m;
end
