#
# httpproxy.rb -- HTTPProxy Class
#
# Author: IPR -- Internet Programming with Ruby -- writers
# Copyright (c) 2002 GOTO Kentaro
# Copyright (c) 2002 Internet Programming with Ruby writers. All rights
# reserved.
#
# $IPR: httpproxy.rb,v 1.18 2003/03/08 18:58:10 gotoyuzo Exp $
# $kNotwork: straw.rb,v 1.3 2002/02/12 15:13:07 gotoken Exp $

require "webrick/httpserver"
require "net/http"

Net::HTTP::version_1_2 if RUBY_VERSION < "1.7"

module WEBrick
  NullReader = Object.new
  class << NullReader
    def read(*args)
      nil
    end
    alias gets read
  end

  FakeProxyURI = Object.new
  class << FakeProxyURI
    def method_missing(meth, *args)
      if %w(scheme host port path query userinfo).member?(meth.to_s)
        return nil
      end
      super
    end
  end

  ##
  # An HTTP Proxy server which proxies GET, HEAD and POST requests.
  #
  # To create a simple proxy server:
  #
  #   require 'webrick'
  #   require 'webrick/httpproxy'
  #
  #   proxy = WEBrick::HTTPProxyServer.new Port: 8000
  #
  #   trap 'INT'  do p.shutdown end
  #   trap 'TERM' do p.shutdown end
  #
  #   p.start
  #
  # See ::new for proxy-specific configuration items.
  #
  # == Modifying proxied responses
  #
  # To modify content the proxy server returns use the +:ProxyContentHandler+
  # option:
  #
  #   handler = proc do |req, res|
  #     if res['content-type'] == 'text/plain' then
  #       res.body << "\nThis content was proxied!\n"
  #     end
  #   end
  #
  #   proxy =
  #     WEBrick::HTTPProxyServer.new Port: 8000, ProxyContentHandler: handler

  class HTTPProxyServer < HTTPServer

    ##
    # Proxy server configurations.  The proxy server handles the following
    # configuration items in addition to those supported by HTTPServer:
    #
    # :ProxyAuthProc:: Called with a request and response to authorize a
    #                  request
    # :ProxyVia:: Appended to the via header
    # :ProxyURI:: The proxy server's URI
    # :ProxyContentHandler:: Called with a request and response and allows
    #                        modification of the response
    # :ProxyTimeout:: Sets the proxy timeouts to 30 seconds for open and 60
    #                 seconds for read operations

    def initialize(config={}, default=Config::HTTP)
      super(config, default)
      c = @config
      @via = "#{c[:HTTPVersion]} #{c[:ServerName]}:#{c[:Port]}"
    end

    def service(req, res)
      if req.request_method == "CONNECT"
        do_CONNECT(req, res)
      elsif req.unparsed_uri =~ %r!^http://!
        proxy_service(req, res)
      else
        super(req, res)
      end
    end

    def proxy_auth(req, res)
      if proc = @config[:ProxyAuthProc]
        proc.call(req, res)
      end
      req.header.delete("proxy-authorization")
    end

    def proxy_uri(req, res)
      # should return upstream proxy server's URI
      return @config[:ProxyURI]
    end

    def proxy_service(req, res)
      # Proxy Authentication
      proxy_auth(req, res)

      begin
        self.send("do_#{req.request_method}", req, res)
      rescue NoMethodError
        raise HTTPStatus::MethodNotAllowed,
          "unsupported method `#{req.request_method}'."
      rescue => err
        logger.debug("#{err.class}: #{err.message}")
        raise HTTPStatus::ServiceUnavailable, err.message
      end

      # Process contents
      if handler = @config[:ProxyContentHandler]
        handler.call(req, res)
      end
    end

    def do_CONNECT(req, res)
      # Proxy Authentication
      proxy_auth(req, res)

      ua = Thread.current[:WEBrickSocket]  # User-Agent
      raise HTTPStatus::InternalServerError,
        "[BUG] cannot get socket" unless ua

      host, port = req.unparsed_uri.split(":", 2)
      # Proxy authentication for upstream proxy server
      if proxy = proxy_uri(req, res)
        proxy_request_line = "CONNECT #{host}:#{port} HTTP/1.0"
        if proxy.userinfo
          credentials = "Basic " + [proxy.userinfo].pack("m").delete("\n")
        end
        host, port = proxy.host, proxy.port
      end

      begin
        @logger.debug("CONNECT: upstream proxy is `#{host}:#{port}'.")
        os = TCPSocket.new(host, port)     # origin server

        if proxy
          @logger.debug("CONNECT: sending a Request-Line")
          os << proxy_request_line << CRLF
          @logger.debug("CONNECT: > #{proxy_request_line}")
          if credentials
            @logger.debug("CONNECT: sending a credentials")
            os << "Proxy-Authorization: " << credentials << CRLF
          end
          os << CRLF
          proxy_status_line = os.gets(LF)
          @logger.debug("CONNECT: read a Status-Line form the upstream server")
          @logger.debug("CONNECT: < #{proxy_status_line}")
          if %r{^HTTP/\d+\.\d+\s+200\s*} =~ proxy_status_line
            while line = os.gets(LF)
              break if /\A(#{CRLF}|#{LF})\z/om =~ line
            end
          else
            raise HTTPStatus::BadGateway
          end
        end
        @logger.debug("CONNECT #{host}:#{port}: succeeded")
        res.status = HTTPStatus::RC_OK
      rescue => ex
        @logger.debug("CONNECT #{host}:#{port}: failed `#{ex.message}'")
        res.set_error(ex)
        raise HTTPStatus::EOFError
      ensure
        if handler = @config[:ProxyContentHandler]
          handler.call(req, res)
        end
        res.send_response(ua)
        access_log(@config, req, res)

        # Should clear request-line not to send the sesponse twice.
        # see: HTTPServer#run
        req.parse(NullReader) rescue nil
      end

      begin
        while fds = IO::select([ua, os])
          if fds[0].member?(ua)
            buf = ua.sysread(1024);
            @logger.debug("CONNECT: #{buf.bytesize} byte from User-Agent")
            os.syswrite(buf)
          elsif fds[0].member?(os)
            buf = os.sysread(1024);
            @logger.debug("CONNECT: #{buf.bytesize} byte from #{host}:#{port}")
            ua.syswrite(buf)
          end
        end
      rescue => ex
        os.close
        @logger.debug("CONNECT #{host}:#{port}: closed")
      end

      raise HTTPStatus::EOFError
    end

    def do_GET(req, res)
      perform_proxy_request(req, res) do |http, path, header|
        http.get(path, header)
      end
    end

    def do_HEAD(req, res)
      perform_proxy_request(req, res) do |http, path, header|
        http.head(path, header)
      end
    end

    def do_POST(req, res)
      perform_proxy_request(req, res) do |http, path, header|
        http.post(path, req.body || "", header)
      end
    end

    def do_OPTIONS(req, res)
      res['allow'] = "GET,HEAD,POST,OPTIONS,CONNECT"
    end

    private

    # Some header fields should not be transferred.
    HopByHop = %w( connection keep-alive proxy-authenticate upgrade
                   proxy-authorization te trailers transfer-encoding )
    ShouldNotTransfer = %w( set-cookie proxy-connection )
    def split_field(f) f ? f.split(/,\s+/).collect{|i| i.downcase } : [] end

    def choose_header(src, dst)
      connections = split_field(src['connection'])
      src.each{|key, value|
        key = key.downcase
        if HopByHop.member?(key)          || # RFC2616: 13.5.1
           connections.member?(key)       || # RFC2616: 14.10
           ShouldNotTransfer.member?(key)    # pragmatics
          @logger.debug("choose_header: `#{key}: #{value}'")
          next
        end
        dst[key] = value
      }
    end

    # Net::HTTP is stupid about the multiple header fields.
    # Here is workaround:
    def set_cookie(src, dst)
      if str = src['set-cookie']
        cookies = []
        str.split(/,\s*/).each{|token|
          if /^[^=]+;/o =~ token
            cookies[-1] << ", " << token
          elsif /=/o =~ token
            cookies << token
          else
            cookies[-1] << ", " << token
          end
        }
        dst.cookies.replace(cookies)
      end
    end

    def set_via(h)
      if @config[:ProxyVia]
        if  h['via']
          h['via'] << ", " << @via
        else
          h['via'] = @via
        end
      end
    end

    def setup_proxy_header(req, res)
      # Choose header fields to transfer
      header = Hash.new
      choose_header(req, header)
      set_via(header)
      return header
    end

    def setup_upstream_proxy_authentication(req, res, header)
      if upstream = proxy_uri(req, res)
        if upstream.userinfo
          header['proxy-authorization'] =
            "Basic " + [upstream.userinfo].pack("m").delete("\n")
        end
        return upstream
      end
      return FakeProxyURI
    end

    def perform_proxy_request(req, res)
      uri = req.request_uri
      path = uri.path.dup
      path << "?" << uri.query if uri.query
      header = setup_proxy_header(req, res)
      upstream = setup_upstream_proxy_authentication(req, res, header)
      response = nil

      http = Net::HTTP.new(uri.host, uri.port, upstream.host, upstream.port)
      http.start do
        if @config[:ProxyTimeout]
          ##################################   these issues are
          http.open_timeout = 30   # secs  #   necessary (maybe bacause
          http.read_timeout = 60   # secs  #   Ruby's bug, but why?)
          ##################################
        end
        response = yield(http, path, header)
      end

      # Persistent connection requirements are mysterious for me.
      # So I will close the connection in every response.
      res['proxy-connection'] = "close"
      res['connection'] = "close"

      # Convert Net::HTTP::HTTPResponse to WEBrick::HTTPResponse
      res.status = response.code.to_i
      choose_header(response, res)
      set_cookie(response, res)
      set_via(res)
      res.body = response.body
    end
  end
end
