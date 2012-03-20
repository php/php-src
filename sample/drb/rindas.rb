require 'drb/drb'
require 'rinda/rinda'

def do_it(v)
  puts "do_it(#{v})"
  v + v
end

uri = ARGV.shift || raise("usage: #{$0} <server_uri>")

DRb.start_service
ts = Rinda::TupleSpaceProxy.new(DRbObject.new(nil, uri))

while true
  r = ts.take(['sum', nil, nil])
  v = do_it(r[2])
  ts.write(['ans', r[1], r[2], v])
end
