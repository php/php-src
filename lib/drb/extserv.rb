=begin
 external service
        Copyright (c) 2000,2002 Masatoshi SEKI
=end

require 'drb/drb'
require 'monitor'

module DRb
  class ExtServ
    include MonitorMixin
    include DRbUndumped

    def initialize(there, name, server=nil)
      super()
      @server = server || DRb::primary_server
      @name = name
      ro = DRbObject.new(nil, there)
      synchronize do
        @invoker = ro.regist(name, DRbObject.new(self, @server.uri))
      end
    end
    attr_reader :server

    def front
      DRbObject.new(nil, @server.uri)
    end

    def stop_service
      synchronize do
        @invoker.unregist(@name)
        server = @server
        @server = nil
        server.stop_service
        true
      end
    end

    def alive?
      @server ? @server.alive? : false
    end
  end
end

if __FILE__ == $0
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
    front = Foo.new(cmd)
    manager = DRb::DRbServer.new(nil, front)
    es = DRb::ExtServ.new(ARGV.shift, ARGV.shift, manager)
    es.server.thread.join
  end
end

