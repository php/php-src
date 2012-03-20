require 'webrick'
require 'drb/drb'
require 'drb/http0'
require 'thread'

module DRb
  module HTTP0

    def self.open_server(uri, config)
      unless /^http:/ =~ uri
	raise(DRbBadScheme, uri) unless uri =~ /^http:/
	raise(DRbBadURI, 'can\'t parse uri:' + uri)
      end
      Server.new(uri, config)
    end

    class Callback < WEBrick::HTTPServlet::AbstractServlet
      def initialize(config, drb)
	@config = config
	@drb = drb
	@queue = Queue.new
      end

      def do_POST(req, res)
	@req = req
	@res = res
	@drb.push(self)
	@res.body = @queue.pop
	@res['content-type'] = 'application/octet-stream;'
      end

      def req_body
	@req.body
      end

      def reply(body)
	@queue.push(body)
      end

      def close
	@queue.push('')
      end
    end

    class Server
      def initialize(uri, config)
	@uri = uri
	@config = config
	@queue = Queue.new
	setup_webrick(uri)
      end
      attr_reader :uri

      def close
	@server.shutdown if @server
	@server = nil
      end

      def push(callback)
	@queue.push(callback)
      end

      def accept
	client = @queue.pop
	ServerSide.new(client, @config)
      end

      def setup_webrick(uri)
	logger = WEBrick::Log::new($stderr, WEBrick::Log::FATAL)
	u = URI.parse(uri)
	s = WEBrick::HTTPServer.new(:Port => u.port,
				    :AddressFamily => Socket::AF_INET,
				    :BindAddress => u.host,
				    :Logger => logger,
				    :ServerType => Thread)
	s.mount(u.path, Callback, self)
	@server = s
	s.start
      end
    end

    class ServerSide
      def initialize(callback, config)
	@callback = callback
	@config = config
	@msg = DRbMessage.new(@config)
	@req_stream = StrStream.new(@callback.req_body)
      end

      def close
	@callback.close if @callback
	@callback = nil
      end

      def alive?; false; end

      def recv_request
	begin
	  @msg.recv_request(@req_stream)
	rescue
	  close
	  raise $!
	end
      end

      def send_reply(succ, result)
	begin
	  return unless @callback
	  stream = StrStream.new
	  @msg.send_reply(stream, succ, result)
	  @callback.reply(stream.buf)
	rescue
	  close
	  raise $!
	end
      end
    end
  end
end
