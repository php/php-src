=begin
 distributed Ruby --- chat server
 	Copyright (c) 1999-2000 Masatoshi SEKI
=end
require 'thread'
require 'drb/drb'

class ChatEntry
  include DRbUndumped

  def initialize(server, there)
    @server = server
    @there = there
    @name = there.name
    @key = there.key = Time.now
  end
  attr :name, true
  attr :there

  def say(str)
    @server.distribute(@there, str)
  end

  def listen(str)
    @there.message(@key, str)
  end
end


class ChatServer
  def initialize
    @mutex = Mutex.new
    @members = {}
  end

  def add_member(there)
    client = ChatEntry.new(self, there)
    @mutex.synchronize do
      @members[there] = client
    end
    client
  end

  def distribute(there, str)
    name = @members[there].name
    msg = "<#{name}> #{str}"
    msg2 = ">#{name}< #{str}"
    @mutex.synchronize do
      for m in @members.keys
	begin
	  if m == there
	    @members[m].listen(msg2)
	   else
	    @members[m].listen(msg)
	  end
	rescue
	  p $!
	  @members.delete(m)
	end
      end
    end
  end
end

if __FILE__ == $0
  here = ARGV.shift
  DRb.start_service(here, ChatServer.new)
  puts DRb.uri
  DRb.thread.join
end
