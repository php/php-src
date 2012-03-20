#! /usr/local/bin/ruby

require "thread"
require "observer"

class Tick
  include Observable
  def initialize
    Thread.start do
      loop do
	sleep 0.999
	now = Time.now
	changed
	notify_observers(now.hour, now.min, now.sec)
      end
    end
  end
end

class Clock
  def initialize(tick)
    @tick = tick
    @tick.add_observer(self)
  end
  def update(h, m, s)
    printf "\e[8D%02d:%02d:%02d", h, m, s
    STDOUT.flush
  end
end

clock = Clock.new(Tick.new)
sleep
