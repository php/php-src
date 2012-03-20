=begin
 multiple DRbServer
 	Copyright (c) 1999-2002 Masatoshi SEKI
=end

=begin
 How to play.

 Terminal 1
 | % ruby drbm.rb
 | druby://yourhost:7640 druby://yourhost:7641

 Terminal 2
 | % ruby drbmc.rb druby://yourhost:7640 druby://yourhost:7641
 | [#<DRb::DRbObject .... @uri="druby://yourhost:7640">, "FOO"]
 | [#<DRb::DRbObject .... @uri="druby://yourhost:7641">, "FOO"]

=end

require 'drb/drb'

class Hoge
  include DRbUndumped
  def initialize(s)
    @str = s
  end

  def to_s
    @str
  end
end

class Foo
  def initialize(s='FOO')
    @hoge = Hoge.new(s)
  end

  def hello
    @hoge
  end
end

class Bar < Foo
  def initialize(foo)
    @hoge = foo.hello
  end
end


if __FILE__ == $0
  foo = Foo.new
  s1 = DRb::DRbServer.new('druby://:7640', foo)
  s2 = DRb::DRbServer.new('druby://:7641', Bar.new(foo))

  puts "#{s1.uri} #{s2.uri}"

  s1.thread.join
  s2.thread.join
end

