#  The Computer Language Benchmarks Game
#  http://shootout.alioth.debian.org/
#
#  contributed by Karl von Laudermann
#  modified by Jeremy Echols

size = 600 # ARGV[0].to_i

puts "P4\n#{size} #{size}"

ITER = 49                           # Iterations - 1 for easy for..in looping
LIMIT_SQUARED = 4.0                 # Presquared limit

byte_acc = 0
bit_num = 0

count_size = size - 1               # Precomputed size for easy for..in looping

# For..in loops are faster than .upto, .downto, .times, etc.
for y in 0..count_size
  for x in 0..count_size
    zr = 0.0
    zi = 0.0
    cr = (2.0*x/size)-1.5
    ci = (2.0*y/size)-1.0
    escape = false

    # To make use of the for..in code, we use a dummy variable,
    # like one would in C
    for dummy in 0..ITER
      tr = zr*zr - zi*zi + cr
      ti = 2*zr*zi + ci
      zr, zi = tr, ti

      if (zr*zr+zi*zi) > LIMIT_SQUARED
        escape = true
        break
      end
    end

    byte_acc = (byte_acc << 1) | (escape ? 0b0 : 0b1)
    bit_num += 1

    # Code is very similar for these cases, but using separate blocks
    # ensures we skip the shifting when it's unnecessary, which is most cases.
    if (bit_num == 8)
      print byte_acc.chr
      byte_acc = 0
      bit_num = 0
    elsif (x == count_size)
      byte_acc <<= (8 - bit_num)
      print byte_acc.chr
      byte_acc = 0
      bit_num = 0
    end
  end
end
