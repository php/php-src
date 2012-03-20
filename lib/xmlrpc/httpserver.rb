#
# Implements a simple HTTP-server by using John W. Small's (jsmall@laser.net)
# ruby-generic-server.
#
# Copyright (C) 2001, 2002, 2003 by Michael Neumann (mneumann@ntecs.de)
#
# $Id$
#


require "gserver"

class HttpServer < GServer

  ##
  # handle_obj specifies the object, that receives calls to request_handler
  # and ip_auth_handler
  def initialize(handle_obj, port = 8080, host = DEFAULT_HOST, maxConnections = 4,
                 stdlog = $stdout, audit = true, debug = true)
    @handler = handle_obj
    super(port, host, maxConnections, stdlog, audit, debug)
  end

private

  # Constants -----------------------------------------------

  CRLF        = "\r\n"
  HTTP_PROTO  = "HTTP/1.0"
  SERVER_NAME = "HttpServer (Ruby #{RUBY_VERSION})"

  DEFAULT_HEADER = {
    "Server" => SERVER_NAME
  }

  ##
  # Mapping of status code and error message
  #
  StatusCodeMapping = {
    200 => "OK",
    400 => "Bad Request",
    403 => "Forbidden",
    405 => "Method Not Allowed",
    411 => "Length Required",
    500 => "Internal Server Error"
  }

  # Classes -------------------------------------------------

  class Request
    attr_reader :data, :header, :method, :path, :proto

    def initialize(data, method=nil, path=nil, proto=nil)
      @header, @data = Table.new, data
      @method, @path, @proto = method, path, proto
    end

    def content_length
      len = @header['Content-Length']
      return nil if len.nil?
      return len.to_i
    end

  end

  class Response
    attr_reader   :header
    attr_accessor :body, :status, :status_message

    def initialize(status=200)
      @status = status
      @status_message = nil
      @header = Table.new
    end
  end


  ##
  # a case-insensitive Hash class for HTTP header
  #
  class Table
    include Enumerable

    def initialize(hash={})
      @hash = hash
      update(hash)
    end

    def [](key)
      @hash[key.to_s.capitalize]
    end

    def []=(key, value)
      @hash[key.to_s.capitalize] = value
    end

    def update(hash)
      hash.each {|k,v| self[k] = v}
      self
    end

    def each
      @hash.each {|k,v| yield k.capitalize, v }
    end

    def writeTo(port)
      each { |k,v| port << "#{k}: #{v}" << CRLF }
    end
  end # class Table


  # Helper Methods ------------------------------------------

  def http_header(header=nil)
    new_header = Table.new(DEFAULT_HEADER)
    new_header.update(header) unless header.nil?

    new_header["Connection"] = "close"
    new_header["Date"]       = http_date(Time.now)

    new_header
  end

  def http_date( aTime )
    aTime.gmtime.strftime( "%a, %d %b %Y %H:%M:%S GMT" )
  end

  def http_resp(status_code, status_message=nil, header=nil, body=nil)
    status_message ||= StatusCodeMapping[status_code]

    str = ""
    str << "#{HTTP_PROTO} #{status_code} #{status_message}" << CRLF
    http_header(header).writeTo(str)
    str << CRLF
    str << body unless body.nil?
    str
  end

  # Main Serve Loop -----------------------------------------

  def serve(io)
    # perform IP authentification
    unless @handler.ip_auth_handler(io)
      io << http_resp(403, "Forbidden")
      return
    end

    # parse first line
    if io.gets =~ /^(\S+)\s+(\S+)\s+(\S+)/
      request = Request.new(io, $1, $2, $3)
    else
      io << http_resp(400, "Bad Request")
      return
    end

    # parse HTTP headers
    while (line=io.gets) !~ /^(\n|\r)/
      if line =~ /^([\w-]+):\s*(.*)$/
        request.header[$1] = $2.strip
      end
    end

    io.binmode
    response = Response.new

    # execute request handler
    @handler.request_handler(request, response)

    # write response back to the client
    io << http_resp(response.status, response.status_message,
                    response.header, response.body)

  rescue Exception
    io << http_resp(500, "Internal Server Error")
  end

end # class HttpServer

