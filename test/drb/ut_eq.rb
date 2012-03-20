require 'drb/drb'
require 'drb/extserv'

class Foo
  include DRbUndumped
end

class Bar
  include DRbUndumped
  def initialize
    @foo = Foo.new
  end
  attr_reader :foo

  def foo?(foo)
    @foo == foo
  end
end

if __FILE__ == $0
  def ARGV.shift
    it = super()
    raise "usage: #{$0} <uri> <name>" unless it
    it
  end

  DRb.start_service('druby://:0', Bar.new)
  es = DRb::ExtServ.new(ARGV.shift, ARGV.shift)
  DRb.thread.join
end
