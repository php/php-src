require 'drb/drb'
require 'rinda/ring'
require 'rinda/tuplespace'

unless $DEBUG
  # Run as a daemon...
  exit!( 0 ) if fork
  Process.setsid
  exit!( 0 ) if fork
end

DRb.start_service(ARGV.shift)

ts = Rinda::TupleSpace.new
place = Rinda::RingServer.new(ts)

if $DEBUG
  puts DRb.uri
  DRb.thread.join
else
  STDIN.reopen('/dev/null')
  STDOUT.reopen('/dev/null', 'w')
  STDERR.reopen('/dev/null', 'w')
  DRb.thread.join
end
