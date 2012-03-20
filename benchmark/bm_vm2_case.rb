i=0
while i<6_000_000 # while loop 2
  case :foo
  when :bar
    raise
  when :baz
    raise
  when :boo
    raise
  when :foo
    i+=1
  end
end

