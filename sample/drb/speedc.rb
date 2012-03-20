#!/usr/local/bin/ruby

uri = ARGV.shift || raise("usage: #{$0} URI")
N = (ARGV.shift || 100).to_i

case uri
when /^tcpromp:/, /^unixromp:/
  require 'romp'

  client = ROMP::Client.new(uri, false)
  foo = client.resolve("foo")
when /^druby:/
  require 'drb/drb'

  DRb.start_service
  foo = DRbObject.new(nil, uri)
end

N.times do |n|
  foo.foo(n)
end
