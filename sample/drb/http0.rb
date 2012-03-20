require 'drb/drb'
require 'net/http'
require 'uri'

module DRb
  module HTTP0
    class StrStream
      def initialize(str='')
	@buf = str
      end
      attr_reader :buf

      def read(n)
	begin
	  return @buf[0,n]
	ensure
	  @buf[0,n] = ''
	end
      end

      def write(s)
	@buf.concat s
      end
    end

    def self.uri_option(uri, config)
      return uri, nil
    end

    def self.open(uri, config)
      unless /^http:/ =~ uri
	raise(DRbBadScheme, uri) unless uri =~ /^http:/
	raise(DRbBadURI, 'can\'t parse uri:' + uri)
      end
      ClientSide.new(uri, config)
    end

    class ClientSide
      def initialize(uri, config)
	@uri = uri
	@res = nil
	@config = config
	@msg = DRbMessage.new(config)
	@proxy = ENV['HTTP_PROXY']
      end

      def close; end
      def alive?; false; end

      def send_request(ref, msg_id, *arg, &b)
	stream = StrStream.new
	@msg.send_request(stream, ref, msg_id, *arg, &b)
	@reply_stream = StrStream.new
	post(@uri, stream.buf)
      end

      def recv_reply
	@msg.recv_reply(@reply_stream)
      end

      def post(url, data)
	it = URI.parse(url)
	path = [(it.path=='' ? '/' : it.path), it.query].compact.join('?')
	http = Net::HTTP.new(it.host, it.port)
	sio = StrStream.new
	http.post(path, data, {'Content-Type'=>'application/octetstream;'}) do |str|
	  sio.write(str)
	  if @config[:load_limit] < sio.buf.size
	    raise TypeError, 'too large packet'
	  end
	end
	@reply_stream = sio
      end
    end
  end
  DRbProtocol.add_protocol(HTTP0)
end
