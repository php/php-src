require 'thread'

class LocalBarrier
  def initialize(n)
    @wait = Queue.new
    @done = Queue.new
    @keeper = begin_keeper(n)
  end

  def sync
    @done.push(true)
    @wait.pop
  end

  def join
    @keeper.join
  end

  private
  def begin_keeper(n)
    Thread.start do
      n.times do
        @done.pop
      end
      n.times do
        @wait.push(true)
      end
    end
  end
end

n = 10

lb = LocalBarrier.new(n)

(n - 1).times do |i|
  Thread.start do
    sleep((rand(n) + 1) / 10.0)
    puts "#{i}: done"
    lb.sync
    puts "#{i}: cont"
  end
end

lb.sync
puts "#{n-1}: cone"

puts "exit."
