#
# cgi.rb -- Yet another CGI library
#
# Author: IPR -- Internet Programming with Ruby -- writers
# Copyright (c) 2003 Internet Programming with Ruby writers. All rights
# reserved.
#
# $Id$

require "webrick/httprequest"
require "webrick/httpresponse"
require "webrick/config"
require "stringio"

module WEBrick
  class CGI
    CGIError = Class.new(StandardError)

    attr_reader :config, :logger

    def initialize(*args)
      if defined?(MOD_RUBY)
        unless ENV.has_key?("GATEWAY_INTERFACE")
          Apache.request.setup_cgi_env
        end
      end
      if %r{HTTP/(\d+\.\d+)} =~ ENV["SERVER_PROTOCOL"]
        httpv = $1
      end
      @config = WEBrick::Config::HTTP.dup.update(
        :ServerSoftware => ENV["SERVER_SOFTWARE"] || "null",
        :HTTPVersion    => HTTPVersion.new(httpv || "1.0"),
        :RunOnCGI       => true,   # to detect if it runs on CGI.
        :NPH            => false   # set true to run as NPH script.
      )
      if config = args.shift
        @config.update(config)
      end
      @config[:Logger] ||= WEBrick::BasicLog.new($stderr)
      @logger = @config[:Logger]
      @options = args
    end

    def [](key)
      @config[key]
    end

    def start(env=ENV, stdin=$stdin, stdout=$stdout)
      sock = WEBrick::CGI::Socket.new(@config, env, stdin, stdout)
      req = HTTPRequest.new(@config)
      res = HTTPResponse.new(@config)
      unless @config[:NPH] or defined?(MOD_RUBY)
        def res.setup_header
          unless @header["status"]
            phrase = HTTPStatus::reason_phrase(@status)
            @header["status"] = "#{@status} #{phrase}"
          end
          super
        end
        def res.status_line
          ""
        end
      end

      begin
        req.parse(sock)
        req.script_name = (env["SCRIPT_NAME"] || File.expand_path($0)).dup
        req.path_info = (env["PATH_INFO"] || "").dup
        req.query_string = env["QUERY_STRING"]
        req.user = env["REMOTE_USER"]
        res.request_method = req.request_method
        res.request_uri = req.request_uri
        res.request_http_version = req.http_version
        res.keep_alive = req.keep_alive?
        self.service(req, res)
      rescue HTTPStatus::Error => ex
        res.set_error(ex)
      rescue HTTPStatus::Status => ex
        res.status = ex.code
      rescue Exception => ex
        @logger.error(ex)
        res.set_error(ex, true)
      ensure
        req.fixup
        if defined?(MOD_RUBY)
          res.setup_header
          Apache.request.status_line = "#{res.status} #{res.reason_phrase}"
          Apache.request.status = res.status
          table = Apache.request.headers_out
          res.header.each{|key, val|
            case key
            when /^content-encoding$/i
              Apache::request.content_encoding = val
            when /^content-type$/i
              Apache::request.content_type = val
            else
              table[key] = val.to_s
            end
          }
          res.cookies.each{|cookie|
            table.add("Set-Cookie", cookie.to_s)
          }
          Apache.request.send_http_header
          res.send_body(sock)
        else
          res.send_response(sock)
        end
      end
    end

    def service(req, res)
      method_name = "do_" + req.request_method.gsub(/-/, "_")
      if respond_to?(method_name)
        __send__(method_name, req, res)
      else
        raise HTTPStatus::MethodNotAllowed,
              "unsupported method `#{req.request_method}'."
      end
    end

    class Socket
      include Enumerable

      private

      def initialize(config, env, stdin, stdout)
        @config = config
        @env = env
        @header_part = StringIO.new
        @body_part = stdin
        @out_port = stdout
        @out_port.binmode

        @server_addr = @env["SERVER_ADDR"] || "0.0.0.0"
        @server_name = @env["SERVER_NAME"]
        @server_port = @env["SERVER_PORT"]
        @remote_addr = @env["REMOTE_ADDR"]
        @remote_host = @env["REMOTE_HOST"] || @remote_addr
        @remote_port = @env["REMOTE_PORT"] || 0

        begin
          @header_part << request_line << CRLF
          setup_header
          @header_part << CRLF
          @header_part.rewind
        rescue Exception
          raise CGIError, "invalid CGI environment"
        end
      end

      def request_line
        meth = @env["REQUEST_METHOD"] || "GET"
        unless url = @env["REQUEST_URI"]
          url = (@env["SCRIPT_NAME"] || File.expand_path($0)).dup
          url << @env["PATH_INFO"].to_s
          url = WEBrick::HTTPUtils.escape_path(url)
          if query_string = @env["QUERY_STRING"]
            unless query_string.empty?
              url << "?" << query_string
            end
          end
        end
        # we cannot get real HTTP version of client ;)
        httpv = @config[:HTTPVersion]
        return "#{meth} #{url} HTTP/#{httpv}"
      end

      def setup_header
        @env.each{|key, value|
          case key
          when "CONTENT_TYPE", "CONTENT_LENGTH"
            add_header(key.gsub(/_/, "-"), value)
          when /^HTTP_(.*)/
            add_header($1.gsub(/_/, "-"), value)
          end
        }
      end

      def add_header(hdrname, value)
        unless value.empty?
          @header_part << hdrname << ": " << value << CRLF
        end
      end

      def input
        @header_part.eof? ? @body_part : @header_part
      end

      public

      def peeraddr
        [nil, @remote_port, @remote_host, @remote_addr]
      end

      def addr
        [nil, @server_port, @server_name, @server_addr]
      end

      def gets(eol=LF, size=nil)
        input.gets(eol, size)
      end

      def read(size=nil)
        input.read(size)
      end

      def each
        input.each{|line| yield(line) }
      end

      def eof?
        input.eof?
      end

      def <<(data)
        @out_port << data
      end

      def cert
        return nil unless defined?(OpenSSL)
        if pem = @env["SSL_SERVER_CERT"]
          OpenSSL::X509::Certificate.new(pem) unless pem.empty?
        end
      end

      def peer_cert
        return nil unless defined?(OpenSSL)
        if pem = @env["SSL_CLIENT_CERT"]
          OpenSSL::X509::Certificate.new(pem) unless pem.empty?
        end
      end

      def peer_cert_chain
        return nil unless defined?(OpenSSL)
        if @env["SSL_CLIENT_CERT_CHAIN_0"]
          keys = @env.keys
          certs = keys.sort.collect{|k|
            if /^SSL_CLIENT_CERT_CHAIN_\d+$/ =~ k
              if pem = @env[k]
                OpenSSL::X509::Certificate.new(pem) unless pem.empty?
              end
            end
          }
          certs.compact
        end
      end

      def cipher
        return nil unless defined?(OpenSSL)
        if cipher = @env["SSL_CIPHER"]
          ret = [ cipher ]
          ret << @env["SSL_PROTOCOL"]
          ret << @env["SSL_CIPHER_USEKEYSIZE"]
          ret << @env["SSL_CIPHER_ALGKEYSIZE"]
          ret
        end
      end
    end
  end
end
