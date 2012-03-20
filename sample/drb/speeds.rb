class Foo
  attr_reader :i
  def initialize
    @i = 0
  end

  def foo(i)
    @i = i
    i + i
  end
end

# server = ROMP::Server.new('tcpromp://localhost:4242', nil, true)

uri = ARGV.shift || raise("usage: #{$0} URI")
foo = Foo.new

case uri
when /^tcpromp:/, /^unixromp:/
  require 'romp'

  server = ROMP::Server.new(uri, nil, true)
  server.bind(foo, "foo")

when /^druby:/
  require 'drb/drb'

  DRb.start_service(uri, Foo.new)
end

DRb.thread.join
