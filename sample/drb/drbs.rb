=begin
 distributed Ruby --- dRuby Sample Server
 	Copyright (c) 1999-2000,2002 Masatoshi SEKI
=end

=begin
 How to play.

 Terminal 1
 | % ruby drbs.rb
 | druby://yourhost:7640

 Terminal 2
 | % ruby drbc.rb druby://yourhost:7640
 | "hello"
 | ....

=end

require 'drb/drb'

class DRbEx
  include DRbUndumped

  def initialize
    @hello = 'hello'
  end

  def hello
    cntxt = Thread.current['DRb']
    if cntxt
      p cntxt['server'].uri
      p cntxt['client'].peeraddr
    end
    Foo::Unknown.new
  end

  def err
    raise FooError
  end

  def sample(a, b, c)
    a.to_i + b.to_i + c.to_i
  end
end

class Foo
  class Unknown
  end
end

class FooError < RuntimeError
end

if __FILE__ == $0
  DRb.start_service(ARGV.shift || 'druby://:7640', DRbEx.new)
  puts DRb.uri
  Thread.new do
    sleep 10
    DRb.stop_service
  end
  DRb.thread.join
end

