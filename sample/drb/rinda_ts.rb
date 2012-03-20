require 'drb/drb'
require 'rinda/tuplespace'

uri = ARGV.shift
DRb.start_service(uri, Rinda::TupleSpace.new)
puts DRb.uri
DRb.thread.join
