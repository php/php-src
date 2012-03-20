=begin
 distributed Ruby --- chat client
 	Copyright (c) 1999-2000 Masatoshi SEKI
=end

require 'drb/drb'

class ChatClient
  include DRbUndumped

  def initialize(name)
    @name = name
    @key = nil
  end
  attr_reader(:name)
  attr_accessor(:key)

  def message(there, str)
    raise 'invalid key' unless @key == there
    puts str
  end
end

if __FILE__ == $0
  begin
    there = ARGV.shift
    name = ARGV.shift
    raise "usage" unless (there and name)
  rescue
    $stderr.puts("usage: #{$0} <server_uri> <your_name>")
    exit 1
  end
  DRb.start_service
  ro = DRbObject.new(nil, there)

  chat = ChatClient.new(name)
  entry = ro.add_member(chat)
  while gets
    entry.say($_)
  end
end
