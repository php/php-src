#!/usr/bin/ruby
# -*- mode: ruby -*-
# $Id: wc-ruby.code,v 1.4 2004/11/13 07:43:32 bfulgham Exp $
# http://www.bagley.org/~doug/shootout/
# with help from Paul Brannan

input = open(File.join(File.dirname($0), 'wc.input'), 'rb')

nl = nw = nc = 0
while true
  tmp = input.read(4096) or break
  data = tmp << (input.gets || "")
  nc += data.length
  nl += data.count("\n")
  ((data.strip! || data).tr!("\n", " ") || data).squeeze!
  nw += data.count(" ") + 1
end
# STDERR.puts "#{nl} #{nw} #{nc}"

