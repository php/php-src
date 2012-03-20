require 'drb/drb'

begin
  there = ARGV.shift || raise
rescue
  $stderr.puts("usage: #{$0} <server_uri>")
  exit 1
end

DRb.start_service()
ro = DRbObject.new(nil, there)

ary = []
10.times do
  ary.push(ro.gen)
end

sleep 5 if $DEBUG

ary.each do |e|
  p e.sample([1])
end
