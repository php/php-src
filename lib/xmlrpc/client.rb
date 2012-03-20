=begin
= xmlrpc/client.rb
Copyright (C) 2001, 2002, 2003 by Michael Neumann (mneumann@ntecs.de)

Released under the same term of license as Ruby.

= Classes
* ((<XMLRPC::Client>))
* ((<XMLRPC::Client::Proxy>))


= XMLRPC::Client
== Synopsis
    require "xmlrpc/client"

    server = XMLRPC::Client.new("www.ruby-lang.org", "/RPC2", 80)
    begin
      param = server.call("michael.add", 4, 5)
      puts "4 + 5 = #{param}"
    rescue XMLRPC::FaultException => e
      puts "Error:"
      puts e.faultCode
      puts e.faultString
    end

or

    require "xmlrpc/client"

    server = XMLRPC::Client.new("www.ruby-lang.org", "/RPC2", 80)
    ok, param = server.call2("michael.add", 4, 5)
    if ok then
      puts "4 + 5 = #{param}"
    else
      puts "Error:"
      puts param.faultCode
      puts param.faultString
    end

== Description
Class (({XMLRPC::Client})) provides remote procedure calls to a XML-RPC server.
After setting the connection-parameters with ((<XMLRPC::Client.new>)) which
creates a new (({XMLRPC::Client})) instance, you can execute a remote procedure
by sending the ((<call|XMLRPC::Client#call>)) or ((<call2|XMLRPC::Client#call2>))
message to this new instance. The given parameters indicate which method to
call on the remote-side and of course the parameters for the remote procedure.

== Class Methods
--- XMLRPC::Client.new( host=nil, path=nil, port=nil, proxy_host=nil, proxy_port=nil, user=nil, password=nil, use_ssl=false, timeout =nil)
    Creates an object which represents the remote XML-RPC server on the
    given host ((|host|)). If the server is CGI-based, ((|path|)) is the
    path to the CGI-script, which will be called, otherwise (in the
    case of a standalone server) ((|path|)) should be (({"/RPC2"})).
    ((|port|)) is the port on which the XML-RPC server listens.
    If ((|proxy_host|)) is given, then a proxy server listening at
    ((|proxy_host|)) is used. ((|proxy_port|)) is the port of the
    proxy server.

    Default values for ((|host|)), ((|path|)) and ((|port|)) are 'localhost', '/RPC2' and
    '80' respectively using SSL '443'.

    If ((|user|)) and ((|password|)) are given, each time a request is send,
    a Authorization header is send. Currently only Basic Authentification is
    implemented no Digest.

    If ((|use_ssl|)) is set to (({true})), comunication over SSL is enabled.
    Note, that you need the SSL package from RAA installed.

    Parameter ((|timeout|)) is the time to wait for a XML-RPC response, defaults to 30.

--- XMLRPC::Client.new2( uri, proxy=nil, timeout=nil)
--- XMLRPC::Client.new_from_uri( uri, proxy=nil, timeout=nil)
:   uri
    URI specifying protocol (http or https), host, port, path, user and password.
    Example: https://user:password@host:port/path

:   proxy
    Is of the form "host:port".

:   timeout
    Defaults to 30.

--- XMLRPC::Client.new3( hash={} )
--- XMLRPC::Client.new_from_hash( hash={} )
    Parameter ((|hash|)) has following case-insensitive keys:
    * host
    * path
    * port
    * proxy_host
    * proxy_port
    * user
    * password
    * use_ssl
    * timeout

    Calls ((<XMLRPC::Client.new>)) with the corresponding values.

== Instance Methods
--- XMLRPC::Client#call( method, *args )
    Invokes the method named ((|method|)) with the parameters given by
    ((|args|)) on the XML-RPC server.
    The parameter ((|method|)) is converted into a (({String})) and should
    be a valid XML-RPC method-name.
    Each parameter of ((|args|)) must be of one of the following types,
    where (({Hash})), (({Struct})) and (({Array})) can contain any of these listed ((:types:)):
    * (({Fixnum})), (({Bignum}))
    * (({TrueClass})), (({FalseClass})) ((({true})), (({false})))
    * (({String})), (({Symbol}))
    * (({Float}))
    * (({Hash})), (({Struct}))
    * (({Array}))
    * (({Date})), (({Time})), (({XMLRPC::DateTime}))
    * (({XMLRPC::Base64}))
    * A Ruby object which class includes XMLRPC::Marshallable (only if Config::ENABLE_MARSHALLABLE is (({true}))).
      That object is converted into a hash, with one additional key/value pair "___class___" which contains the class name
      for restoring later that object.

    The method returns the return-value from the RPC
    ((-stands for Remote Procedure Call-)).
    The type of the return-value is one of the above shown,
    only that a (({Bignum})) is only allowed when it fits in 32-bit and
    that a XML-RPC (('dateTime.iso8601')) type is always returned as
    a ((<(({XMLRPC::DateTime}))|URL:datetime.html>)) object and
    a (({Struct})) is never returned, only a (({Hash})), the same for a (({Symbol})), where
    always a (({String})) is returned.
    A (({XMLRPC::Base64})) is returned as a (({String})) from xmlrpc4r version 1.6.1 on.

    If the remote procedure returned a fault-structure, then a
    (({XMLRPC::FaultException})) exception is raised, which has two accessor-methods
    (({faultCode})) and (({faultString})) of type (({Integer})) and (({String})).

--- XMLRPC::Client#call2( method, *args )
    The difference between this method and ((<call|XMLRPC::Client#call>)) is, that
    this method do ((*not*)) raise a (({XMLRPC::FaultException})) exception.
    The method returns an array of two values. The first value indicates if
    the second value is a return-value ((({true}))) or an object of type
    (({XMLRPC::FaultException})).
    Both are explained in ((<call|XMLRPC::Client#call>)).

    Simple to remember: The "2" in "call2" denotes the number of values it returns.

--- XMLRPC::Client#multicall( *methods )
    You can use this method to execute several methods on a XMLRPC server which supports
    the multi-call extension.
    Example:

      s.multicall(
        ['michael.add', 3, 4],
        ['michael.sub', 4, 5]
      )
      # => [7, -1]

--- XMLRPC::Client#multicall2( *methods )
    Same as ((<XMLRPC::Client#multicall>)), but returns like ((<XMLRPC::Client#call2>)) two parameters
    instead of raising an (({XMLRPC::FaultException})).

--- XMLRPC::Client#proxy( prefix, *args )
    Returns an object of class (({XMLRPC::Client::Proxy})), initialized with
    ((|prefix|)) and ((|args|)). A proxy object returned by this method behaves
    like ((<XMLRPC::Client#call>)), i.e. a call on that object will raise a
    (({XMLRPC::FaultException})) when a fault-structure is returned by that call.

--- XMLRPC::Client#proxy2( prefix, *args )
    Almost the same like ((<XMLRPC::Client#proxy>)) only that a call on the returned
    (({XMLRPC::Client::Proxy})) object behaves like ((<XMLRPC::Client#call2>)), i.e.
    a call on that object will return two parameters.




--- XMLRPC::Client#call_async(...)
--- XMLRPC::Client#call2_async(...)
--- XMLRPC::Client#multicall_async(...)
--- XMLRPC::Client#multicall2_async(...)
--- XMLRPC::Client#proxy_async(...)
--- XMLRPC::Client#proxy2_async(...)
    In contrast to corresponding methods without "_async", these can be
    called concurrently and use for each request a new connection, where the
    non-asynchronous counterparts use connection-alive (one connection for all requests)
    if possible.

    Note, that you have to use Threads to call these methods concurrently.
    The following example calls two methods concurrently:

      Thread.new {
        p client.call_async("michael.add", 4, 5)
      }

      Thread.new {
        p client.call_async("michael.div", 7, 9)
      }


--- XMLRPC::Client#timeout
--- XMLRPC::Client#user
--- XMLRPC::Client#password
    Return the corresponding attributes.

--- XMLRPC::Client#timeout= (new_timeout)
--- XMLRPC::Client#user= (new_user)
--- XMLRPC::Client#password= (new_password)
    Set the corresponding attributes.


--- XMLRPC::Client#set_writer( writer )
    Sets the XML writer to use for generating XML output.
    Should be an instance of a class from module (({XMLRPC::XMLWriter})).
    If this method is not called, then (({XMLRPC::Config::DEFAULT_WRITER})) is used.

--- XMLRPC::Client#set_parser( parser )
    Sets the XML parser to use for parsing XML documents.
    Should be an instance of a class from module (({XMLRPC::XMLParser})).
    If this method is not called, then (({XMLRPC::Config::DEFAULT_PARSER})) is used.

--- XMLRPC::Client#cookie
--- XMLRPC::Client#cookie= (cookieString)
    Get and set the HTTP Cookie header.

--- XMLRPC::Client#http_header_extra= (additionalHeaders)
    Set extra HTTP headers that are included in the request.

--- XMLRPC::Client#http_header_extra
    Access the via ((<XMLRPC::Client#http_header_extra=>)) assigned header.

--- XMLRPC::Client#http_last_response
    Returns the (({Net::HTTPResponse})) object of the last RPC.

= XMLRPC::Client::Proxy
== Synopsis
    require "xmlrpc/client"

    server = XMLRPC::Client.new("www.ruby-lang.org", "/RPC2", 80)

    michael  = server.proxy("michael")
    michael2 = server.proxy("michael", 4)

    # both calls should return the same value '9'.
    p michael.add(4,5)
    p michael2.add(5)

== Description
Class (({XMLRPC::Client::Proxy})) makes XML-RPC calls look nicer!
You can call any method onto objects of that class - the object handles
(({method_missing})) and will forward the method call to a XML-RPC server.
Don't use this class directly, but use instead method ((<XMLRPC::Client#proxy>)) or
((<XMLRPC::Client#proxy2>)).

== Class Methods
--- XMLRPC::Client::Proxy.new( server, prefix, args=[], meth=:call, delim="." )
    Creates an object which provides (({method_missing})).

    ((|server|)) must be of type (({XMLRPC::Client})), which is the XML-RPC server to be used
    for a XML-RPC call. ((|prefix|)) and ((|delim|)) will be prepended to the methodname
    called onto this object.

    Parameter ((|meth|)) is the method (call, call2, call_async, call2_async) to use for
    a RPC.

    ((|args|)) are arguments which are automatically given
    to every XML-RPC call before the arguments provides through (({method_missing})).

== Instance Methods
Every method call is forwarded to the XML-RPC server defined in ((<new|XMLRPC::Client::Proxy#new>)).

Note: Inherited methods from class (({Object})) cannot be used as XML-RPC names, because they get around
(({method_missing})).



= History
    $Id$

=end



require "xmlrpc/parser"
require "xmlrpc/create"
require "xmlrpc/config"
require "xmlrpc/utils"     # ParserWriterChooseMixin
require "net/http"
require "uri"

module XMLRPC

  class Client

    USER_AGENT = "XMLRPC::Client (Ruby #{RUBY_VERSION})"

    include ParserWriterChooseMixin
    include ParseContentType


    # Constructors -------------------------------------------------------------------

    def initialize(host=nil, path=nil, port=nil, proxy_host=nil, proxy_port=nil,
                   user=nil, password=nil, use_ssl=nil, timeout=nil)

      @http_header_extra = nil
      @http_last_response = nil
      @cookie = nil

      @host       = host || "localhost"
      @path       = path || "/RPC2"
      @proxy_host = proxy_host
      @proxy_port = proxy_port
      @proxy_host ||= 'localhost' if @proxy_port != nil
      @proxy_port ||= 8080 if @proxy_host != nil
      @use_ssl    = use_ssl || false
      @timeout    = timeout || 30

      if use_ssl
        require "net/https"
        @port = port || 443
      else
        @port = port || 80
      end

      @user, @password = user, password

      set_auth

      # convert ports to integers
      @port = @port.to_i if @port != nil
      @proxy_port = @proxy_port.to_i if @proxy_port != nil

      # HTTP object for synchronous calls
      @http = net_http(@host, @port, @proxy_host, @proxy_port)
      @http.use_ssl = @use_ssl if @use_ssl
      @http.read_timeout = @timeout
      @http.open_timeout = @timeout

      @parser = nil
      @create = nil
    end


    class << self

    def new2(uri, proxy=nil, timeout=nil)
      begin
        url = URI(uri)
      rescue URI::InvalidURIError => e
        raise ArgumentError, e.message, e.backtrace
      end

      unless URI::HTTP === url
        raise ArgumentError, "Wrong protocol specified. Only http or https allowed!"
      end

      proto  = url.scheme
      user   = url.user
      passwd = url.password
      host   = url.host
      port   = url.port
      path   = url.path.empty? ? nil : url.request_uri

      proxy_host, proxy_port = (proxy || "").split(":")
      proxy_port = proxy_port.to_i if proxy_port

      self.new(host, path, port, proxy_host, proxy_port, user, passwd, (proto == "https"), timeout)
    end

    alias new_from_uri new2

    def new3(hash={})

      # convert all keys into lowercase strings
      h = {}
      hash.each { |k,v| h[k.to_s.downcase] = v }

      self.new(h['host'], h['path'], h['port'], h['proxy_host'], h['proxy_port'], h['user'], h['password'],
               h['use_ssl'], h['timeout'])
    end

    alias new_from_hash new3

    end


    # Attribute Accessors -------------------------------------------------------------------

    # add additional HTTP headers to the request
    attr_accessor :http_header_extra

    # makes last HTTP response accessible
    attr_reader :http_last_response

    # Cookie support
    attr_accessor :cookie


    attr_reader :timeout, :user, :password

    def timeout=(new_timeout)
      @timeout = new_timeout
      @http.read_timeout = @timeout
      @http.open_timeout = @timeout
    end

    def user=(new_user)
      @user = new_user
      set_auth
    end

    def password=(new_password)
      @password = new_password
      set_auth
    end

    # Call methods --------------------------------------------------------------

    def call(method, *args)
      ok, param = call2(method, *args)
      if ok
        param
      else
        raise param
      end
    end

    def call2(method, *args)
      request = create().methodCall(method, *args)
      data = do_rpc(request, false)
      parser().parseMethodResponse(data)
    end

    def call_async(method, *args)
      ok, param = call2_async(method, *args)
      if ok
        param
      else
        raise param
      end
    end

    def call2_async(method, *args)
      request = create().methodCall(method, *args)
      data = do_rpc(request, true)
      parser().parseMethodResponse(data)
    end


    # Multicall methods --------------------------------------------------------------

    def multicall(*methods)
      ok, params = multicall2(*methods)
      if ok
        params
      else
        raise params
      end
    end

    def multicall2(*methods)
      gen_multicall(methods, false)
    end

    def multicall_async(*methods)
      ok, params = multicall2_async(*methods)
      if ok
        params
      else
        raise params
      end
    end

    def multicall2_async(*methods)
      gen_multicall(methods, true)
    end


    # Proxy generating methods ------------------------------------------

    def proxy(prefix=nil, *args)
      Proxy.new(self, prefix, args, :call)
    end

    def proxy2(prefix=nil, *args)
      Proxy.new(self, prefix, args, :call2)
    end

    def proxy_async(prefix=nil, *args)
      Proxy.new(self, prefix, args, :call_async)
    end

    def proxy2_async(prefix=nil, *args)
      Proxy.new(self, prefix, args, :call2_async)
    end


    private # ----------------------------------------------------------

    def net_http(host, port, proxy_host, proxy_port)
      Net::HTTP.new host, port, proxy_host, proxy_port
    end

    def set_auth
      if @user.nil?
        @auth = nil
      else
        a =  "#@user"
        a << ":#@password" if @password != nil
        @auth = "Basic " + [a].pack("m0")
      end
    end

    def do_rpc(request, async=false)
      header = {
       "User-Agent"     =>  USER_AGENT,
       "Content-Type"   => "text/xml; charset=utf-8",
       "Content-Length" => request.bytesize.to_s,
       "Connection"     => (async ? "close" : "keep-alive")
      }

      header["Cookie"] = @cookie        if @cookie
      header.update(@http_header_extra) if @http_header_extra

      if @auth != nil
        # add authorization header
        header["Authorization"] = @auth
      end

      resp = nil
      @http_last_response = nil

      if async
        # use a new HTTP object for each call
        http = net_http(@host, @port, @proxy_host, @proxy_port)
        http.use_ssl = @use_ssl if @use_ssl
        http.read_timeout = @timeout
        http.open_timeout = @timeout

        # post request
        http.start {
          resp = http.request_post(@path, request, header)
        }
      else
        # reuse the HTTP object for each call => connection alive is possible
        # we must start connection explicitely first time so that http.request
        # does not assume that we don't want keepalive
        @http.start if not @http.started?

        # post request
        resp = @http.request_post(@path, request, header)
      end

      @http_last_response = resp

      data = resp.body

      if resp.code == "401"
        # Authorization Required
        raise "Authorization failed.\nHTTP-Error: #{resp.code} #{resp.message}"
      elsif resp.code[0,1] != "2"
        raise "HTTP-Error: #{resp.code} #{resp.message}"
      end

      # assume text/xml on instances where Content-Type header is not set
      ct_expected = resp["Content-Type"] || 'text/xml'
      ct = parse_content_type(ct_expected).first
      if ct != "text/xml"
        if ct == "text/html"
          raise "Wrong content-type (received '#{ct}' but expected 'text/xml'): \n#{data}"
        else
          raise "Wrong content-type (received '#{ct}' but expected 'text/xml')"
        end
      end

      expected = resp["Content-Length"] || "<unknown>"
      if data.nil? or data.bytesize == 0
        raise "Wrong size. Was #{data.bytesize}, should be #{expected}"
      elsif expected != "<unknown>" and expected.to_i != data.bytesize and resp["Transfer-Encoding"].nil?
        raise "Wrong size. Was #{data.bytesize}, should be #{expected}"
      end

      set_cookies = resp.get_fields("Set-Cookie")
      if set_cookies and !set_cookies.empty?
        require 'webrick/cookie'
        @cookie = set_cookies.collect do |set_cookie|
          cookie = WEBrick::Cookie.parse_set_cookie(set_cookie)
          WEBrick::Cookie.new(cookie.name, cookie.value).to_s
        end.join("; ")
      end

      return data
    end

    def gen_multicall(methods=[], async=false)
      meth = :call2
      meth = :call2_async if async

      ok, params = self.send(meth, "system.multicall",
        methods.collect {|m| {'methodName' => m[0], 'params' => m[1..-1]} }
      )

      if ok
        params = params.collect do |param|
          if param.is_a? Array
            param[0]
          elsif param.is_a? Hash
            XMLRPC::FaultException.new(param["faultCode"], param["faultString"])
          else
            raise "Wrong multicall return value"
          end
        end
      end

      return ok, params
    end



    class Proxy

      def initialize(server, prefix, args=[], meth=:call, delim=".")
        @server = server
        @prefix = prefix ? prefix + delim : ""
        @args   = args
        @meth   = meth
      end

      def method_missing(mid, *args)
        pre = @prefix + mid.to_s
        arg = @args + args
        @server.send(@meth, pre, *arg)
      end

    end # class Proxy

  end # class Client

end # module XMLRPC

