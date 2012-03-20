=begin
  multiple DRbServer client
 	Copyright (c) 1999-2002 Masatoshi SEKI
=end

require 'drb/drb'

if __FILE__ == $0
  s1 = ARGV.shift
  s2 = ARGV.shift
  unless s1 && s2
    $stderr.puts("usage: #{$0} <server_uri1> <server_uri2>")
    exit 1
  end

  DRb.start_service()
  r1 = DRbObject.new(nil, s1)
  r2 = DRbObject.new(nil, s2)

  p [r1.hello, r1.hello.to_s]
  p [r2.hello, r2.hello.to_s]
end
