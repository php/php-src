#!/usr/local/bin/ruby
# TupleSpace
# Copyright (c) 1999-2000 Masatoshi SEKI
# You can redistribute it and/or modify it under the same terms as Ruby.

require 'thread'

class TupleSpace
  class Template
    def initialize(list)
      @list = list
      @check_idx = []
      @list.each_with_index do |x, i|
	@check_idx.push i if x
      end
      @size = @list.size
    end

    attr :size
    alias length size

    def match(tuple)
      return nil if tuple.size != self.size
      @check_idx.each do |i|
	unless @list[i] === tuple[i]
	  return false
	end
      end
      return true
    end
  end

  def initialize
    @que = {}
    @waiting = {}
    @que.taint		# enable tainted comunication
    @waiting.taint
    self.taint
  end

  def wakeup_waiting(tuple)
    sz = tuple.length
    return nil unless @waiting[sz]

    x = nil
    i = -1
    found = false
    @waiting[sz] = @waiting[sz].find_all { |x|
      if x[0].match(tuple)
	begin
	  x[1].wakeup
	rescue ThreadError
	end
	false
      else
	true
      end
    }
  end

  def put_waiting(template, thread)
    sz = template.length
    @waiting[sz] = [] unless @waiting[sz]
    @waiting[sz].push([Template.new(template), thread])
  end
  private :wakeup_waiting
  private :put_waiting

  def get_que(template)
    sz = template.length
    return nil unless @que[sz]

    template = Template.new(template)

    x = nil
    i = -1
    found = false
    @que[sz].each_with_index do |x, i|
      if template.match(x)
	found = true
	break
      end
    end
    return nil unless found

    @que[sz].delete_at(i)

    return x
  end

  def put_que(tuple)
    sz = tuple.length
    @que[sz] = [] unless @que[sz]
    @que[sz].push tuple
  end
  private :get_que
  private :put_que

  def out(*tuples)
    tuples.each do |tuple|
      Thread.critical = true
      put_que(tuple)
      wakeup_waiting(tuple)
      Thread.critical = false
    end
  end
  alias put out
  alias write out

  def in(template, non_block=false)
    begin
      loop do
	Thread.critical = true
	tuple = get_que(template)
	unless tuple
	  if non_block
	    raise ThreadError, "queue empty"
	  end
	  put_waiting(template, Thread.current)
	  Thread.stop
	else
	  return tuple
	end
      end
    ensure
      Thread.critical = false
    end
  end
  alias get in
  alias take in

  def rd(template, non_block=false)
    tuple = self.in(template, non_block)
    out(tuple)
    tuple
  end
  alias read rd

  def mv(dest, template, non_block=false)
    tuple = self.in(template, non_block)
    begin
      dest.out(tuple)
    rescue
      self.out(tuple)
    end
  end
  alias move mv
end

if __FILE__ == $0
  ts = TupleSpace.new
  clients = []
  servers = []

  def server(ts, id)
    Thread.start {
      loop do
	req = ts.in(['req', nil, nil])
	ac = req[1]
	num = req[2]
	sleep id
	ts.out([ac, id, num, num * num])
      end
    }
  end

  def client(ts, n)
    Thread.start {
      ac = Object.new
      tuples = (1..10).collect { |i|
	['req', ac, i * 10 + n]
      }
      ts.out(*tuples)
      ts.out(tuples[0])
      puts "out: #{n}"
      11.times do |i|
	ans = ts.in([ac, nil, nil, nil])
	puts "client(#{n}) server(#{ans[1]}) #{ans[2]} #{ans[3]}"
      end
    }
  end

  def watcher(ts)
    Thread.start {
      loop do
	begin
	  sleep 1
	  p ts.rd(['req', nil, nil], true)
	rescue ThreadError
	  puts "'req' not found."
	end
      end
    }
  end

  (0..3).each do |n|
    servers.push(server(ts, n))
  end

  (1..6).each do |n|
    clients.push(client(ts, n))
  end

  (1..3).each do
    watcher(ts)
  end

  clients.each do |t|
    t.join
  end
end



