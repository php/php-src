# from http://www.bagley.org/~doug/shootout/bench/sieve/sieve.ruby
num = 500
count = i = j = 0
flags0 = Array.new(8192,1)
k = 0
while k < num
  k+=1
  count = 0
  flags = flags0.dup
  i = 2
  while i<8192
    i+=1
    if flags[i]
      # remove all multiples of prime: i
      j = i*i
      while j < 8192
        j += i
        flags[j] = nil
      end
      count += 1
    end
  end
end
count
