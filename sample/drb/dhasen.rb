=begin
 distributed Ruby --- dRuby Sample Server --- chasen server
 	Copyright (c) 1999-2001 Masatoshi SEKI
=end

=begin
 How to play.

 Terminal 1
 | % ruby dhasen.rb
 | druby://yourhost:7640

 Terminal 2
 | % ruby dhasenc.rb druby://yourhost:7640

=end

require 'drb/drb'
require 'chasen'
require 'thread'

class Dhasen
  include DRbUndumped

  def initialize
    @mutex = Mutex.new
  end

  def sparse(str, *arg)
    @mutex.synchronize do
      Chasen.getopt(*arg)
      Chasen.sparse(str)
    end
  end
end

if __FILE__ == $0
  DRb.start_service(nil, Dhasen.new)
  puts DRb.uri
  DRb.thread.join
end

