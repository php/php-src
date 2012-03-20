require 'drb/drb'
require 'drb/eq'
require 'rinda/ring'
require 'thread'

class RingEcho
  include DRbUndumped
  def initialize(name)
    @name = name
  end

  def echo(str)
    "#{@name}: #{str}"
  end
end

DRb.start_service

renewer = Rinda::SimpleRenewer.new

finder = Rinda::RingFinger.new
ts = finder.lookup_ring_any
ts.read_all([:name, :RingEcho, nil, nil]).each do |tuple|
  p tuple[2]
  puts tuple[2].echo('Hello, World') rescue nil
end
ts.write([:name, :RingEcho, RingEcho.new(DRb.uri), ''], renewer)

DRb.thread.join

