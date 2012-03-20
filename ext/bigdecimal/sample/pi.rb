#!/usr/local/bin/ruby

#
# pi.rb
#
# Calculates 3.1415.... (the number of times that a circle's diameter
# will fit around the circle) using J. Machin's formula.
#

require "bigdecimal"
require "bigdecimal/math.rb"

include BigMath

if ARGV.size == 1
    print "PI("+ARGV[0]+"):\n"
    p PI(ARGV[0].to_i)
else
    print "TRY: ruby pi.rb 1000 \n"
end
