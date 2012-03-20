#
# = net/http.rb
#
# Copyright (c) 1999-2007 Yukihiro Matsumoto
# Copyright (c) 1999-2007 Minero Aoki
# Copyright (c) 2001 GOTOU Yuuzou
#
# Written and maintained by Minero Aoki <aamine@loveruby.net>.
# HTTPS support added by GOTOU Yuuzou <gotoyuzo@notwork.org>.
#
# This file is derived from "http-access.rb".
#
# Documented by Minero Aoki; converted to RDoc by William Webber.
#
# This program is free software. You can re-distribute and/or
# modify this program under the same terms of ruby itself ---
# Ruby Distribution License or GNU General Public License.
#
# See Net::HTTP for an overview and examples.
#

require 'net/protocol'
require 'uri'
autoload :OpenSSL, 'openssl'

module Net   #:nodoc:

  # :stopdoc:
  class HTTPBadResponse < StandardError; end
  class HTTPHeaderSyntaxError < StandardError; end
  # :startdoc:

  # == An HTTP client API for Ruby.
  #
  # Net::HTTP provides a rich library which can be used to build HTTP
  # user-agents.  For more details about HTTP see
  # [RFC2616](http://www.ietf.org/rfc/rfc2616.txt)
  #
  # Net::HTTP is designed to work closely with URI.  URI::HTTP#host,
  # URI::HTTP#port and URI::HTTP#request_uri are designed to work with
  # Net::HTTP.
  #
  # If you are only performing a few GET requests you should try OpenURI.
  #
  # == Simple Examples
  #
  # All examples assume you have loaded Net::HTTP with:
  #
  #   require 'net/http'
  #
  # This will also require 'uri' so you don't need to require it separately.
  #
  # The Net::HTTP methods in the following section do not persist
  # connections.  They are not recommended if you are performing many HTTP
  # requests.
  #
  # === GET
  #
  #   Net::HTTP.get('example.com', '/index.html') # => String
  #
  # === GET by URI
  #
  #   uri = URI('http://example.com/index.html?count=10')
  #   Net::HTTP.get(uri) # => String
  #
  # === GET with Dynamic Parameters
  #
  #   uri = URI('http://example.com/index.html')
  #   params = { :limit => 10, :page => 3 }
  #   uri.query = URI.encode_www_form(params)
  #
  #   res = Net::HTTP.get_response(uri)
  #   puts res.body if res.is_a?(Net::HTTPSuccess)
  #
  # === POST
  #
  #   uri = URI('http://www.example.com/search.cgi')
  #   res = Net::HTTP.post_form(uri, 'q' => 'ruby', 'max' => '50')
  #   puts res.body
  #
  # === POST with Multiple Values
  #
  #   uri = URI('http://www.example.com/search.cgi')
  #   res = Net::HTTP.post_form(uri, 'q' => ['ruby', 'perl'], 'max' => '50')
  #   puts res.body
  #
  # == How to use Net::HTTP
  #
  # The following example code can be used as the basis of a HTTP user-agent
  # which can perform a variety of request types using persistent
  # connections.
  #
  #   uri = URI('http://example.com/some_path?query=string')
  #
  #   Net::HTTP.start(uri.host, uri.port) do |http|
  #     request = Net::HTTP::Get.new uri.request_uri
  #
  #     response = http.request request # Net::HTTPResponse object
  #   end
  #
  # Net::HTTP::start immediately creates a connection to an HTTP server which
  # is kept open for the duration of the block.  The connection will remain
  # open for multiple requests in the block if the server indicates it
  # supports persistent connections.
  #
  # The request types Net::HTTP supports are listed below in the section "HTTP
  # Request Classes".
  #
  # If you wish to re-use a connection across multiple HTTP requests without
  # automatically closing it you can use ::new instead of ::start.  #request
  # will automatically open a connection to the server if one is not currently
  # open.  You can manually close the connection with #finish.
  #
  # === Response Data
  #
  #   uri = URI('http://example.com/index.html')
  #   res = Net::HTTP.get_response(uri)
  #
  #   # Headers
  #   res['Set-Cookie']            # => String
  #   res.get_fields('set-cookie') # => Array
  #   res.to_hash['set-cookie']    # => Array
  #   puts "Headers: #{res.to_hash.inspect}"
  #
  #   # Status
  #   puts res.code       # => '200'
  #   puts res.message    # => 'OK'
  #   puts res.class.name # => 'HTTPOK'
  #
  #   # Body
  #   puts res.body if res.response_body_permitted?
  #
  # === Following Redirection
  #
  # Each Net::HTTPResponse object belongs to a class for its response code.
  #
  # For example, all 2XX responses are instances of a Net::HTTPSuccess
  # subclass, a 3XX response is an instance of a Net::HTTPRedirection
  # subclass and a 200 response is an instance of the Net::HTTPOK class.  For
  # details of response classes, see the section "HTTP Response Classes"
  # below.
  #
  # Using a case statement you can handle various types of responses properly:
  #
  #   def fetch(uri_str, limit = 10)
  #     # You should choose a better exception.
  #     raise ArgumentError, 'too many HTTP redirects' if limit == 0
  #
  #     response = Net::HTTP.get_response(URI(uri_str))
  #
  #     case response
  #     when Net::HTTPSuccess then
  #       response
  #     when Net::HTTPRedirection then
  #       location = response['location']
  #       warn "redirected to #{location}"
  #       fetch(location, limit - 1)
  #     else
  #       response.value
  #     end
  #   end
  #
  #   print fetch('http://www.ruby-lang.org')
  #
  # === POST
  #
  # A POST can be made using the Net::HTTP::Post request class.  This example
  # creates a urlencoded POST body:
  #
  #   uri = URI('http://www.example.com/todo.cgi')
  #   req = Net::HTTP::Post.new(uri.path)
  #   req.set_form_data('from' => '2005-01-01', 'to' => '2005-03-31')
  #
  #   res = Net::HTTP.start(uri.hostname, uri.port) do |http|
  #     http.request(req)
  #   end
  #
  #   case res
  #   when Net::HTTPSuccess, Net::HTTPRedirection
  #     # OK
  #   else
  #     res.value
  #   end
  #
  # At this time Net::HTTP does not support multipart/form-data.  To send
  # multipart/form-data use Net::HTTPRequest#body= and
  # Net::HTTPRequest#content_type=:
  #
  #   req = Net::HTTP::Post.new(uri.path)
  #   req.body = multipart_data
  #   req.content_type = 'multipart/form-data'
  #
  # Other requests that can contain a body such as PUT can be created in the
  # same way using the corresponding request class (Net::HTTP::Put).
  #
  # === Setting Headers
  #
  # The following example performs a conditional GET using the
  # If-Modified-Since header.  If the files has not been modified since the
  # time in the header a Not Modified response will be returned.  See RFC 2616
  # section 9.3 for further details.
  #
  #   uri = URI('http://example.com/cached_response')
  #   file = File.stat 'cached_response'
  #
  #   req = Net::HTTP::Get.new(uri.request_uri)
  #   req['If-Modified-Since'] = file.mtime.rfc2822
  #
  #   res = Net::HTTP.start(uri.hostname, uri.port) {|http|
  #     http.request(req)
  #   }
  #
  #   open 'cached_response', 'w' do |io|
  #     io.write res.body
  #   end if res.is_a?(Net::HTTPSuccess)
  #
  # === Basic Authentication
  #
  # Basic authentication is performed according to
  # [RFC2617](http://www.ietf.org/rfc/rfc2617.txt)
  #
  #   uri = URI('http://example.com/index.html?key=value')
  #
  #   req = Net::HTTP::Get.new(uri.request_uri)
  #   req.basic_auth 'user', 'pass'
  #
  #   res = Net::HTTP.start(uri.hostname, uri.port) {|http|
  #     http.request(req)
  #   }
  #   puts res.body
  #
  # === Streaming Response Bodies
  #
  # By default Net::HTTP reads an entire response into memory.  If you are
  # handling large files or wish to implement a progress bar you can instead
  # stream the body directly to an IO.
  #
  #   uri = URI('http://example.com/large_file')
  #
  #   Net::HTTP.start(uri.host, uri.port) do |http|
  #     request = Net::HTTP::Get.new uri.request_uri
  #
  #     http.request request do |response|
  #       open 'large_file', 'w' do |io|
  #         response.read_body do |chunk|
  #           io.write chunk
  #         end
  #       end
  #     end
  #   end
  #
  # === HTTPS
  #
  # HTTPS is enabled for an HTTP connection by Net::HTTP#use_ssl=.
  #
  #   uri = URI('https://secure.example.com/some_path?query=string')
  #
  #   Net::HTTP.start(uri.host, uri.port,
  #     :use_ssl => uri.scheme == 'https') do |http|
  #     request = Net::HTTP::Get.new uri.request_uri
  #
  #     response = http.request request # Net::HTTPResponse object
  #   end
  #
  # In previous versions of ruby you would need to require 'net/https' to use
  # HTTPS.  This is no longer true.
  #
  # === Proxies
  #
  # Net::HTTP::Proxy has the same methods as Net::HTTP but its instances always
  # connect via the proxy instead of directly to the given host.
  #
  #   proxy_addr = 'your.proxy.host'
  #   proxy_port = 8080
  #
  #   Net::HTTP::Proxy(proxy_addr, proxy_port).start('www.example.com') {|http|
  #     # always connect to your.proxy.addr:8080
  #   }
  #
  # Net::HTTP::Proxy returns a Net::HTTP instance when proxy_addr is nil so
  # there is no need for conditional code.
  #
  # See Net::HTTP::Proxy for further details and examples such as proxies that
  # require a username and password.
  #
  # == HTTP Request Classes
  #
  # Here is the HTTP request class hierarchy.
  #
  # * Net::HTTPRequest
  #   * Net::HTTP::Get
  #   * Net::HTTP::Head
  #   * Net::HTTP::Post
  #   * Net::HTTP::Put
  #   * Net::HTTP::Proppatch
  #   * Net::HTTP::Lock
  #   * Net::HTTP::Unlock
  #   * Net::HTTP::Options
  #   * Net::HTTP::Propfind
  #   * Net::HTTP::Delete
  #   * Net::HTTP::Move
  #   * Net::HTTP::Copy
  #   * Net::HTTP::Mkcol
  #   * Net::HTTP::Trace
  #
  # == HTTP Response Classes
  #
  # Here is HTTP response class hierarchy.  All classes are defined in Net
  # module and are subclasses of Net::HTTPResponse.
  #
  # HTTPUnknownResponse:: For unhandled HTTP extensions
  # HTTPInformation::                    1xx
  #   HTTPContinue::                        100
  #   HTTPSwitchProtocol::                  101
  # HTTPSuccess::                        2xx
  #   HTTPOK::                              200
  #   HTTPCreated::                         201
  #   HTTPAccepted::                        202
  #   HTTPNonAuthoritativeInformation::     203
  #   HTTPNoContent::                       204
  #   HTTPResetContent::                    205
  #   HTTPPartialContent::                  206
  # HTTPRedirection::                    3xx
  #   HTTPMultipleChoice::                  300
  #   HTTPMovedPermanently::                301
  #   HTTPFound::                           302
  #   HTTPSeeOther::                        303
  #   HTTPNotModified::                     304
  #   HTTPUseProxy::                        305
  #   HTTPTemporaryRedirect::               307
  # HTTPClientError::                    4xx
  #   HTTPBadRequest::                      400
  #   HTTPUnauthorized::                    401
  #   HTTPPaymentRequired::                 402
  #   HTTPForbidden::                       403
  #   HTTPNotFound::                        404
  #   HTTPMethodNotAllowed::                405
  #   HTTPNotAcceptable::                   406
  #   HTTPProxyAuthenticationRequired::     407
  #   HTTPRequestTimeOut::                  408
  #   HTTPConflict::                        409
  #   HTTPGone::                            410
  #   HTTPLengthRequired::                  411
  #   HTTPPreconditionFailed::              412
  #   HTTPRequestEntityTooLarge::           413
  #   HTTPRequestURITooLong::               414
  #   HTTPUnsupportedMediaType::            415
  #   HTTPRequestedRangeNotSatisfiable::    416
  #   HTTPExpectationFailed::               417
  # HTTPServerError::                    5xx
  #   HTTPInternalServerError::             500
  #   HTTPNotImplemented::                  501
  #   HTTPBadGateway::                      502
  #   HTTPServiceUnavailable::              503
  #   HTTPGatewayTimeOut::                  504
  #   HTTPVersionNotSupported::             505
  #
  # There is also the Net::HTTPBadResponse exception which is raised when
  # there is a protocol error.
  #
  class HTTP < Protocol

    # :stopdoc:
    Revision = %q$Revision$.split[1]
    HTTPVersion = '1.1'
    begin
      require 'zlib'
      require 'stringio'  #for our purposes (unpacking gzip) lump these together
      HAVE_ZLIB=true
    rescue LoadError
      HAVE_ZLIB=false
    end
    # :startdoc:

    # Turns on net/http 1.2 (ruby 1.8) features.
    # Defaults to ON in ruby 1.8 or later.
    def HTTP.version_1_2
      true
    end

    # Returns true if net/http is in version 1.2 mode.
    # Defaults to true.
    def HTTP.version_1_2?
      true
    end

    def HTTP.version_1_1?  #:nodoc:
      false
    end

    class << HTTP
      alias is_version_1_1? version_1_1?   #:nodoc:
      alias is_version_1_2? version_1_2?   #:nodoc:
    end

    #
    # short cut methods
    #

    #
    # Gets the body text from the target and outputs it to $stdout.  The
    # target can either be specified as
    # (+uri+), or as (+host+, +path+, +port+ = 80); so:
    #
    #    Net::HTTP.get_print URI('http://www.example.com/index.html')
    #
    # or:
    #
    #    Net::HTTP.get_print 'www.example.com', '/index.html'
    #
    def HTTP.get_print(uri_or_host, path = nil, port = nil)
      get_response(uri_or_host, path, port) {|res|
        res.read_body do |chunk|
          $stdout.print chunk
        end
      }
      nil
    end

    # Sends a GET request to the target and returns the HTTP response
    # as a string.  The target can either be specified as
    # (+uri+), or as (+host+, +path+, +port+ = 80); so:
    #
    #    print Net::HTTP.get(URI('http://www.example.com/index.html'))
    #
    # or:
    #
    #    print Net::HTTP.get('www.example.com', '/index.html')
    #
    def HTTP.get(uri_or_host, path = nil, port = nil)
      get_response(uri_or_host, path, port).body
    end

    # Sends a GET request to the target and returns the HTTP response
    # as a Net::HTTPResponse object.  The target can either be specified as
    # (+uri+), or as (+host+, +path+, +port+ = 80); so:
    #
    #    res = Net::HTTP.get_response(URI('http://www.example.com/index.html'))
    #    print res.body
    #
    # or:
    #
    #    res = Net::HTTP.get_response('www.example.com', '/index.html')
    #    print res.body
    #
    def HTTP.get_response(uri_or_host, path = nil, port = nil, &block)
      if path
        host = uri_or_host
        new(host, port || HTTP.default_port).start {|http|
          return http.request_get(path, &block)
        }
      else
        uri = uri_or_host
        start(uri.hostname, uri.port,
              :use_ssl => uri.scheme == 'https') {|http|
          return http.request_get(uri.request_uri, &block)
        }
      end
    end

    # Posts HTML form data to the specified URI object.
    # The form data must be provided as a Hash mapping from String to String.
    # Example:
    #
    #   { "cmd" => "search", "q" => "ruby", "max" => "50" }
    #
    # This method also does Basic Authentication iff +url+.user exists.
    # But userinfo for authentication is deprecated (RFC3986).
    # So this feature will be removed.
    #
    # Example:
    #
    #   require 'net/http'
    #   require 'uri'
    #
    #   HTTP.post_form URI('http://www.example.com/search.cgi'),
    #                  { "q" => "ruby", "max" => "50" }
    #
    def HTTP.post_form(url, params)
      req = Post.new(url.request_uri)
      req.form_data = params
      req.basic_auth url.user, url.password if url.user
      start(url.hostname, url.port,
            :use_ssl => url.scheme == 'https' ) {|http|
        http.request(req)
      }
    end

    #
    # HTTP session management
    #

    # The default port to use for HTTP requests; defaults to 80.
    def HTTP.default_port
      http_default_port()
    end

    # The default port to use for HTTP requests; defaults to 80.
    def HTTP.http_default_port
      80
    end

    # The default port to use for HTTPS requests; defaults to 443.
    def HTTP.https_default_port
      443
    end

    def HTTP.socket_type   #:nodoc: obsolete
      BufferedIO
    end

    # :call-seq:
    #   HTTP.start(address, port, p_addr, p_port, p_user, p_pass, &block)
    #   HTTP.start(address, port=nil, p_addr=nil, p_port=nil, p_user=nil, p_pass=nil, opt, &block)
    #
    # Creates a new Net::HTTP object, then additionally opens the TCP
    # connection and HTTP session.
    #
    # Arguments are the following:
    # _address_ :: hostname or IP address of the server
    # _port_    :: port of the server
    # _p_addr_  :: address of proxy
    # _p_port_  :: port of proxy
    # _p_user_  :: user of proxy
    # _p_pass_  :: pass of proxy
    # _opt_     :: optional hash
    #
    # _opt_ sets following values by its accessor.
    # The keys are ca_file, ca_path, cert, cert_store, ciphers,
    # close_on_empty_response, key, open_timeout, read_timeout, ssl_timeout,
    # ssl_version, use_ssl, verify_callback, verify_depth and verify_mode.
    # If you set :use_ssl as true, you can use https and default value of
    # verify_mode is set as OpenSSL::SSL::VERIFY_PEER.
    #
    # If the optional block is given, the newly
    # created Net::HTTP object is passed to it and closed when the
    # block finishes.  In this case, the return value of this method
    # is the return value of the block.  If no block is given, the
    # return value of this method is the newly created Net::HTTP object
    # itself, and the caller is responsible for closing it upon completion
    # using the finish() method.
    def HTTP.start(address, *arg, &block) # :yield: +http+
      arg.pop if opt = Hash.try_convert(arg[-1])
      port, p_addr, p_port, p_user, p_pass = *arg
      port = https_default_port if !port && opt && opt[:use_ssl]
      http = new(address, port, p_addr, p_port, p_user, p_pass)

      if opt
        if opt[:use_ssl]
          opt = {verify_mode: OpenSSL::SSL::VERIFY_PEER}.update(opt)
        end
        http.methods.grep(/\A(\w+)=\z/) do |meth|
          key = $1.to_sym
          opt.key?(key) or next
          http.__send__(meth, opt[key])
        end
      end

      http.start(&block)
    end

    class << HTTP
      alias newobj new
    end

    # Creates a new Net::HTTP object without opening a TCP connection or
    # HTTP session.
    # The +address+ should be a DNS hostname or IP address.
    # If +p_addr+ is given, creates a Net::HTTP object with proxy support.
    def HTTP.new(address, port = nil, p_addr = nil, p_port = nil, p_user = nil, p_pass = nil)
      Proxy(p_addr, p_port, p_user, p_pass).newobj(address, port)
    end

    # Creates a new Net::HTTP object for the specified server address,
    # without opening the TCP connection or initializing the HTTP session.
    # The +address+ should be a DNS hostname or IP address.
    def initialize(address, port = nil)
      @address = address
      @port    = (port || HTTP.default_port)
      @curr_http_version = HTTPVersion
      @keep_alive_timeout = 2
      @last_communicated = nil
      @close_on_empty_response = false
      @socket  = nil
      @started = false
      @open_timeout = nil
      @read_timeout = 60
      @continue_timeout = nil
      @debug_output = nil
      @use_ssl = false
      @ssl_context = nil
      @enable_post_connection_check = true
      @compression = nil
      @sspi_enabled = false
      SSL_IVNAMES.each do |ivname|
        instance_variable_set ivname, nil
      end
    end

    def inspect
      "#<#{self.class} #{@address}:#{@port} open=#{started?}>"
    end

    # *WARNING* This method opens a serious security hole.
    # Never use this method in production code.
    #
    # Sets an output stream for debugging.
    #
    #   http = Net::HTTP.new
    #   http.set_debug_output $stderr
    #   http.start { .... }
    #
    def set_debug_output(output)
      warn 'Net::HTTP#set_debug_output called after HTTP started' if started?
      @debug_output = output
    end

    # The DNS host name or IP address to connect to.
    attr_reader :address

    # The port number to connect to.
    attr_reader :port

    # Number of seconds to wait for the connection to open. Any number
    # may be used, including Floats for fractional seconds. If the HTTP
    # object cannot open a connection in this many seconds, it raises a
    # TimeoutError exception.
    attr_accessor :open_timeout

    # Number of seconds to wait for one block to be read (via one read(2)
    # call). Any number may be used, including Floats for fractional
    # seconds. If the HTTP object cannot read data in this many seconds,
    # it raises a TimeoutError exception.
    attr_reader :read_timeout

    # Setter for the read_timeout attribute.
    def read_timeout=(sec)
      @socket.read_timeout = sec if @socket
      @read_timeout = sec
    end

    # Seconds to wait for 100 Continue response.  If the HTTP object does not
    # receive a response in this many seconds it sends the request body.
    attr_reader :continue_timeout

    # Setter for the continue_timeout attribute.
    def continue_timeout=(sec)
      @socket.continue_timeout = sec if @socket
      @continue_timeout = sec
    end

    # Seconds to reuse the connection of the previous request.
    # If the idle time is less than this Keep-Alive Timeout,
    # Net::HTTP reuses the TCP/IP socket used by the previous communication.
    # The default value is 2 seconds.
    attr_accessor :keep_alive_timeout

    # Returns true if the HTTP session has been started.
    def started?
      @started
    end

    alias active? started?   #:nodoc: obsolete

    attr_accessor :close_on_empty_response

    # Returns true if SSL/TLS is being used with HTTP.
    def use_ssl?
      @use_ssl
    end

    # Turn on/off SSL.
    # This flag must be set before starting session.
    # If you change use_ssl value after session started,
    # a Net::HTTP object raises IOError.
    def use_ssl=(flag)
      flag = flag ? true : false
      if started? and @use_ssl != flag
        raise IOError, "use_ssl value changed, but session already started"
      end
      @use_ssl = flag
    end

    SSL_IVNAMES = [
      :@ca_file,
      :@ca_path,
      :@cert,
      :@cert_store,
      :@ciphers,
      :@key,
      :@ssl_timeout,
      :@ssl_version,
      :@verify_callback,
      :@verify_depth,
      :@verify_mode,
    ]
    SSL_ATTRIBUTES = [
      :ca_file,
      :ca_path,
      :cert,
      :cert_store,
      :ciphers,
      :key,
      :ssl_timeout,
      :ssl_version,
      :verify_callback,
      :verify_depth,
      :verify_mode,
    ]

    # Sets path of a CA certification file in PEM format.
    #
    # The file can contain several CA certificates.
    attr_accessor :ca_file

    # Sets path of a CA certification directory containing certifications in
    # PEM format.
    attr_accessor :ca_path

    # Sets an OpenSSL::X509::Certificate object as client certificate.
    # (This method is appeared in Michal Rokos's OpenSSL extension).
    attr_accessor :cert

    # Sets the X509::Store to verify peer certificate.
    attr_accessor :cert_store

    # Sets the available ciphers.  See OpenSSL::SSL::SSLContext#ciphers=
    attr_accessor :ciphers

    # Sets an OpenSSL::PKey::RSA or OpenSSL::PKey::DSA object.
    # (This method is appeared in Michal Rokos's OpenSSL extension.)
    attr_accessor :key

    # Sets the SSL timeout seconds.
    attr_accessor :ssl_timeout

    # Sets the SSL version.  See OpenSSL::SSL::SSLContext#ssl_version=
    attr_accessor :ssl_version

    # Sets the verify callback for the server certification verification.
    attr_accessor :verify_callback

    # Sets the maximum depth for the certificate chain verification.
    attr_accessor :verify_depth

    # Sets the flags for server the certification verification at beginning of
    # SSL/TLS session.
    #
    # OpenSSL::SSL::VERIFY_NONE or OpenSSL::SSL::VERIFY_PEER are acceptable.
    attr_accessor :verify_mode

    # Returns the X.509 certificates the server presented.
    def peer_cert
      if not use_ssl? or not @socket
        return nil
      end
      @socket.io.peer_cert
    end

    # Opens a TCP connection and HTTP session.
    #
    # When this method is called with a block, it passes the Net::HTTP
    # object to the block, and closes the TCP connection and HTTP session
    # after the block has been executed.
    #
    # When called with a block, it returns the return value of the
    # block; otherwise, it returns self.
    #
    def start  # :yield: http
      raise IOError, 'HTTP session already opened' if @started
      if block_given?
        begin
          do_start
          return yield(self)
        ensure
          do_finish
        end
      end
      do_start
      self
    end

    def do_start
      connect
      @started = true
    end
    private :do_start

    def connect
      D "opening connection to #{conn_address()}..."
      s = Timeout.timeout(@open_timeout, Net::OpenTimeout) {
        TCPSocket.open(conn_address(), conn_port())
      }
      D "opened"
      if use_ssl?
        ssl_parameters = Hash.new
        iv_list = instance_variables
        SSL_IVNAMES.each_with_index do |ivname, i|
          if iv_list.include?(ivname) and
            value = instance_variable_get(ivname)
            ssl_parameters[SSL_ATTRIBUTES[i]] = value if value
          end
        end
        @ssl_context = OpenSSL::SSL::SSLContext.new
        @ssl_context.set_params(ssl_parameters)
        s = OpenSSL::SSL::SSLSocket.new(s, @ssl_context)
        s.sync_close = true
      end
      @socket = BufferedIO.new(s)
      @socket.read_timeout = @read_timeout
      @socket.continue_timeout = @continue_timeout
      @socket.debug_output = @debug_output
      if use_ssl?
        begin
          if proxy?
            buf = "CONNECT #{@address}:#{@port} HTTP/#{HTTPVersion}\r\n"
            buf << "Host: #{@address}:#{@port}\r\n"
            if proxy_user
              credential = ["#{proxy_user}:#{proxy_pass}"].pack('m')
              credential.delete!("\r\n")
              buf << "Proxy-Authorization: Basic #{credential}\r\n"
            end
            buf << "\r\n"
            @socket.write(buf)
            HTTPResponse.read_new(@socket).value
          end
          # Server Name Indication (SNI) RFC 3546
          s.hostname = @address if s.respond_to? :hostname=
          Timeout.timeout(@open_timeout, Net::OpenTimeout) { s.connect }
          if @ssl_context.verify_mode != OpenSSL::SSL::VERIFY_NONE
            s.post_connection_check(@address)
          end
        rescue => exception
          D "Conn close because of connect error #{exception}"
          @socket.close if @socket and not @socket.closed?
          raise exception
        end
      end
      on_connect
    end
    private :connect

    def on_connect
    end
    private :on_connect

    # Finishes the HTTP session and closes the TCP connection.
    # Raises IOError if the session has not been started.
    def finish
      raise IOError, 'HTTP session not yet started' unless started?
      do_finish
    end

    def do_finish
      @started = false
      @socket.close if @socket and not @socket.closed?
      @socket = nil
    end
    private :do_finish

    #
    # proxy
    #

    public

    # no proxy
    @is_proxy_class = false
    @proxy_addr = nil
    @proxy_port = nil
    @proxy_user = nil
    @proxy_pass = nil

    # Creates an HTTP proxy class which behaves like Net::HTTP, but
    # performs all access via the specified proxy.
    #
    # The arguments are the DNS name or IP address of the proxy host,
    # the port to use to access the proxy, and a username and password
    # if authorization is required to use the proxy.
    #
    # You can replace any use of the Net::HTTP class with use of the
    # proxy class created.
    #
    # If +p_addr+ is nil, this method returns self (a Net::HTTP object).
    #
    #   # Example
    #   proxy_class = Net::HTTP::Proxy('proxy.example.com', 8080)
    #
    #   proxy_class.start('www.ruby-lang.org') {|http|
    #     # connecting proxy.foo.org:8080
    #   }
    #
    # You may use them to work with authorization-enabled proxies:
    #
    #   proxy_host = 'your.proxy.example'
    #   proxy_port = 8080
    #   proxy_user = 'user'
    #   proxy_pass = 'pass'
    #
    #   proxy = Net::HTTP::Proxy(proxy_host, proxy_port, proxy_user, proxy_pass)
    #   proxy.start('www.example.com') { |http|
    #     # always connect to your.proxy.example:8080 using specified username
    #     # and password
    #   }
    #
    # Note that net/http does not use the HTTP_PROXY environment variable.
    # If you want to use a proxy, you must set it explicitly.
    #
    def HTTP.Proxy(p_addr, p_port = nil, p_user = nil, p_pass = nil)
      return self unless p_addr
      delta = ProxyDelta
      proxyclass = Class.new(self)
      proxyclass.module_eval {
        include delta
        # with proxy
        @is_proxy_class = true
        @proxy_address = p_addr
        @proxy_port    = p_port || default_port()
        @proxy_user    = p_user
        @proxy_pass    = p_pass
      }
      proxyclass
    end

    class << HTTP
      # returns true if self is a class which was created by HTTP::Proxy.
      def proxy_class?
        @is_proxy_class
      end

      # Address of proxy host. If Net::HTTP does not use a proxy, nil.
      attr_reader :proxy_address

      # Port number of proxy host. If Net::HTTP does not use a proxy, nil.
      attr_reader :proxy_port

      # User name for accessing proxy. If Net::HTTP does not use a proxy, nil.
      attr_reader :proxy_user

      # User password for accessing proxy. If Net::HTTP does not use a proxy,
      # nil.
      attr_reader :proxy_pass
    end

    # True if self is a HTTP proxy class.
    def proxy?
      self.class.proxy_class?
    end

    # A convenience method for accessing value of proxy_address from Net::HTTP.
    def proxy_address
      self.class.proxy_address
    end

    # A convenience method for accessing value of proxy_port from Net::HTTP.
    def proxy_port
      self.class.proxy_port
    end

    # A convenience method for accessing value of proxy_user from Net::HTTP.
    def proxy_user
      self.class.proxy_user
    end

    # A convenience method for accessing value of proxy_pass from Net::HTTP.
    def proxy_pass
      self.class.proxy_pass
    end

    alias proxyaddr proxy_address   #:nodoc: obsolete
    alias proxyport proxy_port      #:nodoc: obsolete

    private

    # without proxy

    def conn_address
      address()
    end

    def conn_port
      port()
    end

    def edit_path(path)
      path
    end

    module ProxyDelta   #:nodoc: internal use only
      private

      def conn_address
        proxy_address()
      end

      def conn_port
        proxy_port()
      end

      def edit_path(path)
        use_ssl? ? path : "http://#{addr_port()}#{path}"
      end
    end

    #
    # HTTP operations
    #

    public

    # Gets data from +path+ on the connected-to host.
    # +initheader+ must be a Hash like { 'Accept' => '*/*', ... },
    # and it defaults to an empty hash.
    # If +initheader+ doesn't have the key 'accept-encoding', then
    # a value of "gzip;q=1.0,deflate;q=0.6,identity;q=0.3" is used,
    # so that gzip compression is used in preference to deflate
    # compression, which is used in preference to no compression.
    # Ruby doesn't have libraries to support the compress (Lempel-Ziv)
    # compression, so that is not supported.  The intent of this is
    # to reduce bandwidth by default.   If this routine sets up
    # compression, then it does the decompression also, removing
    # the header as well to prevent confusion.  Otherwise
    # it leaves the body as it found it.
    #
    # This method returns a Net::HTTPResponse object.
    #
    # If called with a block, yields each fragment of the
    # entity body in turn as a string as it is read from
    # the socket.  Note that in this case, the returned response
    # object will *not* contain a (meaningful) body.
    #
    # +dest+ argument is obsolete.
    # It still works but you must not use it.
    #
    # This method never raises an exception.
    #
    #     response = http.get('/index.html')
    #
    #     # using block
    #     File.open('result.txt', 'w') {|f|
    #       http.get('/~foo/') do |str|
    #         f.write str
    #       end
    #     }
    #
    def get(path, initheader = {}, dest = nil, &block) # :yield: +body_segment+
      res = nil
      if HAVE_ZLIB
        unless  initheader.keys.any?{|k| k.downcase == "accept-encoding"}
          initheader = initheader.merge({
            "accept-encoding" => "gzip;q=1.0,deflate;q=0.6,identity;q=0.3"
          })
          @compression = true
        end
      end
      request(Get.new(path, initheader)) {|r|
        if r.key?("content-encoding") and @compression
          @compression = nil # Clear it till next set.
          the_body = r.read_body dest, &block
          case r["content-encoding"]
          when "gzip"
            r.body= Zlib::GzipReader.new(StringIO.new(the_body), encoding: "ASCII-8BIT").read
            r.delete("content-encoding")
          when "deflate"
            r.body= Zlib::Inflate.inflate(the_body);
            r.delete("content-encoding")
          when "identity"
            ; # nothing needed
          else
            ; # Don't do anything dramatic, unless we need to later
          end
        else
          r.read_body dest, &block
        end
        res = r
      }
      res
    end

    # Gets only the header from +path+ on the connected-to host.
    # +header+ is a Hash like { 'Accept' => '*/*', ... }.
    #
    # This method returns a Net::HTTPResponse object.
    #
    # This method never raises an exception.
    #
    #     response = nil
    #     Net::HTTP.start('some.www.server', 80) {|http|
    #       response = http.head('/index.html')
    #     }
    #     p response['content-type']
    #
    def head(path, initheader = nil)
      request(Head.new(path, initheader))
    end

    # Posts +data+ (must be a String) to +path+. +header+ must be a Hash
    # like { 'Accept' => '*/*', ... }.
    #
    # This method returns a Net::HTTPResponse object.
    #
    # If called with a block, yields each fragment of the
    # entity body in turn as a string as it is read from
    # the socket.  Note that in this case, the returned response
    # object will *not* contain a (meaningful) body.
    #
    # +dest+ argument is obsolete.
    # It still works but you must not use it.
    #
    # This method never raises exception.
    #
    #     response = http.post('/cgi-bin/search.rb', 'query=foo')
    #
    #     # using block
    #     File.open('result.txt', 'w') {|f|
    #       http.post('/cgi-bin/search.rb', 'query=foo') do |str|
    #         f.write str
    #       end
    #     }
    #
    # You should set Content-Type: header field for POST.
    # If no Content-Type: field given, this method uses
    # "application/x-www-form-urlencoded" by default.
    #
    def post(path, data, initheader = nil, dest = nil, &block) # :yield: +body_segment+
      send_entity(path, data, initheader, dest, Post, &block)
    end

    # Sends a PATCH request to the +path+ and gets a response,
    # as an HTTPResponse object.
    def patch(path, data, initheader = nil, dest = nil, &block) # :yield: +body_segment+
      send_entity(path, data, initheader, dest, Patch, &block)
    end

    def put(path, data, initheader = nil)   #:nodoc:
      request(Put.new(path, initheader), data)
    end

    # Sends a PROPPATCH request to the +path+ and gets a response,
    # as an HTTPResponse object.
    def proppatch(path, body, initheader = nil)
      request(Proppatch.new(path, initheader), body)
    end

    # Sends a LOCK request to the +path+ and gets a response,
    # as an HTTPResponse object.
    def lock(path, body, initheader = nil)
      request(Lock.new(path, initheader), body)
    end

    # Sends a UNLOCK request to the +path+ and gets a response,
    # as an HTTPResponse object.
    def unlock(path, body, initheader = nil)
      request(Unlock.new(path, initheader), body)
    end

    # Sends a OPTIONS request to the +path+ and gets a response,
    # as an HTTPResponse object.
    def options(path, initheader = nil)
      request(Options.new(path, initheader))
    end

    # Sends a PROPFIND request to the +path+ and gets a response,
    # as an HTTPResponse object.
    def propfind(path, body = nil, initheader = {'Depth' => '0'})
      request(Propfind.new(path, initheader), body)
    end

    # Sends a DELETE request to the +path+ and gets a response,
    # as an HTTPResponse object.
    def delete(path, initheader = {'Depth' => 'Infinity'})
      request(Delete.new(path, initheader))
    end

    # Sends a MOVE request to the +path+ and gets a response,
    # as an HTTPResponse object.
    def move(path, initheader = nil)
      request(Move.new(path, initheader))
    end

    # Sends a COPY request to the +path+ and gets a response,
    # as an HTTPResponse object.
    def copy(path, initheader = nil)
      request(Copy.new(path, initheader))
    end

    # Sends a MKCOL request to the +path+ and gets a response,
    # as an HTTPResponse object.
    def mkcol(path, body = nil, initheader = nil)
      request(Mkcol.new(path, initheader), body)
    end

    # Sends a TRACE request to the +path+ and gets a response,
    # as an HTTPResponse object.
    def trace(path, initheader = nil)
      request(Trace.new(path, initheader))
    end

    # Sends a GET request to the +path+.
    # Returns the response as a Net::HTTPResponse object.
    #
    # When called with a block, passes an HTTPResponse object to the block.
    # The body of the response will not have been read yet;
    # the block can process it using HTTPResponse#read_body,
    # if desired.
    #
    # Returns the response.
    #
    # This method never raises Net::* exceptions.
    #
    #     response = http.request_get('/index.html')
    #     # The entity body is already read in this case.
    #     p response['content-type']
    #     puts response.body
    #
    #     # Using a block
    #     http.request_get('/index.html') {|response|
    #       p response['content-type']
    #       response.read_body do |str|   # read body now
    #         print str
    #       end
    #     }
    #
    def request_get(path, initheader = nil, &block) # :yield: +response+
      request(Get.new(path, initheader), &block)
    end

    # Sends a HEAD request to the +path+ and returns the response
    # as a Net::HTTPResponse object.
    #
    # Returns the response.
    #
    # This method never raises Net::* exceptions.
    #
    #     response = http.request_head('/index.html')
    #     p response['content-type']
    #
    def request_head(path, initheader = nil, &block)
      request(Head.new(path, initheader), &block)
    end

    # Sends a POST request to the +path+.
    #
    # Returns the response as a Net::HTTPResponse object.
    #
    # When called with a block, the block is passed an HTTPResponse
    # object.  The body of that response will not have been read yet;
    # the block can process it using HTTPResponse#read_body, if desired.
    #
    # Returns the response.
    #
    # This method never raises Net::* exceptions.
    #
    #     # example
    #     response = http.request_post('/cgi-bin/nice.rb', 'datadatadata...')
    #     p response.status
    #     puts response.body          # body is already read in this case
    #
    #     # using block
    #     http.request_post('/cgi-bin/nice.rb', 'datadatadata...') {|response|
    #       p response.status
    #       p response['content-type']
    #       response.read_body do |str|   # read body now
    #         print str
    #       end
    #     }
    #
    def request_post(path, data, initheader = nil, &block) # :yield: +response+
      request Post.new(path, initheader), data, &block
    end

    def request_put(path, data, initheader = nil, &block)   #:nodoc:
      request Put.new(path, initheader), data, &block
    end

    alias get2   request_get    #:nodoc: obsolete
    alias head2  request_head   #:nodoc: obsolete
    alias post2  request_post   #:nodoc: obsolete
    alias put2   request_put    #:nodoc: obsolete


    # Sends an HTTP request to the HTTP server.
    # Also sends a DATA string if +data+ is given.
    #
    # Returns a Net::HTTPResponse object.
    #
    # This method never raises Net::* exceptions.
    #
    #    response = http.send_request('GET', '/index.html')
    #    puts response.body
    #
    def send_request(name, path, data = nil, header = nil)
      r = HTTPGenericRequest.new(name,(data ? true : false),true,path,header)
      request r, data
    end

    # Sends an HTTPRequest object +req+ to the HTTP server.
    #
    # If +req+ is a Net::HTTP::Post or Net::HTTP::Put request containing
    # data, the data is also sent. Providing data for a Net::HTTP::Head or
    # Net::HTTP::Get request results in an ArgumentError.
    #
    # Returns an HTTPResponse object.
    #
    # When called with a block, passes an HTTPResponse object to the block.
    # The body of the response will not have been read yet;
    # the block can process it using HTTPResponse#read_body,
    # if desired.
    #
    # This method never raises Net::* exceptions.
    #
    def request(req, body = nil, &block)  # :yield: +response+
      unless started?
        start {
          req['connection'] ||= 'close'
          return request(req, body, &block)
        }
      end
      if proxy_user()
        req.proxy_basic_auth proxy_user(), proxy_pass() unless use_ssl?
      end
      req.set_body_internal body
      res = transport_request(req, &block)
      if sspi_auth?(res)
        sspi_auth(req)
        res = transport_request(req, &block)
      end
      res
    end

    private

    # Executes a request which uses a representation
    # and returns its body.
    def send_entity(path, data, initheader, dest, type, &block)
      res = nil
      request(type.new(path, initheader), data) {|r|
        r.read_body dest, &block
        res = r
      }
      res
    end

    IDEMPOTENT_METHODS_ = %w/GET HEAD PUT DELETE OPTIONS TRACE/ # :nodoc:

    def transport_request(req)
      count = 0
      begin
        begin_transport req
        res = catch(:response) {
          req.exec @socket, @curr_http_version, edit_path(req.path)
          begin
            res = HTTPResponse.read_new(@socket)
          end while res.kind_of?(HTTPContinue)
          res.reading_body(@socket, req.response_body_permitted?) {
            yield res if block_given?
          }
          res
        }
      rescue IOError, EOFError,
             Errno::ECONNRESET, Errno::ECONNABORTED, Errno::EPIPE,
             OpenSSL::SSL::SSLError, Timeout::Error => exception
        raise if Net::OpenTimeout === exception

        if count == 0 && IDEMPOTENT_METHODS_.include?(req.method)
          count += 1
          @socket.close if @socket and not @socket.closed?
          D "Conn close because of error #{exception}, and retry"
          retry
        end
        D "Conn close because of error #{exception}"
        @socket.close if @socket and not @socket.closed?
        raise
      end

      end_transport req, res
      res
    rescue => exception
      D "Conn close because of error #{exception}"
      @socket.close if @socket and not @socket.closed?
      raise exception
    end

    def begin_transport(req)
      if @socket.closed?
        connect
      elsif @last_communicated && @last_communicated + @keep_alive_timeout < Time.now
        D 'Conn close because of keep_alive_timeout'
        @socket.close
        connect
      end

      if not req.response_body_permitted? and @close_on_empty_response
        req['connection'] ||= 'close'
      end
      req['host'] ||= addr_port()
    end

    def end_transport(req, res)
      @curr_http_version = res.http_version
      @last_communicated = nil
      if @socket.closed?
        D 'Conn socket closed'
      elsif not res.body and @close_on_empty_response
        D 'Conn close'
        @socket.close
      elsif keep_alive?(req, res)
        D 'Conn keep-alive'
        @last_communicated = Time.now
      else
        D 'Conn close'
        @socket.close
      end
    end

    def keep_alive?(req, res)
      return false if req.connection_close?
      if @curr_http_version <= '1.0'
        res.connection_keep_alive?
      else   # HTTP/1.1 or later
        not res.connection_close?
      end
    end

    def sspi_auth?(res)
      return false unless @sspi_enabled
      if res.kind_of?(HTTPProxyAuthenticationRequired) and
          proxy? and res["Proxy-Authenticate"].include?("Negotiate")
        begin
          require 'win32/sspi'
          true
        rescue LoadError
          false
        end
      else
        false
      end
    end

    def sspi_auth(req)
      n = Win32::SSPI::NegotiateAuth.new
      req["Proxy-Authorization"] = "Negotiate #{n.get_initial_token}"
      # Some versions of ISA will close the connection if this isn't present.
      req["Connection"] = "Keep-Alive"
      req["Proxy-Connection"] = "Keep-Alive"
      res = transport_request(req)
      authphrase = res["Proxy-Authenticate"]  or return res
      req["Proxy-Authorization"] = "Negotiate #{n.complete_authentication(authphrase)}"
    rescue => err
      raise HTTPAuthenticationError.new('HTTP authentication failed', err)
    end

    #
    # utils
    #

    private

    def addr_port
      if use_ssl?
        address() + (port == HTTP.https_default_port ? '' : ":#{port()}")
      else
        address() + (port == HTTP.http_default_port ? '' : ":#{port()}")
      end
    end

    def D(msg)
      return unless @debug_output
      @debug_output << msg
      @debug_output << "\n"
    end

  end

  HTTPSession = HTTP


  # The HTTPHeader module defines methods for reading and writing
  # HTTP headers.
  #
  # It is used as a mixin by other classes, to provide hash-like
  # access to HTTP header values. Unlike raw hash access, HTTPHeader
  # provides access via case-insensitive keys. It also provides
  # methods for accessing commonly-used HTTP header values in more
  # convenient formats.
  #
  module HTTPHeader

    def initialize_http_header(initheader)
      @header = {}
      return unless initheader
      initheader.each do |key, value|
        warn "net/http: warning: duplicated HTTP header: #{key}" if key?(key) and $VERBOSE
        @header[key.downcase] = [value.strip]
      end
    end

    def size   #:nodoc: obsolete
      @header.size
    end

    alias length size   #:nodoc: obsolete

    # Returns the header field corresponding to the case-insensitive key.
    # For example, a key of "Content-Type" might return "text/html"
    def [](key)
      a = @header[key.downcase] or return nil
      a.join(', ')
    end

    # Sets the header field corresponding to the case-insensitive key.
    def []=(key, val)
      unless val
        @header.delete key.downcase
        return val
      end
      @header[key.downcase] = [val]
    end

    # [Ruby 1.8.3]
    # Adds a value to a named header field, instead of replacing its value.
    # Second argument +val+ must be a String.
    # See also #[]=, #[] and #get_fields.
    #
    #   request.add_field 'X-My-Header', 'a'
    #   p request['X-My-Header']              #=> "a"
    #   p request.get_fields('X-My-Header')   #=> ["a"]
    #   request.add_field 'X-My-Header', 'b'
    #   p request['X-My-Header']              #=> "a, b"
    #   p request.get_fields('X-My-Header')   #=> ["a", "b"]
    #   request.add_field 'X-My-Header', 'c'
    #   p request['X-My-Header']              #=> "a, b, c"
    #   p request.get_fields('X-My-Header')   #=> ["a", "b", "c"]
    #
    def add_field(key, val)
      if @header.key?(key.downcase)
        @header[key.downcase].push val
      else
        @header[key.downcase] = [val]
      end
    end

    # [Ruby 1.8.3]
    # Returns an array of header field strings corresponding to the
    # case-insensitive +key+.  This method allows you to get duplicated
    # header fields without any processing.  See also #[].
    #
    #   p response.get_fields('Set-Cookie')
    #     #=> ["session=al98axx; expires=Fri, 31-Dec-1999 23:58:23",
    #          "query=rubyscript; expires=Fri, 31-Dec-1999 23:58:23"]
    #   p response['Set-Cookie']
    #     #=> "session=al98axx; expires=Fri, 31-Dec-1999 23:58:23, query=rubyscript; expires=Fri, 31-Dec-1999 23:58:23"
    #
    def get_fields(key)
      return nil unless @header[key.downcase]
      @header[key.downcase].dup
    end

    # Returns the header field corresponding to the case-insensitive key.
    # Returns the default value +args+, or the result of the block, or
    # raises an IndexError if there's no header field named +key+
    # See Hash#fetch
    def fetch(key, *args, &block)   #:yield: +key+
      a = @header.fetch(key.downcase, *args, &block)
      a.kind_of?(Array) ? a.join(', ') : a
    end

    # Iterates through the header names and values, passing in the name
    # and value to the code block supplied.
    #
    # Example:
    #
    #     response.header.each_header {|key,value| puts "#{key} = #{value}" }
    #
    def each_header   #:yield: +key+, +value+
      block_given? or return enum_for(__method__)
      @header.each do |k,va|
        yield k, va.join(', ')
      end
    end

    alias each each_header

    # Iterates through the header names in the header, passing
    # each header name to the code block.
    def each_name(&block)   #:yield: +key+
      block_given? or return enum_for(__method__)
      @header.each_key(&block)
    end

    alias each_key each_name

    # Iterates through the header names in the header, passing
    # capitalized header names to the code block.
    #
    # Note that header names are capitalized systematically;
    # capitalization may not match that used by the remote HTTP
    # server in its response.
    def each_capitalized_name  #:yield: +key+
      block_given? or return enum_for(__method__)
      @header.each_key do |k|
        yield capitalize(k)
      end
    end

    # Iterates through header values, passing each value to the
    # code block.
    def each_value   #:yield: +value+
      block_given? or return enum_for(__method__)
      @header.each_value do |va|
        yield va.join(', ')
      end
    end

    # Removes a header field, specified by case-insensitive key.
    def delete(key)
      @header.delete(key.downcase)
    end

    # true if +key+ header exists.
    def key?(key)
      @header.key?(key.downcase)
    end

    # Returns a Hash consisting of header names and values.
    # e.g.
    # {"cache-control" => "private",
    #  "content-type" => "text/html",
    #  "date" => "Wed, 22 Jun 2005 22:11:50 GMT"}
    def to_hash
      @header.dup
    end

    # As for #each_header, except the keys are provided in capitalized form.
    #
    # Note that header names are capitalized systematically;
    # capitalization may not match that used by the remote HTTP
    # server in its response.
    def each_capitalized
      block_given? or return enum_for(__method__)
      @header.each do |k,v|
        yield capitalize(k), v.join(', ')
      end
    end

    alias canonical_each each_capitalized

    def capitalize(name)
      name.split(/-/).map {|s| s.capitalize }.join('-')
    end
    private :capitalize

    # Returns an Array of Range objects which represent the Range:
    # HTTP header field, or +nil+ if there is no such header.
    def range
      return nil unless @header['range']
      self['Range'].split(/,/).map {|spec|
        m = /bytes\s*=\s*(\d+)?\s*-\s*(\d+)?/i.match(spec) or
                raise HTTPHeaderSyntaxError, "wrong Range: #{spec}"
        d1 = m[1].to_i
        d2 = m[2].to_i
        if    m[1] and m[2] then  d1..d2
        elsif m[1]          then  d1..-1
        elsif          m[2] then -d2..-1
        else
          raise HTTPHeaderSyntaxError, 'range is not specified'
        end
      }
    end

    # Sets the HTTP Range: header.
    # Accepts either a Range object as a single argument,
    # or a beginning index and a length from that index.
    # Example:
    #
    #   req.range = (0..1023)
    #   req.set_range 0, 1023
    #
    def set_range(r, e = nil)
      unless r
        @header.delete 'range'
        return r
      end
      r = (r...r+e) if e
      case r
      when Numeric
        n = r.to_i
        rangestr = (n > 0 ? "0-#{n-1}" : "-#{-n}")
      when Range
        first = r.first
        last = r.last
        last -= 1 if r.exclude_end?
        if last == -1
          rangestr = (first > 0 ? "#{first}-" : "-#{-first}")
        else
          raise HTTPHeaderSyntaxError, 'range.first is negative' if first < 0
          raise HTTPHeaderSyntaxError, 'range.last is negative' if last < 0
          raise HTTPHeaderSyntaxError, 'must be .first < .last' if first > last
          rangestr = "#{first}-#{last}"
        end
      else
        raise TypeError, 'Range/Integer is required'
      end
      @header['range'] = ["bytes=#{rangestr}"]
      r
    end

    alias range= set_range

    # Returns an Integer object which represents the HTTP Content-Length:
    # header field, or +nil+ if that field was not provided.
    def content_length
      return nil unless key?('Content-Length')
      len = self['Content-Length'].slice(/\d+/) or
          raise HTTPHeaderSyntaxError, 'wrong Content-Length format'
      len.to_i
    end

    def content_length=(len)
      unless len
        @header.delete 'content-length'
        return nil
      end
      @header['content-length'] = [len.to_i.to_s]
    end

    # Returns "true" if the "transfer-encoding" header is present and
    # set to "chunked".  This is an HTTP/1.1 feature, allowing the
    # the content to be sent in "chunks" without at the outset
    # stating the entire content length.
    def chunked?
      return false unless @header['transfer-encoding']
      field = self['Transfer-Encoding']
      (/(?:\A|[^\-\w])chunked(?![\-\w])/i =~ field) ? true : false
    end

    # Returns a Range object which represents the value of the Content-Range:
    # header field.
    # For a partial entity body, this indicates where this fragment
    # fits inside the full entity body, as range of byte offsets.
    def content_range
      return nil unless @header['content-range']
      m = %r<bytes\s+(\d+)-(\d+)/(\d+|\*)>i.match(self['Content-Range']) or
          raise HTTPHeaderSyntaxError, 'wrong Content-Range format'
      m[1].to_i .. m[2].to_i
    end

    # The length of the range represented in Content-Range: header.
    def range_length
      r = content_range() or return nil
      r.end - r.begin + 1
    end

    # Returns a content type string such as "text/html".
    # This method returns nil if Content-Type: header field does not exist.
    def content_type
      return nil unless main_type()
      if sub_type()
      then "#{main_type()}/#{sub_type()}"
      else main_type()
      end
    end

    # Returns a content type string such as "text".
    # This method returns nil if Content-Type: header field does not exist.
    def main_type
      return nil unless @header['content-type']
      self['Content-Type'].split(';').first.to_s.split('/')[0].to_s.strip
    end

    # Returns a content type string such as "html".
    # This method returns nil if Content-Type: header field does not exist
    # or sub-type is not given (e.g. "Content-Type: text").
    def sub_type
      return nil unless @header['content-type']
      _, sub = *self['Content-Type'].split(';').first.to_s.split('/')
      return nil unless sub
      sub.strip
    end

    # Any parameters specified for the content type, returned as a Hash.
    # For example, a header of Content-Type: text/html; charset=EUC-JP
    # would result in type_params returning {'charset' => 'EUC-JP'}
    def type_params
      result = {}
      list = self['Content-Type'].to_s.split(';')
      list.shift
      list.each do |param|
        k, v = *param.split('=', 2)
        result[k.strip] = v.strip
      end
      result
    end

    # Sets the content type in an HTTP header.
    # The +type+ should be a full HTTP content type, e.g. "text/html".
    # The +params+ are an optional Hash of parameters to add after the
    # content type, e.g. {'charset' => 'iso-8859-1'}
    def set_content_type(type, params = {})
      @header['content-type'] = [type + params.map{|k,v|"; #{k}=#{v}"}.join('')]
    end

    alias content_type= set_content_type

    # Set header fields and a body from HTML form data.
    # +params+ should be an Array of Arrays or
    # a Hash containing HTML form data.
    # Optional argument +sep+ means data record separator.
    #
    # Values are URL encoded as necessary and the content-type is set to
    # application/x-www-form-urlencoded
    #
    # Example:
    #    http.form_data = {"q" => "ruby", "lang" => "en"}
    #    http.form_data = {"q" => ["ruby", "perl"], "lang" => "en"}
    #    http.set_form_data({"q" => "ruby", "lang" => "en"}, ';')
    #
    def set_form_data(params, sep = '&')
      query = URI.encode_www_form(params)
      query.gsub!(/&/, sep) if sep != '&'
      self.body = query
      self.content_type = 'application/x-www-form-urlencoded'
    end

    alias form_data= set_form_data

    # Set a HTML form data set.
    # +params+ is the form data set; it is an Array of Arrays or a Hash
    # +enctype is the type to encode the form data set.
    # It is application/x-www-form-urlencoded or multipart/form-data.
    # +formpot+ is an optional hash to specify the detail.
    #
    # boundary:: the boundary of the multipart message
    # charset::  the charset of the message. All names and the values of
    #            non-file fields are encoded as the charset.
    #
    # Each item of params is an array and contains following items:
    # +name+::  the name of the field
    # +value+:: the value of the field, it should be a String or a File
    # +opt+::   an optional hash to specify additional information
    #
    # Each item is a file field or a normal field.
    # If +value+ is a File object or the +opt+ have a filename key,
    # the item is treated as a file field.
    #
    # If Transfer-Encoding is set as chunked, this send the request in
    # chunked encoding. Because chunked encoding is HTTP/1.1 feature,
    # you must confirm the server to support HTTP/1.1 before sending it.
    #
    # Example:
    #    http.set_form([["q", "ruby"], ["lang", "en"]])
    #
    # See also RFC 2388, RFC 2616, HTML 4.01, and HTML5
    #
    def set_form(params, enctype='application/x-www-form-urlencoded', formopt={})
      @body_data = params
      @body = nil
      @body_stream = nil
      @form_option = formopt
      case enctype
      when /\Aapplication\/x-www-form-urlencoded\z/i,
        /\Amultipart\/form-data\z/i
        self.content_type = enctype
      else
        raise ArgumentError, "invalid enctype: #{enctype}"
      end
    end

    # Set the Authorization: header for "Basic" authorization.
    def basic_auth(account, password)
      @header['authorization'] = [basic_encode(account, password)]
    end

    # Set Proxy-Authorization: header for "Basic" authorization.
    def proxy_basic_auth(account, password)
      @header['proxy-authorization'] = [basic_encode(account, password)]
    end

    def basic_encode(account, password)
      'Basic ' + ["#{account}:#{password}"].pack('m').delete("\r\n")
    end
    private :basic_encode

    def connection_close?
      tokens(@header['connection']).include?('close') or
      tokens(@header['proxy-connection']).include?('close')
    end

    def connection_keep_alive?
      tokens(@header['connection']).include?('keep-alive') or
      tokens(@header['proxy-connection']).include?('keep-alive')
    end

    def tokens(vals)
      return [] unless vals
      vals.map {|v| v.split(',') }.flatten\
          .reject {|str| str.strip.empty? }\
          .map {|tok| tok.strip.downcase }
    end
    private :tokens

  end


  # HTTPGenericRequest is the parent of the HTTPRequest class.
  # Do not use this directly; use a subclass of HTTPRequest.
  #
  # Mixes in the HTTPHeader module to provide easier access to HTTP headers.
  #
  class HTTPGenericRequest

    include HTTPHeader

    def initialize(m, reqbody, resbody, path, initheader = nil)
      @method = m
      @request_has_body = reqbody
      @response_has_body = resbody
      raise ArgumentError, "no HTTP request path given" unless path
      raise ArgumentError, "HTTP request path is empty" if path.empty?
      @path = path
      initialize_http_header initheader
      self['Accept'] ||= '*/*'
      self['User-Agent'] ||= 'Ruby'
      @body = nil
      @body_stream = nil
      @body_data = nil
    end

    attr_reader :method
    attr_reader :path

    def inspect
      "\#<#{self.class} #{@method}>"
    end

    def request_body_permitted?
      @request_has_body
    end

    def response_body_permitted?
      @response_has_body
    end

    def body_exist?
      warn "Net::HTTPRequest#body_exist? is obsolete; use response_body_permitted?" if $VERBOSE
      response_body_permitted?
    end

    attr_reader :body

    def body=(str)
      @body = str
      @body_stream = nil
      @body_data = nil
      str
    end

    attr_reader :body_stream

    def body_stream=(input)
      @body = nil
      @body_stream = input
      @body_data = nil
      input
    end

    def set_body_internal(str)   #:nodoc: internal use only
      raise ArgumentError, "both of body argument and HTTPRequest#body set" if str and (@body or @body_stream)
      self.body = str if str
    end

    #
    # write
    #

    def exec(sock, ver, path)   #:nodoc: internal use only
      if @body
        send_request_with_body sock, ver, path, @body
      elsif @body_stream
        send_request_with_body_stream sock, ver, path, @body_stream
      elsif @body_data
        send_request_with_body_data sock, ver, path, @body_data
      else
        write_header sock, ver, path
      end
    end

    private

    def send_request_with_body(sock, ver, path, body)
      self.content_length = body.bytesize
      delete 'Transfer-Encoding'
      supply_default_content_type
      write_header sock, ver, path
      wait_for_continue sock, ver if sock.continue_timeout
      sock.write body
    end

    def send_request_with_body_stream(sock, ver, path, f)
      unless content_length() or chunked?
        raise ArgumentError,
            "Content-Length not given and Transfer-Encoding is not `chunked'"
      end
      supply_default_content_type
      write_header sock, ver, path
      wait_for_continue sock, ver if sock.continue_timeout
      if chunked?
        while s = f.read(1024)
          sock.write(sprintf("%x\r\n", s.length) << s << "\r\n")
        end
        sock.write "0\r\n\r\n"
      else
        while s = f.read(1024)
          sock.write s
        end
      end
    end

    def send_request_with_body_data(sock, ver, path, params)
      if /\Amultipart\/form-data\z/i !~ self.content_type
        self.content_type = 'application/x-www-form-urlencoded'
        return send_request_with_body(sock, ver, path, URI.encode_www_form(params))
      end

      opt = @form_option.dup
      require 'securerandom' unless defined?(SecureRandom)
      opt[:boundary] ||= SecureRandom.urlsafe_base64(40)
      self.set_content_type(self.content_type, boundary: opt[:boundary])
      if chunked?
        write_header sock, ver, path
        encode_multipart_form_data(sock, params, opt)
      else
        require 'tempfile'
        file = Tempfile.new('multipart')
        file.binmode
        encode_multipart_form_data(file, params, opt)
        file.rewind
        self.content_length = file.size
        write_header sock, ver, path
        IO.copy_stream(file, sock)
      end
    end

    def encode_multipart_form_data(out, params, opt)
      charset = opt[:charset]
      boundary = opt[:boundary]
      require 'securerandom' unless defined?(SecureRandom)
      boundary ||= SecureRandom.urlsafe_base64(40)
      chunked_p = chunked?

      buf = ''
      params.each do |key, value, h={}|
        key = quote_string(key, charset)
        filename =
          h.key?(:filename) ? h[:filename] :
          value.respond_to?(:to_path) ? File.basename(value.to_path) :
          nil

        buf << "--#{boundary}\r\n"
        if filename
          filename = quote_string(filename, charset)
          type = h[:content_type] || 'application/octet-stream'
          buf << "Content-Disposition: form-data; " \
            "name=\"#{key}\"; filename=\"#{filename}\"\r\n" \
            "Content-Type: #{type}\r\n\r\n"
          if !out.respond_to?(:write) || !value.respond_to?(:read)
            # if +out+ is not an IO or +value+ is not an IO
            buf << (value.respond_to?(:read) ? value.read : value)
          elsif value.respond_to?(:size) && chunked_p
            # if +out+ is an IO and +value+ is a File, use IO.copy_stream
            flush_buffer(out, buf, chunked_p)
            out << "%x\r\n" % value.size if chunked_p
            IO.copy_stream(value, out)
            out << "\r\n" if chunked_p
          else
            # +out+ is an IO, and +value+ is not a File but an IO
            flush_buffer(out, buf, chunked_p)
            1 while flush_buffer(out, value.read(4096), chunked_p)
          end
        else
          # non-file field:
          #   HTML5 says, "The parts of the generated multipart/form-data
          #   resource that correspond to non-file fields must not have a
          #   Content-Type header specified."
          buf << "Content-Disposition: form-data; name=\"#{key}\"\r\n\r\n"
          buf << (value.respond_to?(:read) ? value.read : value)
        end
        buf << "\r\n"
      end
      buf << "--#{boundary}--\r\n"
      flush_buffer(out, buf, chunked_p)
      out << "0\r\n\r\n" if chunked_p
    end

    def quote_string(str, charset)
      str = str.encode(charset, fallback:->(c){'&#%d;'%c.encode("UTF-8").ord}) if charset
      str = str.gsub(/[\\"]/, '\\\\\&')
    end

    def flush_buffer(out, buf, chunked_p)
      return unless buf
      out << "%x\r\n"%buf.bytesize if chunked_p
      out << buf
      out << "\r\n" if chunked_p
      buf.clear
    end

    def supply_default_content_type
      return if content_type()
      warn 'net/http: warning: Content-Type did not set; using application/x-www-form-urlencoded' if $VERBOSE
      set_content_type 'application/x-www-form-urlencoded'
    end

    ##
    # Waits up to the continue timeout for a response from the server provided
    # we're speaking HTTP 1.1 and are expecting a 100-continue response.

    def wait_for_continue(sock, ver)
      if ver >= '1.1' and @header['expect'] and
          @header['expect'].include?('100-continue')
        if IO.select([sock.io], nil, nil, sock.continue_timeout)
          res = HTTPResponse.read_new(sock)
          unless res.kind_of?(Net::HTTPContinue)
            throw :response, res
          end
        end
      end
    end

    def write_header(sock, ver, path)
      buf = "#{@method} #{path} HTTP/#{ver}\r\n"
      each_capitalized do |k,v|
        buf << "#{k}: #{v}\r\n"
      end
      buf << "\r\n"
      sock.write buf
    end

  end


  #
  # HTTP request class.
  # This class wraps together the request header and the request path.
  # You cannot use this class directly. Instead, you should use one of its
  # subclasses: Net::HTTP::Get, Net::HTTP::Post, Net::HTTP::Head.
  #
  class HTTPRequest < HTTPGenericRequest

    # Creates HTTP request object.
    def initialize(path, initheader = nil)
      super self.class::METHOD,
            self.class::REQUEST_HAS_BODY,
            self.class::RESPONSE_HAS_BODY,
            path, initheader
    end
  end


  class HTTP   # reopen
    #
    # HTTP/1.1 methods --- RFC2616
    #

    # See Net::HTTPGenericRequest for attributes and methods.
    # See Net::HTTP for usage examples.
    class Get < HTTPRequest
      METHOD = 'GET'
      REQUEST_HAS_BODY  = false
      RESPONSE_HAS_BODY = true
    end

    # See Net::HTTPGenericRequest for attributes and methods.
    # See Net::HTTP for usage examples.
    class Head < HTTPRequest
      METHOD = 'HEAD'
      REQUEST_HAS_BODY = false
      RESPONSE_HAS_BODY = false
    end

    # See Net::HTTPGenericRequest for attributes and methods.
    # See Net::HTTP for usage examples.
    class Post < HTTPRequest
      METHOD = 'POST'
      REQUEST_HAS_BODY = true
      RESPONSE_HAS_BODY = true
    end

    # See Net::HTTPGenericRequest for attributes and methods.
    # See Net::HTTP for usage examples.
    class Put < HTTPRequest
      METHOD = 'PUT'
      REQUEST_HAS_BODY = true
      RESPONSE_HAS_BODY = true
    end

    # See Net::HTTPGenericRequest for attributes and methods.
    # See Net::HTTP for usage examples.
    class Delete < HTTPRequest
      METHOD = 'DELETE'
      REQUEST_HAS_BODY = false
      RESPONSE_HAS_BODY = true
    end

    # See Net::HTTPGenericRequest for attributes and methods.
    class Options < HTTPRequest
      METHOD = 'OPTIONS'
      REQUEST_HAS_BODY = false
      RESPONSE_HAS_BODY = false
    end

    # See Net::HTTPGenericRequest for attributes and methods.
    class Trace < HTTPRequest
      METHOD = 'TRACE'
      REQUEST_HAS_BODY = false
      RESPONSE_HAS_BODY = true
    end

    #
    # PATCH method --- RFC5789
    #

    # See Net::HTTPGenericRequest for attributes and methods.
    class Patch < HTTPRequest
      METHOD = 'PATCH'
      REQUEST_HAS_BODY = true
      RESPONSE_HAS_BODY = true
    end

    #
    # WebDAV methods --- RFC2518
    #

    # See Net::HTTPGenericRequest for attributes and methods.
    class Propfind < HTTPRequest
      METHOD = 'PROPFIND'
      REQUEST_HAS_BODY = true
      RESPONSE_HAS_BODY = true
    end

    # See Net::HTTPGenericRequest for attributes and methods.
    class Proppatch < HTTPRequest
      METHOD = 'PROPPATCH'
      REQUEST_HAS_BODY = true
      RESPONSE_HAS_BODY = true
    end

    # See Net::HTTPGenericRequest for attributes and methods.
    class Mkcol < HTTPRequest
      METHOD = 'MKCOL'
      REQUEST_HAS_BODY = true
      RESPONSE_HAS_BODY = true
    end

    # See Net::HTTPGenericRequest for attributes and methods.
    class Copy < HTTPRequest
      METHOD = 'COPY'
      REQUEST_HAS_BODY = false
      RESPONSE_HAS_BODY = true
    end

    # See Net::HTTPGenericRequest for attributes and methods.
    class Move < HTTPRequest
      METHOD = 'MOVE'
      REQUEST_HAS_BODY = false
      RESPONSE_HAS_BODY = true
    end

    # See Net::HTTPGenericRequest for attributes and methods.
    class Lock < HTTPRequest
      METHOD = 'LOCK'
      REQUEST_HAS_BODY = true
      RESPONSE_HAS_BODY = true
    end

    # See Net::HTTPGenericRequest for attributes and methods.
    class Unlock < HTTPRequest
      METHOD = 'UNLOCK'
      REQUEST_HAS_BODY = true
      RESPONSE_HAS_BODY = true
    end
  end


  ###
  ### Response
  ###

  # HTTP exception class.
  # You cannot use HTTPExceptions directly; instead, you must use
  # its subclasses.
  module HTTPExceptions
    def initialize(msg, res)   #:nodoc:
      super msg
      @response = res
    end
    attr_reader :response
    alias data response    #:nodoc: obsolete
  end
  class HTTPError < ProtocolError
    include HTTPExceptions
  end
  class HTTPRetriableError < ProtoRetriableError
    include HTTPExceptions
  end
  class HTTPServerException < ProtoServerError
    # We cannot use the name "HTTPServerError", it is the name of the response.
    include HTTPExceptions
  end
  class HTTPFatalError < ProtoFatalError
    include HTTPExceptions
  end


  # HTTP response class.
  #
  # This class wraps together the response header and the response body (the
  # entity requested).
  #
  # It mixes in the HTTPHeader module, which provides access to response
  # header values both via hash-like methods and via individual readers.
  #
  # Note that each possible HTTP response code defines its own
  # HTTPResponse subclass.  These are listed below.
  #
  # All classes are
  # defined under the Net module. Indentation indicates inheritance.
  #
  #   xxx        HTTPResponse
  #
  #     1xx        HTTPInformation
  #       100        HTTPContinue
  #       101        HTTPSwitchProtocol
  #
  #     2xx        HTTPSuccess
  #       200        HTTPOK
  #       201        HTTPCreated
  #       202        HTTPAccepted
  #       203        HTTPNonAuthoritativeInformation
  #       204        HTTPNoContent
  #       205        HTTPResetContent
  #       206        HTTPPartialContent
  #
  #     3xx        HTTPRedirection
  #       300        HTTPMultipleChoice
  #       301        HTTPMovedPermanently
  #       302        HTTPFound
  #       303        HTTPSeeOther
  #       304        HTTPNotModified
  #       305        HTTPUseProxy
  #       307        HTTPTemporaryRedirect
  #
  #     4xx        HTTPClientError
  #       400        HTTPBadRequest
  #       401        HTTPUnauthorized
  #       402        HTTPPaymentRequired
  #       403        HTTPForbidden
  #       404        HTTPNotFound
  #       405        HTTPMethodNotAllowed
  #       406        HTTPNotAcceptable
  #       407        HTTPProxyAuthenticationRequired
  #       408        HTTPRequestTimeOut
  #       409        HTTPConflict
  #       410        HTTPGone
  #       411        HTTPLengthRequired
  #       412        HTTPPreconditionFailed
  #       413        HTTPRequestEntityTooLarge
  #       414        HTTPRequestURITooLong
  #       415        HTTPUnsupportedMediaType
  #       416        HTTPRequestedRangeNotSatisfiable
  #       417        HTTPExpectationFailed
  #
  #     5xx        HTTPServerError
  #       500        HTTPInternalServerError
  #       501        HTTPNotImplemented
  #       502        HTTPBadGateway
  #       503        HTTPServiceUnavailable
  #       504        HTTPGatewayTimeOut
  #       505        HTTPVersionNotSupported
  #
  #     xxx        HTTPUnknownResponse
  #
  class HTTPResponse
    # true if the response has a body.
    def HTTPResponse.body_permitted?
      self::HAS_BODY
    end

    def HTTPResponse.exception_type   # :nodoc: internal use only
      self::EXCEPTION_TYPE
    end
  end   # reopened after

  # :stopdoc:

  class HTTPUnknownResponse < HTTPResponse
    HAS_BODY = true
    EXCEPTION_TYPE = HTTPError
  end
  class HTTPInformation < HTTPResponse           # 1xx
    HAS_BODY = false
    EXCEPTION_TYPE = HTTPError
  end
  class HTTPSuccess < HTTPResponse               # 2xx
    HAS_BODY = true
    EXCEPTION_TYPE = HTTPError
  end
  class HTTPRedirection < HTTPResponse           # 3xx
    HAS_BODY = true
    EXCEPTION_TYPE = HTTPRetriableError
  end
  class HTTPClientError < HTTPResponse           # 4xx
    HAS_BODY = true
    EXCEPTION_TYPE = HTTPServerException   # for backward compatibility
  end
  class HTTPServerError < HTTPResponse           # 5xx
    HAS_BODY = true
    EXCEPTION_TYPE = HTTPFatalError    # for backward compatibility
  end

  class HTTPContinue < HTTPInformation           # 100
    HAS_BODY = false
  end
  class HTTPSwitchProtocol < HTTPInformation     # 101
    HAS_BODY = false
  end

  class HTTPOK < HTTPSuccess                            # 200
    HAS_BODY = true
  end
  class HTTPCreated < HTTPSuccess                       # 201
    HAS_BODY = true
  end
  class HTTPAccepted < HTTPSuccess                      # 202
    HAS_BODY = true
  end
  class HTTPNonAuthoritativeInformation < HTTPSuccess   # 203
    HAS_BODY = true
  end
  class HTTPNoContent < HTTPSuccess                     # 204
    HAS_BODY = false
  end
  class HTTPResetContent < HTTPSuccess                  # 205
    HAS_BODY = false
  end
  class HTTPPartialContent < HTTPSuccess                # 206
    HAS_BODY = true
  end

  class HTTPMultipleChoice < HTTPRedirection     # 300
    HAS_BODY = true
  end
  class HTTPMovedPermanently < HTTPRedirection   # 301
    HAS_BODY = true
  end
  class HTTPFound < HTTPRedirection              # 302
    HAS_BODY = true
  end
  HTTPMovedTemporarily = HTTPFound
  class HTTPSeeOther < HTTPRedirection           # 303
    HAS_BODY = true
  end
  class HTTPNotModified < HTTPRedirection        # 304
    HAS_BODY = false
  end
  class HTTPUseProxy < HTTPRedirection           # 305
    HAS_BODY = false
  end
  # 306 unused
  class HTTPTemporaryRedirect < HTTPRedirection  # 307
    HAS_BODY = true
  end

  class HTTPBadRequest < HTTPClientError                    # 400
    HAS_BODY = true
  end
  class HTTPUnauthorized < HTTPClientError                  # 401
    HAS_BODY = true
  end
  class HTTPPaymentRequired < HTTPClientError               # 402
    HAS_BODY = true
  end
  class HTTPForbidden < HTTPClientError                     # 403
    HAS_BODY = true
  end
  class HTTPNotFound < HTTPClientError                      # 404
    HAS_BODY = true
  end
  class HTTPMethodNotAllowed < HTTPClientError              # 405
    HAS_BODY = true
  end
  class HTTPNotAcceptable < HTTPClientError                 # 406
    HAS_BODY = true
  end
  class HTTPProxyAuthenticationRequired < HTTPClientError   # 407
    HAS_BODY = true
  end
  class HTTPRequestTimeOut < HTTPClientError                # 408
    HAS_BODY = true
  end
  class HTTPConflict < HTTPClientError                      # 409
    HAS_BODY = true
  end
  class HTTPGone < HTTPClientError                          # 410
    HAS_BODY = true
  end
  class HTTPLengthRequired < HTTPClientError                # 411
    HAS_BODY = true
  end
  class HTTPPreconditionFailed < HTTPClientError            # 412
    HAS_BODY = true
  end
  class HTTPRequestEntityTooLarge < HTTPClientError         # 413
    HAS_BODY = true
  end
  class HTTPRequestURITooLong < HTTPClientError             # 414
    HAS_BODY = true
  end
  HTTPRequestURITooLarge = HTTPRequestURITooLong
  class HTTPUnsupportedMediaType < HTTPClientError          # 415
    HAS_BODY = true
  end
  class HTTPRequestedRangeNotSatisfiable < HTTPClientError  # 416
    HAS_BODY = true
  end
  class HTTPExpectationFailed < HTTPClientError             # 417
    HAS_BODY = true
  end

  class HTTPInternalServerError < HTTPServerError   # 500
    HAS_BODY = true
  end
  class HTTPNotImplemented < HTTPServerError        # 501
    HAS_BODY = true
  end
  class HTTPBadGateway < HTTPServerError            # 502
    HAS_BODY = true
  end
  class HTTPServiceUnavailable < HTTPServerError    # 503
    HAS_BODY = true
  end
  class HTTPGatewayTimeOut < HTTPServerError        # 504
    HAS_BODY = true
  end
  class HTTPVersionNotSupported < HTTPServerError   # 505
    HAS_BODY = true
  end

  # :startdoc:


  class HTTPResponse   # reopen

    CODE_CLASS_TO_OBJ = {
      '1' => HTTPInformation,
      '2' => HTTPSuccess,
      '3' => HTTPRedirection,
      '4' => HTTPClientError,
      '5' => HTTPServerError
    }
    CODE_TO_OBJ = {
      '100' => HTTPContinue,
      '101' => HTTPSwitchProtocol,

      '200' => HTTPOK,
      '201' => HTTPCreated,
      '202' => HTTPAccepted,
      '203' => HTTPNonAuthoritativeInformation,
      '204' => HTTPNoContent,
      '205' => HTTPResetContent,
      '206' => HTTPPartialContent,

      '300' => HTTPMultipleChoice,
      '301' => HTTPMovedPermanently,
      '302' => HTTPFound,
      '303' => HTTPSeeOther,
      '304' => HTTPNotModified,
      '305' => HTTPUseProxy,
      '307' => HTTPTemporaryRedirect,

      '400' => HTTPBadRequest,
      '401' => HTTPUnauthorized,
      '402' => HTTPPaymentRequired,
      '403' => HTTPForbidden,
      '404' => HTTPNotFound,
      '405' => HTTPMethodNotAllowed,
      '406' => HTTPNotAcceptable,
      '407' => HTTPProxyAuthenticationRequired,
      '408' => HTTPRequestTimeOut,
      '409' => HTTPConflict,
      '410' => HTTPGone,
      '411' => HTTPLengthRequired,
      '412' => HTTPPreconditionFailed,
      '413' => HTTPRequestEntityTooLarge,
      '414' => HTTPRequestURITooLong,
      '415' => HTTPUnsupportedMediaType,
      '416' => HTTPRequestedRangeNotSatisfiable,
      '417' => HTTPExpectationFailed,

      '500' => HTTPInternalServerError,
      '501' => HTTPNotImplemented,
      '502' => HTTPBadGateway,
      '503' => HTTPServiceUnavailable,
      '504' => HTTPGatewayTimeOut,
      '505' => HTTPVersionNotSupported
    }

    class << HTTPResponse
      def read_new(sock)   #:nodoc: internal use only
        httpv, code, msg = read_status_line(sock)
        res = response_class(code).new(httpv, code, msg)
        each_response_header(sock) do |k,v|
          res.add_field k, v
        end
        res
      end

      private

      def read_status_line(sock)
        str = sock.readline
        m = /\AHTTP(?:\/(\d+\.\d+))?\s+(\d\d\d)\s*(.*)\z/in.match(str) or
          raise HTTPBadResponse, "wrong status line: #{str.dump}"
        m.captures
      end

      def response_class(code)
        CODE_TO_OBJ[code] or
        CODE_CLASS_TO_OBJ[code[0,1]] or
        HTTPUnknownResponse
      end

      def each_response_header(sock)
        key = value = nil
        while true
          line = sock.readuntil("\n", true).sub(/\s+\z/, '')
          break if line.empty?
          if line[0] == ?\s or line[0] == ?\t and value
            value << ' ' unless value.empty?
            value << line.strip
          else
            yield key, value if key
            key, value = line.strip.split(/\s*:\s*/, 2)
            raise HTTPBadResponse, 'wrong header line format' if value.nil?
          end
        end
        yield key, value if key
      end
    end

    # next is to fix bug in RDoc, where the private inside class << self
    # spills out.
    public

    include HTTPHeader

    def initialize(httpv, code, msg)   #:nodoc: internal use only
      @http_version = httpv
      @code         = code
      @message      = msg
      initialize_http_header nil
      @body = nil
      @read = false
    end

    # The HTTP version supported by the server.
    attr_reader :http_version

    # The HTTP result code string. For example, '302'.  You can also
    # determine the response type by examining which response subclass
    # the response object is an instance of.
    attr_reader :code

    # The HTTP result message sent by the server. For example, 'Not Found'.
    attr_reader :message
    alias msg message   # :nodoc: obsolete

    def inspect
      "#<#{self.class} #{@code} #{@message} readbody=#{@read}>"
    end

    #
    # response <-> exception relationship
    #

    def code_type   #:nodoc:
      self.class
    end

    def error!   #:nodoc:
      raise error_type().new(@code + ' ' + @message.dump, self)
    end

    def error_type   #:nodoc:
      self.class::EXCEPTION_TYPE
    end

    # Raises an HTTP error if the response is not 2xx (success).
    def value
      error! unless self.kind_of?(HTTPSuccess)
    end

    #
    # header (for backward compatibility only; DO NOT USE)
    #

    def response   #:nodoc:
      warn "#{caller(1)[0]}: warning: HTTPResponse#response is obsolete" if $VERBOSE
      self
    end

    def header   #:nodoc:
      warn "#{caller(1)[0]}: warning: HTTPResponse#header is obsolete" if $VERBOSE
      self
    end

    def read_header   #:nodoc:
      warn "#{caller(1)[0]}: warning: HTTPResponse#read_header is obsolete" if $VERBOSE
      self
    end

    #
    # body
    #

    def reading_body(sock, reqmethodallowbody)  #:nodoc: internal use only
      @socket = sock
      @body_exist = reqmethodallowbody && self.class.body_permitted?
      begin
        yield
        self.body   # ensure to read body
      ensure
        @socket = nil
      end
    end

    # Gets the entity body returned by the remote HTTP server.
    #
    # If a block is given, the body is passed to the block, and
    # the body is provided in fragments, as it is read in from the socket.
    #
    # Calling this method a second or subsequent time for the same
    # HTTPResponse object will return the value already read.
    #
    #   http.request_get('/index.html') {|res|
    #     puts res.read_body
    #   }
    #
    #   http.request_get('/index.html') {|res|
    #     p res.read_body.object_id   # 538149362
    #     p res.read_body.object_id   # 538149362
    #   }
    #
    #   # using iterator
    #   http.request_get('/index.html') {|res|
    #     res.read_body do |segment|
    #       print segment
    #     end
    #   }
    #
    def read_body(dest = nil, &block)
      if @read
        raise IOError, "#{self.class}\#read_body called twice" if dest or block
        return @body
      end
      to = procdest(dest, block)
      stream_check
      if @body_exist
        read_body_0 to
        @body = to
      else
        @body = nil
      end
      @read = true

      @body
    end

    # Returns the full entity body.
    #
    # Calling this method a second or subsequent time will return the
    # string already read.
    #
    #   http.request_get('/index.html') {|res|
    #     puts res.body
    #   }
    #
    #   http.request_get('/index.html') {|res|
    #     p res.body.object_id   # 538149362
    #     p res.body.object_id   # 538149362
    #   }
    #
    def body
      read_body()
    end

    # Because it may be necessary to modify the body, Eg, decompression
    # this method facilitates that.
    def body=(value)
      @body = value
    end

    alias entity body   #:nodoc: obsolete

    private

    def read_body_0(dest)
      if chunked?
        read_chunked dest
        return
      end
      clen = content_length()
      if clen
        @socket.read clen, dest, true   # ignore EOF
        return
      end
      clen = range_length()
      if clen
        @socket.read clen, dest
        return
      end
      @socket.read_all dest
    end

    def read_chunked(dest)
      len = nil
      total = 0
      while true
        line = @socket.readline
        hexlen = line.slice(/[0-9a-fA-F]+/) or
            raise HTTPBadResponse, "wrong chunk size line: #{line}"
        len = hexlen.hex
        break if len == 0
        begin
          @socket.read len, dest
        ensure
          total += len
          @socket.read 2   # \r\n
        end
      end
      until @socket.readline.empty?
        # none
      end
    end

    def stream_check
      raise IOError, 'attempt to read body out of block' if @socket.closed?
    end

    def procdest(dest, block)
      raise ArgumentError, 'both arg and block given for HTTP method' \
          if dest and block
      if block
        ReadAdapter.new(block)
      else
        dest || ''
      end
    end

  end


  # :enddoc:

  #--
  # for backward compatibility
  class HTTP
    ProxyMod = ProxyDelta
  end
  module NetPrivate
    HTTPRequest = ::Net::HTTPRequest
  end

  HTTPInformationCode = HTTPInformation
  HTTPSuccessCode     = HTTPSuccess
  HTTPRedirectionCode = HTTPRedirection
  HTTPRetriableCode   = HTTPRedirection
  HTTPClientErrorCode = HTTPClientError
  HTTPFatalErrorCode  = HTTPClientError
  HTTPServerErrorCode = HTTPServerError
  HTTPResponceReceiver = HTTPResponse

end   # module Net
