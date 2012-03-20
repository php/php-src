#
# httpresponse.rb -- HTTPResponse Class
#
# Author: IPR -- Internet Programming with Ruby -- writers
# Copyright (c) 2000, 2001 TAKAHASHI Masayoshi, GOTOU Yuuzou
# Copyright (c) 2002 Internet Programming with Ruby writers. All rights
# reserved.
#
# $IPR: httpresponse.rb,v 1.45 2003/07/11 11:02:25 gotoyuzo Exp $

require 'time'
require 'webrick/httpversion'
require 'webrick/htmlutils'
require 'webrick/httputils'
require 'webrick/httpstatus'

module WEBrick
  ##
  # An HTTP response.

  class HTTPResponse
    attr_reader :http_version, :status, :header
    attr_reader :cookies
    attr_accessor :reason_phrase

    ##
    # Body may be a String or IO subclass.

    attr_accessor :body

    attr_accessor :request_method, :request_uri, :request_http_version
    attr_accessor :filename
    attr_accessor :keep_alive
    attr_reader :config, :sent_size

    ##
    # Creates a new HTTP response object

    def initialize(config)
      @config = config
      @buffer_size = config[:OutputBufferSize]
      @logger = config[:Logger]
      @header = Hash.new
      @status = HTTPStatus::RC_OK
      @reason_phrase = nil
      @http_version = HTTPVersion::convert(@config[:HTTPVersion])
      @body = ''
      @keep_alive = true
      @cookies = []
      @request_method = nil
      @request_uri = nil
      @request_http_version = @http_version  # temporary
      @chunked = false
      @filename = nil
      @sent_size = 0
    end

    ##
    # The response's HTTP status line

    def status_line
      "HTTP/#@http_version #@status #@reason_phrase #{CRLF}"
    end

    ##
    # Sets the response's status to the +status+ code

    def status=(status)
      @status = status
      @reason_phrase = HTTPStatus::reason_phrase(status)
    end

    ##
    # Retrieves the response header +field+

    def [](field)
      @header[field.downcase]
    end

    ##
    # Sets the response header +field+ to +value+

    def []=(field, value)
      @header[field.downcase] = value.to_s
    end

    ##
    # The content-length header

    def content_length
      if len = self['content-length']
        return Integer(len)
      end
    end

    ##
    # Sets the content-length header to +len+

    def content_length=(len)
      self['content-length'] = len.to_s
    end

    ##
    # The content-type header

    def content_type
      self['content-type']
    end

    ##
    # Sets the content-type header to +type+

    def content_type=(type)
      self['content-type'] = type
    end

    ##
    # Iterates over each header in the resopnse

    def each
      @header.each{|field, value|  yield(field, value) }
    end

    ##
    # Will this response body be returned using chunked transfer-encoding?

    def chunked?
      @chunked
    end

    ##
    # Enables chunked transfer encoding.

    def chunked=(val)
      @chunked = val ? true : false
    end

    ##
    # Will this response's connection be kept alive?

    def keep_alive?
      @keep_alive
    end

    ##
    # Sends the response on +socket+

    def send_response(socket)
      begin
        setup_header()
        send_header(socket)
        send_body(socket)
      rescue Errno::EPIPE, Errno::ECONNRESET, Errno::ENOTCONN => ex
        @logger.debug(ex)
        @keep_alive = false
      rescue Exception => ex
        @logger.error(ex)
        @keep_alive = false
      end
    end

    ##
    # Sets up the headers for sending

    def setup_header()
      @reason_phrase    ||= HTTPStatus::reason_phrase(@status)
      @header['server'] ||= @config[:ServerSoftware]
      @header['date']   ||= Time.now.httpdate

      # HTTP/0.9 features
      if @request_http_version < "1.0"
        @http_version = HTTPVersion.new("0.9")
        @keep_alive = false
      end

      # HTTP/1.0 features
      if @request_http_version < "1.1"
        if chunked?
          @chunked = false
          ver = @request_http_version.to_s
          msg = "chunked is set for an HTTP/#{ver} request. (ignored)"
          @logger.warn(msg)
        end
      end

      # Determine the message length (RFC2616 -- 4.4 Message Length)
      if @status == 304 || @status == 204 || HTTPStatus::info?(@status)
        @header.delete('content-length')
        @body = ""
      elsif chunked?
        @header["transfer-encoding"] = "chunked"
        @header.delete('content-length')
      elsif %r{^multipart/byteranges} =~ @header['content-type']
        @header.delete('content-length')
      elsif @header['content-length'].nil?
        unless @body.is_a?(IO)
          @header['content-length'] = @body ? @body.bytesize : 0
        end
      end

      # Keep-Alive connection.
      if @header['connection'] == "close"
         @keep_alive = false
      elsif keep_alive?
        if chunked? || @header['content-length'] || @status == 304 || @status == 204 || HTTPStatus.info?(@status)
          @header['connection'] = "Keep-Alive"
        else
          msg = "Could not determine content-length of response body. Set content-length of the response or set Response#chunked = true"
          @logger.warn(msg)
          @header['connection'] = "close"
          @keep_alive = false
        end
      else
        @header['connection'] = "close"
      end

      # Location is a single absoluteURI.
      if location = @header['location']
        if @request_uri
          @header['location'] = @request_uri.merge(location)
        end
      end
    end

    ##
    # Sends the headers on +socket+

    def send_header(socket)
      if @http_version.major > 0
        data = status_line()
        @header.each{|key, value|
          tmp = key.gsub(/\bwww|^te$|\b\w/){ $&.upcase }
          data << "#{tmp}: #{value}" << CRLF
        }
        @cookies.each{|cookie|
          data << "Set-Cookie: " << cookie.to_s << CRLF
        }
        data << CRLF
        _write_data(socket, data)
      end
    end

    ##
    # Sends the body on +socket+

    def send_body(socket)
      case @body
      when IO then send_body_io(socket)
      else send_body_string(socket)
      end
    end

    def to_s # :nodoc:
      ret = ""
      send_response(ret)
      ret
    end

    ##
    # Redirects to +url+ with a WEBrick::HTTPStatus::Redirect +status+.
    #
    # Example:
    #
    #   res.set_redirect WEBrick::HTTPStatus::TemporaryRedirect

    def set_redirect(status, url)
      @body = "<HTML><A HREF=\"#{url.to_s}\">#{url.to_s}</A>.</HTML>\n"
      @header['location'] = url.to_s
      raise status
    end

    ##
    # Creates an error page for exception +ex+ with an optional +backtrace+

    def set_error(ex, backtrace=false)
      case ex
      when HTTPStatus::Status
        @keep_alive = false if HTTPStatus::error?(ex.code)
        self.status = ex.code
      else
        @keep_alive = false
        self.status = HTTPStatus::RC_INTERNAL_SERVER_ERROR
      end
      @header['content-type'] = "text/html; charset=ISO-8859-1"

      if respond_to?(:create_error_page)
        create_error_page()
        return
      end

      if @request_uri
        host, port = @request_uri.host, @request_uri.port
      else
        host, port = @config[:ServerName], @config[:Port]
      end

      @body = ''
      @body << <<-_end_of_html_
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0//EN">
<HTML>
  <HEAD><TITLE>#{HTMLUtils::escape(@reason_phrase)}</TITLE></HEAD>
  <BODY>
    <H1>#{HTMLUtils::escape(@reason_phrase)}</H1>
    #{HTMLUtils::escape(ex.message)}
    <HR>
      _end_of_html_

      if backtrace && $DEBUG
        @body << "backtrace of `#{HTMLUtils::escape(ex.class.to_s)}' "
        @body << "#{HTMLUtils::escape(ex.message)}"
        @body << "<PRE>"
        ex.backtrace.each{|line| @body << "\t#{line}\n"}
        @body << "</PRE><HR>"
      end

      @body << <<-_end_of_html_
    <ADDRESS>
     #{HTMLUtils::escape(@config[:ServerSoftware])} at
     #{host}:#{port}
    </ADDRESS>
  </BODY>
</HTML>
      _end_of_html_
    end

    private

    def send_body_io(socket)
      begin
        if @request_method == "HEAD"
          # do nothing
        elsif chunked?
          while buf = @body.read(@buffer_size)
            next if buf.empty?
            data = ""
            data << format("%x", buf.bytesize) << CRLF
            data << buf << CRLF
            _write_data(socket, data)
            @sent_size += buf.bytesize
          end
          _write_data(socket, "0#{CRLF}#{CRLF}")
        else
          size = @header['content-length'].to_i
          _send_file(socket, @body, 0, size)
          @sent_size = size
        end
      ensure
        @body.close
      end
    end

    def send_body_string(socket)
      if @request_method == "HEAD"
        # do nothing
      elsif chunked?
        body ? @body.bytesize : 0
        while buf = @body[@sent_size, @buffer_size]
          break if buf.empty?
          data = ""
          data << format("%x", buf.bytesize) << CRLF
          data << buf << CRLF
          _write_data(socket, data)
          @sent_size += buf.bytesize
        end
        _write_data(socket, "0#{CRLF}#{CRLF}")
      else
        if @body && @body.bytesize > 0
          _write_data(socket, @body)
          @sent_size = @body.bytesize
        end
      end
    end

    def _send_file(output, input, offset, size)
      while offset > 0
        sz = @buffer_size < size ? @buffer_size : size
        buf = input.read(sz)
        offset -= buf.bytesize
      end

      if size == 0
        while buf = input.read(@buffer_size)
          _write_data(output, buf)
        end
      else
        while size > 0
          sz = @buffer_size < size ? @buffer_size : size
          buf = input.read(sz)
          _write_data(output, buf)
          size -= buf.bytesize
        end
      end
    end

    def _write_data(socket, data)
      socket << data
    end
  end
end
