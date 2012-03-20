=begin
 distributed Ruby --- Array client
 	Copyright (c) 1999-2001 Masatoshi SEKI
=end

require 'drb/drb'

there = ARGV.shift || raise("usage: #{$0} <server_uri>")

DRb.start_service(nil, nil)
ro = DRbObject.new(nil, there)
p ro.size

puts "# collect"
a = ro.collect { |x|
  x + x
}
p a

puts "# find"
p ro.find { |x| x.kind_of? String }

puts "# each, break"
ro.each do |x|
  next if x == "five"
  puts x
end

puts "# each, break"
ro.each do |x|
  break if x == "five"
  puts x
end

puts "# each, next"
ro.each do |x|
  next if x == "five"
  puts x
end

puts "# each, redo"
count = 0
ro.each do |x|
  count += 1
  puts count
  redo if count == 3
end
