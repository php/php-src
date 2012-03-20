#!/usr/bin/ruby
# -*- mode: ruby -*-
# $Id: ary-ruby.code,v 1.4 2004/11/13 07:41:27 bfulgham Exp $
# http://www.bagley.org/~doug/shootout/
# with help from Paul Brannan and Mark Hubbart

n = 9000 # Integer(ARGV.shift || 1)

x = Array.new(n)
y = Array.new(n, 0)

n.times{|bi|
  x[bi] = bi + 1
}

(0 .. 999).each do |e|
  (n-1).step(0,-1) do |bi|
    y[bi] += x.at(bi)
  end
end
# puts "#{y.first} #{y.last}"


