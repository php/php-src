require 'drb/drb'
require 'drb/extserv'
require 'timeout'

class DRbLarge
  include DRbUndumped

  def size(ary)
    ary.size
  end

  def sum(ary)
    sum = 0
    ary.each do |e|
      sum += e.to_i
    end
    sum
  end

  def arg_test(*arg)
    # nop
  end
end

if __FILE__ == $0
  def ARGV.shift
    it = super()
    raise "usage: #{$0} <manager-uri> <name>" unless it
    it
  end

  DRb::DRbServer.default_argc_limit(3)
  DRb::DRbServer.default_load_limit(100000)
  DRb.start_service('druby://localhost:0', DRbLarge.new)
  es = DRb::ExtServ.new(ARGV.shift, ARGV.shift)
  DRb.thread.join
end

