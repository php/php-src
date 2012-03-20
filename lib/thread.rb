#
#               thread.rb - thread support classes
#                       by Yukihiro Matsumoto <matz@netlab.co.jp>
#
# Copyright (C) 2001  Yukihiro Matsumoto
# Copyright (C) 2000  Network Applied Communication Laboratory, Inc.
# Copyright (C) 2000  Information-technology Promotion Agency, Japan
#

unless defined? Thread
  raise "Thread not available for this ruby interpreter"
end

unless defined? ThreadError
  class ThreadError < StandardError
  end
end

if $DEBUG
  Thread.abort_on_exception = true
end

#
# ConditionVariable objects augment class Mutex. Using condition variables,
# it is possible to suspend while in the middle of a critical section until a
# resource becomes available.
#
# Example:
#
#   require 'thread'
#
#   mutex = Mutex.new
#   resource = ConditionVariable.new
#
#   a = Thread.new {
#     mutex.synchronize {
#       # Thread 'a' now needs the resource
#       resource.wait(mutex)
#       # 'a' can now have the resource
#     }
#   }
#
#   b = Thread.new {
#     mutex.synchronize {
#       # Thread 'b' has finished using the resource
#       resource.signal
#     }
#   }
#
class ConditionVariable
  #
  # Creates a new ConditionVariable
  #
  def initialize
    @waiters = []
    @waiters_mutex = Mutex.new
  end

  #
  # Releases the lock held in +mutex+ and waits; reacquires the lock on wakeup.
  #
  # If +timeout+ is given, this method returns after +timeout+ seconds passed,
  # even if no other thread doesn't signal.
  #
  def wait(mutex, timeout=nil)
    begin
      # TODO: mutex should not be used
      @waiters_mutex.synchronize do
        @waiters.push(Thread.current)
      end
      mutex.sleep timeout
    ensure
      @waiters_mutex.synchronize do
        @waiters.delete(Thread.current)
      end
    end
    self
  end

  #
  # Wakes up the first thread in line waiting for this lock.
  #
  def signal
    begin
      t = @waiters_mutex.synchronize { @waiters.shift }
      t.run if t
    rescue ThreadError
      retry
    end
    self
  end

  #
  # Wakes up all threads waiting for this lock.
  #
  def broadcast
    # TODO: incomplete
    waiters0 = nil
    @waiters_mutex.synchronize do
      waiters0 = @waiters.dup
      @waiters.clear
    end
    for t in waiters0
      begin
        t.run
      rescue ThreadError
      end
    end
    self
  end
end

#
# This class provides a way to synchronize communication between threads.
#
# Example:
#
#   require 'thread'
#
#   queue = Queue.new
#
#   producer = Thread.new do
#     5.times do |i|
#       sleep rand(i) # simulate expense
#       queue << i
#       puts "#{i} produced"
#     end
#   end
#
#   consumer = Thread.new do
#     5.times do |i|
#       value = queue.pop
#       sleep rand(i/2) # simulate expense
#       puts "consumed #{value}"
#     end
#   end
#
#   consumer.join
#
class Queue
  #
  # Creates a new queue.
  #
  def initialize
    @que = []
    @waiting = []
    @que.taint          # enable tainted communication
    @waiting.taint
    self.taint
    @mutex = Mutex.new
  end

  #
  # Pushes +obj+ to the queue.
  #
  def push(obj)
    @mutex.synchronize{
      @que.push obj
      begin
        t = @waiting.shift
        t.wakeup if t
      rescue ThreadError
        retry
      end
    }
  end

  #
  # Alias of push
  #
  alias << push

  #
  # Alias of push
  #
  alias enq push

  #
  # Retrieves data from the queue.  If the queue is empty, the calling thread is
  # suspended until data is pushed onto the queue.  If +non_block+ is true, the
  # thread isn't suspended, and an exception is raised.
  #
  def pop(non_block=false)
    @mutex.synchronize{
      while true
        if @que.empty?
          raise ThreadError, "queue empty" if non_block
          # @waiting.include? check is necessary for avoiding a race against
          # Thread.wakeup [Bug 5195]
          @waiting.push Thread.current unless @waiting.include?(Thread.current)
          @mutex.sleep
        else
          return @que.shift
        end
      end
    }
  end

  #
  # Alias of pop
  #
  alias shift pop

  #
  # Alias of pop
  #
  alias deq pop

  #
  # Returns +true+ if the queue is empty.
  #
  def empty?
    @que.empty?
  end

  #
  # Removes all objects from the queue.
  #
  def clear
    @que.clear
  end

  #
  # Returns the length of the queue.
  #
  def length
    @que.length
  end

  #
  # Alias of length.
  #
  alias size length

  #
  # Returns the number of threads waiting on the queue.
  #
  def num_waiting
    @waiting.size
  end
end

#
# This class represents queues of specified size capacity.  The push operation
# may be blocked if the capacity is full.
#
# See Queue for an example of how a SizedQueue works.
#
class SizedQueue < Queue
  #
  # Creates a fixed-length queue with a maximum size of +max+.
  #
  def initialize(max)
    raise ArgumentError, "queue size must be positive" unless max > 0
    @max = max
    @queue_wait = []
    @queue_wait.taint           # enable tainted comunication
    super()
  end

  #
  # Returns the maximum size of the queue.
  #
  def max
    @max
  end

  #
  # Sets the maximum size of the queue.
  #
  def max=(max)
    raise ArgumentError, "queue size must be positive" unless max > 0
    diff = nil
    @mutex.synchronize {
      if max <= @max
        @max = max
      else
        diff = max - @max
        @max = max
      end
    }
    if diff
      diff.times do
        begin
          t = @queue_wait.shift
          t.run if t
        rescue ThreadError
          retry
        end
      end
    end
    max
  end

  #
  # Pushes +obj+ to the queue.  If there is no space left in the queue, waits
  # until space becomes available.
  #
  def push(obj)
    @mutex.synchronize{
      while true
        break if @que.length < @max
        @queue_wait.push Thread.current
        @mutex.sleep
      end

      @que.push obj
      begin
        t = @waiting.shift
        t.wakeup if t
      rescue ThreadError
        retry
      end
    }
  end

  #
  # Alias of push
  #
  alias << push

  #
  # Alias of push
  #
  alias enq push

  #
  # Retrieves data from the queue and runs a waiting thread, if any.
  #
  def pop(*args)
    retval = super
    @mutex.synchronize {
      if @que.length < @max
        begin
          t = @queue_wait.shift
          t.wakeup if t
        rescue ThreadError
          retry
        end
      end
    }
    retval
  end

  #
  # Alias of pop
  #
  alias shift pop

  #
  # Alias of pop
  #
  alias deq pop

  #
  # Returns the number of threads waiting on the queue.
  #
  def num_waiting
    @waiting.size + @queue_wait.size
  end
end

# Documentation comments:
#  - How do you make RDoc inherit documentation from superclass?
