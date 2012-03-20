=begin
 dRuby sample
 	Copyright (c) 2000 Masatoshi SEKI

= How to play

* Terminal 1

  % ruby -I. extserv_test.rb server
  druby://yourhost:12345

* Terminal 2

  % ruby -I. extserv_test.rb druby://yourhost:12345
  ...

=end

require 'drb/drb'

def ARGV.shift
  it = super()
  raise "usage:\nserver: #{$0} server [<uri>]\nclient: #{$0} [quit] <uri>" unless it
  it
end

class Foo
  include DRbUndumped

  def initialize(str)
    @str = str
  end

  def hello(it)
    "#{it}: #{self}"
  end

  def to_s
    @str
  end
end

cmd = ARGV.shift
case cmd
when 'itest1', 'itest2'
  require 'drb/extserv'

  front = Foo.new(cmd)
  server = DRb::DRbServer.new(nil, front)
  es = DRb::ExtServ.new(ARGV.shift, ARGV.shift, server)
  server.thread.join

when 'server'
  require 'drb/extservm'

  DRb::ExtServManager.command['itest1'] = "ruby -I. #{$0} itest1"
  DRb::ExtServManager.command['itest2'] = "ruby -I. #{$0} itest2"

  s = DRb::ExtServManager.new
  DRb.start_service(ARGV.shift, s)
  puts DRb.uri
  DRb.thread.join


else
  uri = (cmd == 'quit') ? ARGV.shift : cmd

  DRb.start_service
  s = DRbObject.new(nil, uri)
  t1 = s.service('itest1').front
  puts t1
  t2 = s.service('itest2').front
  puts t2
  puts t1.hello(t2)
  if (cmd == 'quit')
    s.service('itest1').stop_service
    s.service('itest2').stop_service
  end
end

