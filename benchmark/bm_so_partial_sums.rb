n = 2_500_000 # (ARGV.shift || 1).to_i

alt = 1.0 ; s0 = s1 = s2 = s3 = s4 = s5 = s6 = s7 = s8 = 0.0

1.upto(n) do |d|
  d = d.to_f ; d2 = d * d ; d3 = d2 * d ; ds = Math.sin(d) ; dc = Math.cos(d)

  s0 += (2.0 / 3.0) ** (d - 1.0)
  s1 += 1.0 / Math.sqrt(d)
  s2 += 1.0 / (d * (d + 1.0))
  s3 += 1.0 / (d3 * ds * ds)
  s4 += 1.0 / (d3 * dc * dc)
  s5 += 1.0 / d
  s6 += 1.0 / d2
  s7 += alt / d
  s8 += alt / (2.0 * d - 1.0)

  alt = -alt
end

if false
  printf("%.9f\t(2/3)^k\n", s0)
  printf("%.9f\tk^-0.5\n", s1)
  printf("%.9f\t1/k(k+1)\n", s2)
  printf("%.9f\tFlint Hills\n", s3)
  printf("%.9f\tCookson Hills\n", s4)
  printf("%.9f\tHarmonic\n", s5)
  printf("%.9f\tRiemann Zeta\n", s6)
  printf("%.9f\tAlternating Harmonic\n", s7)
  printf("%.9f\tGregory\n", s8)
end
