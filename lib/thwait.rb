#
#   thwait.rb - thread synchronization class
#       $Release Version: 0.9 $
#       $Revision: 1.3 $
#       by Keiju ISHITSUKA(Nihon Rational Software Co.,Ltd.)

require "thread.rb"
require "e2mmap.rb"

#
# This class watches for termination of multiple threads.  Basic functionality
# (wait until specified threads have terminated) can be accessed through the
# class method ThreadsWait::all_waits.  Finer control can be gained using
# instance methods.
#
# Example:
#
#   ThreadsWait.all_wait(thr1, thr2, ...) do |t|
#     STDERR.puts "Thread #{t} has terminated."
#   end
#
#
#   th = ThreadsWait.new(thread1,...)
#   th.next_wait # next one to be done
#
#
class ThreadsWait
  RCS_ID='-$Id: thwait.rb,v 1.3 1998/06/26 03:19:34 keiju Exp keiju $-'

  extend Exception2MessageMapper
  def_exception("ErrNoWaitingThread", "No threads for waiting.")
  def_exception("ErrNoFinishedThread", "No finished threads.")

  #
  # Waits until all specified threads have terminated.  If a block is provided,
  # it is executed for each thread as they terminate.
  #
  def ThreadsWait.all_waits(*threads) # :yield: thread
    tw = ThreadsWait.new(*threads)
    if block_given?
      tw.all_waits do |th|
        yield th
      end
    else
      tw.all_waits
    end
  end

  #
  # Creates a ThreadsWait object, specifying the threads to wait on.
  # Non-blocking.
  #
  def initialize(*threads)
    @threads = []
    @wait_queue = Queue.new
    join_nowait(*threads) unless threads.empty?
  end

  # Returns the array of threads that have not terminated yet.
  attr :threads

  #
  # Returns +true+ if there are no threads in the pool still running.
  #
  def empty?
    @threads.empty?
  end

  #
  # Returns +true+ if any thread has terminated and is ready to be collected.
  #
  def finished?
    !@wait_queue.empty?
  end

  #
  # Waits for specified threads to terminate, and returns when one of
  # the threads terminated.
  #
  def join(*threads)
    join_nowait(*threads)
    next_wait
  end

  #
  # Specifies the threads that this object will wait for, but does not actually
  # wait.
  #
  def join_nowait(*threads)
    threads.flatten!
    @threads.concat threads
    for th in threads
      Thread.start(th) do |t|
        begin
          t.join
        ensure
          @wait_queue.push t
        end
      end
    end
  end

  #
  # Waits until any of the specified threads has terminated, and returns the one
  # that does.
  #
  # If there is no thread to wait, raises +ErrNoWaitingThread+.  If +nonblock+
  # is true, and there is no terminated thread, raises +ErrNoFinishedThread+.
  #
  def next_wait(nonblock = nil)
    ThreadsWait.fail ErrNoWaitingThread if @threads.empty?
    begin
      @threads.delete(th = @wait_queue.pop(nonblock))
      th
    rescue ThreadError
      ThreadsWait.fail ErrNoFinishedThread
    end
  end

  #
  # Waits until all of the specified threads are terminated.  If a block is
  # supplied for the method, it is executed for each thread termination.
  #
  # Raises exceptions in the same manner as +next_wait+.
  #
  def all_waits
    until @threads.empty?
      th = next_wait
      yield th if block_given?
    end
  end
end

##
# An alias for ThreadsWait from thwait.rb

ThWait = ThreadsWait

# Documentation comments:
#  - Source of documentation is evenly split between Nutshell, existing
#    comments, and my own rephrasing.
#  - I'm not particularly confident that the comments are all exactly correct.
