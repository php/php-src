def fact(n)
  return 1 if n == 0
  f = 1
  n.downto(1) do |i|
    f *= i
  end
  return f
end
print fact(ARGV[0].to_i), "\n"
