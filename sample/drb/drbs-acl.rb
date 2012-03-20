=begin
 distributed Ruby --- dRuby Sample Server
 	Copyright (c) 1999-2000 Masatoshi SEKI
=end

=begin
 How to play.

 Terminal 1
 | % ruby drbs.rb
 | druby://yourhost:7640

 Terminal 2
 | % ruby drbc.rb druby://yourhost:7640
 | "hello"
 | 6
 | 10

=end

require 'drb/drb'
require 'acl'

class DRbEx
  def initialize
    @hello = 'hello'
  end

  def hello
    info = Thread.current['DRb']
    p info['socket'].peeraddr if info
    @hello
  end

  def sample(a, b, c)
    a.to_i + b.to_i + c.to_i
  end
end

if __FILE__ == $0
  acl = ACL.new(%w(deny all
                   allow 192.168.1.*
                   allow localhost))

  DRb.install_acl(acl)

  DRb.start_service(nil, DRbEx.new)
  puts DRb.uri
  DRb.thread.join
end

