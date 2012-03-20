#!/usr/local/bin/ruby
# SimpleTupleSpace
# Copyright (c) 1999-2000 Masatoshi SEKI
# You can redistribute it and/or modify it under the same terms as Ruby.

require 'thread'

class SimpleTupleSpace
  def initialize
    @hash = {}
    @waiting = {}
    @hash.taint
    @waiting.taint
    self.taint
  end

  def out(key, obj)
    Thread.critical = true
    @hash[key] ||= []
    @waiting[key] ||= []
    @hash[key].push obj
    begin
      t = @waiting[key].shift
      @waiting.delete(key) if @waiting[key].length == 0
      t.wakeup if t
    rescue ThreadError
      retry
    ensure
      Thread.critical = false
    end
  end

  def in(key)
    Thread.critical = true
    @hash[key] ||= []
    @waiting[key] ||= []
    begin
      loop do
        if @hash[key].length == 0
          @waiting[key].push Thread.current
          Thread.stop
        else
          return @hash[key].shift
        end
      end
    ensure
      @hash.delete(key) if @hash[key].length == 0
      Thread.critical = false
    end
  end
end

if __FILE__ == $0
  ts = SimpleTupleSpace.new
  clients = []
  servers = []

  def server(ts)
    Thread.start {
      loop do
	req = ts.in('req')
	ac = req[0]
	num = req[1]
	ts.out(ac, num * num)
      end
    }
  end

  def client(ts, n)
    Thread.start {
      ac = Object.new
      ts.out('req', [ac, n])
      ans = ts.in(ac)
      puts "#{n}: #{ans}"
    }
  end

  3.times do
    servers.push(server(ts))
  end

  (1..6).each do |n|
    clients.push(client(ts, n))
  end

  clients.each do |t|
    t.join
  end
end


