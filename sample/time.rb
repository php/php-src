#! /usr/bin/env ruby

b = Time.now
system(*ARGV)
e = Time.now

tms = Process.times
real = e - b
user = tms.cutime
sys = tms.cstime

STDERR.printf("%11.1f real %11.1f user %11.1f sys\n", real, user, sys)
