#!/usr/bin/ruby
#
# The Great Computer Language Shootout
# http://shootout.alioth.debian.org/
#
# nsieve-bits in Ruby
# Contributed by Glenn Parker, March 2005

CharExponent = 3
BitsPerChar = 1 << CharExponent
LowMask = BitsPerChar - 1

def sieve(m)
  items = "\xFF" * ((m / BitsPerChar) + 1)
  masks = ""
  BitsPerChar.times do |b|
    masks << (1 << b).chr
  end

  count = 0
  pmax = m - 1
  2.step(pmax, 1) do |p|
    if items[p >> CharExponent][p & LowMask] == 1
      count += 1
      p.step(pmax, p) do |mult|
	a = mult >> CharExponent
	b = mult & LowMask
	items[a] -= masks[b] if items[a][b] != 0
      end
    end
  end
  count
end

n = 9 # (ARGV[0] || 2).to_i
n.step(n - 2, -1) do |exponent|
  break if exponent < 0
  m = 2 ** exponent * 10_000
  count = sieve(m)
  printf "Primes up to %8d %8d\n", m, count
end

