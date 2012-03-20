=begin
 distributed Ruby --- Log server
 	Copyright (c) 1999-2000 Masatoshi SEKI
=end

require 'drb/drb'
require 'thread'

class Logger
  def initialize(fname)
    @fname = fname.to_s
    @fp = File.open(@fname, "a+")
    @queue = Queue.new
    @th = Thread.new { self.flush }
  end

  def log(str)
    @queue.push("#{Time.now}\t" + str.to_s)
  end

  def flush
    begin
      while(1)
	@fp.puts(@queue.pop)
	@fp.flush
      end
    ensure
      @fp.close
    end
  end
end

if __FILE__ == $0
  here = ARGV.shift
  DRb.start_service(here, Logger.new('/usr/tmp/dlogd.log'))
  puts DRb.uri
  DRb.thread.join
end

