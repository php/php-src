=begin
 distributed Ruby --- NamedObject Sample Client
 	Copyright (c) 2000-2001 Masatoshi SEKI
=end

require 'drb/drb'

begin
  there = ARGV.shift || raise
rescue
  puts "usage: #{$0} <server_uri>"
  exit 1
end

DRb.start_service()
ro = DRbObject.new(nil, there)

seq = ro["seq"]
mutex = ro["mutex"]

p seq
p mutex

mutex.synchronize do
  p seq.next_value
  p seq.next_value
end

puts '[return] to continue'
gets

mutex.synchronize do
  p seq.next_value
  p seq.next_value
end

