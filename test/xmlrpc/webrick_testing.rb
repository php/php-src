require 'timeout'

module WEBrick_Testing
  class DummyLog < WEBrick::BasicLog
    def initialize() super(self) end
    def <<(*args) end
  end

  def start_server(config={})
    raise "already started" if defined?(@__server) && @__server
    @__started = false

    @__server_thread = Thread.new {
      @__server = WEBrick::HTTPServer.new(
        {
          :BindAddress => "localhost",
          :Logger => DummyLog.new,
          :AccessLog => [],
          :StartCallback => proc { @__started = true }
        }.update(config))
      yield @__server
      begin
        @__server.start
      rescue IOError => e
        assert_match(/closed/, e.message)
      ensure
        @__started = false
      end
    }

    Timeout.timeout(5) {
      Thread.pass until @__started # wait until the server is ready
    }
  end

  def stop_server
    Timeout.timeout(5) {
      @__server.shutdown
      Thread.pass while @__started # wait until the server is down
    }
    @__server_thread.join
    @__server = nil
  end
end
