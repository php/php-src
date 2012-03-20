=begin
= How to play.

==  with timeridconv:
  % ruby -d holders.rb
  druby://yourhost:1234

  % ruby holderc.rb druby://yourhost:1234


==  without timeridconv:
  % ruby holders.rb
  druby://yourhost:1234

  % ruby holderc.rb druby://yourhost:1234
=end


require 'drb/drb'

class DRbEx3
  include DRbUndumped

  def initialize(n)
    @v = n
  end

  def sample(list)
    sum = 0
    list.each do |e|
      sum += e.to_i
    end
    @v * sum
  end
end

class DRbEx4
  include DRbUndumped

  def initialize
    @curr = 1
  end

  def gen
    begin
      @curr += 1
      DRbEx3.new(@curr)
    ensure
      GC.start
    end
  end
end

if __FILE__ == $0
  if $DEBUG
    require 'drb/timeridconv'
    DRb.install_id_conv(DRb::TimerIdConv.new(2))
  end

  DRb.start_service(nil, DRbEx4.new)
  puts DRb.uri
  DRb.thread.join
end
