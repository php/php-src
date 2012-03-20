require 'drb/drb'
require 'rinda/rinda'

uri = ARGV.shift || raise("usage: #{$0} <server_uri>")

DRb.start_service
ts = Rinda::TupleSpaceProxy.new(DRbObject.new(nil, uri))

(1..10).each do |n|
  ts.write(['sum', DRb.uri, n])
end

(1..10).each do |n|
  ans = ts.take(['ans', DRb.uri, n, nil])
  p [ans[2], ans[3]]
end

