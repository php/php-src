=begin
 distributed Ruby --- dRuby Sample Client
 	Copyright (c) 1999-2000 Masatoshi SEKI
=end

require 'drb/drb'

class DRbEx2
  include DRbUndumped

  def initialize(n)
    @n = n
  end

  def to_i
    @n.to_i
  end
end

if __FILE__ == $0
  there = ARGV.shift
  unless there
    $stderr.puts("usage: #{$0} <server_uri>")
    exit 1
  end

  DRb.start_service()
  ro = DRbObject.new_with_uri(there)

  puts ro
  p ro.to_a
  puts ro.hello
  p ro.hello
  puts ro.sample(DRbEx2.new(1), 2, 3)
  puts ro.sample(1, ro.sample(DRbEx2.new(1), 2, 3), DRbEx2.new(3))

  begin
    ro.err
  rescue DRb::DRbUnknownError
    p $!
    p $!.unknown
  rescue RuntimeError
    p $!
  end
end
