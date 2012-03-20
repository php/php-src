=begin
 external service manager
        Copyright (c) 2000 Masatoshi SEKI
=end

require 'drb/drb'
require 'thread'
require 'monitor'

module DRb
  class ExtServManager
    include DRbUndumped
    include MonitorMixin

    @@command = {}

    def self.command
      @@command
    end

    def self.command=(cmd)
      @@command = cmd
    end

    def initialize
      super()
      @cond = new_cond
      @servers = {}
      @waiting = []
      @queue = Queue.new
      @thread = invoke_thread
      @uri = nil
    end
    attr_accessor :uri

    def service(name)
      synchronize do
        while true
          server = @servers[name]
          return server if server && server.alive?
          invoke_service(name)
          @cond.wait
        end
      end
    end

    def regist(name, ro)
      synchronize do
        @servers[name] = ro
        @cond.signal
      end
      self
    end

    def unregist(name)
      synchronize do
        @servers.delete(name)
      end
    end

    private
    def invoke_thread
      Thread.new do
        while true
          name = @queue.pop
          invoke_service_command(name, @@command[name])
        end
      end
    end

    def invoke_service(name)
      @queue.push(name)
    end

    def invoke_service_command(name, command)
      raise "invalid command. name: #{name}" unless command
      synchronize do
        return if @servers.include?(name)
        @servers[name] = false
      end
      uri = @uri || DRb.uri
      Process.detach spawn("#{command} #{uri} #{name}")
    end
  end
end
