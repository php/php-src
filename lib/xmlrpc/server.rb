=begin
= xmlrpc/server.rb
Copyright (C) 2001, 2002, 2003, 2005 by Michael Neumann (mneumann@ntecs.de)

Released under the same term of license as Ruby.

= Classes
* ((<XMLRPC::BasicServer>))
* ((<XMLRPC::CGIServer>))
* ((<XMLRPC::ModRubyServer>))
* ((<XMLRPC::Server>))
* ((<XMLRPC::WEBrickServlet>))

= XMLRPC::BasicServer
== Description
Is the base class for all XML-RPC server-types (CGI, standalone).
You can add handler and set a default handler.
Do not use this server, as this is/should be an abstract class.

=== How the method to call is found
The arity (number of accepted arguments) of a handler (method or (({Proc})) object) is
compared to the given arguments submitted by the client for a RPC ((-Remote Procedure Call-)).
A handler is only called if it accepts the number of arguments, otherwise the search
for another handler will go on. When at the end no handler was found,
the ((<default_handler|XMLRPC::BasicServer#set_default_handler>)) will be called.
With this technique it is possible to do overloading by number of parameters, but
only for (({Proc})) handler, because you cannot define two methods of the same name in
the same class.


== Class Methods
--- XMLRPC::BasicServer.new( class_delim="." )
    Creates a new (({XMLRPC::BasicServer})) instance, which should not be
    done, because (({XMLRPC::BasicServer})) is an abstract class. This
    method should be called from a subclass indirectly by a (({super})) call
    in the method (({initialize})). The paramter ((|class_delim|)) is used
    in ((<add_handler|XMLRPC::BasicServer#add_handler>)) when an object is
    added as handler, to delimit the object-prefix and the method-name.

== Instance Methods
--- XMLRPC::BasicServer#add_handler( name, signature=nil, help=nil ) { aBlock }
    Adds ((|aBlock|)) to the list of handlers, with ((|name|)) as the name of the method.
    Parameters ((|signature|)) and ((|help|)) are used by the Introspection method if specified,
    where ((|signature|)) is either an Array containing strings each representing a type of it's
    signature (the first is the return value) or an Array of Arrays if the method has multiple
    signatures. Value type-names are "int, boolean, double, string, dateTime.iso8601, base64, array, struct".

    Parameter ((|help|)) is a String with informations about how to call this method etc.

    A handler method or code-block can return the types listed at
    ((<XMLRPC::Client#call|URL:client.html#index:0>)).
    When a method fails, it can tell it the client by throwing an
    (({XMLRPC::FaultException})) like in this example:
        s.add_handler("michael.div") do |a,b|
          if b == 0
            raise XMLRPC::FaultException.new(1, "division by zero")
          else
            a / b
          end
        end
    The client gets in the case of (({b==0})) an object back of type
    (({XMLRPC::FaultException})) that has a ((|faultCode|)) and ((|faultString|))
    field.

--- XMLRPC::BasicServer#add_handler( prefix, obj )
    This is the second form of ((<add_handler|XMLRPC::BasicServer#add_handler>)).
    To add an object write:
        server.add_handler("michael", MyHandlerClass.new)
    All public methods of (({MyHandlerClass})) are accessible to
    the XML-RPC clients by (('michael."name of method"')). This is
    where the ((|class_delim|)) in ((<new|XMLRPC::BasicServer.new>))
    has it's role, a XML-RPC method-name is defined by
    ((|prefix|)) + ((|class_delim|)) + (('"name of method"')).

--- XMLRPC::BasicServer#add_handler( interface, obj )
    This is the third form of ((<add_handler|XMLRPC::BasicServer#add_handler>)).

    Use (({XMLRPC::interface})) to generate an ServiceInterface object, which
    represents an interface (with signature and help text) for a handler class.

    Parameter ((|interface|)) must be of type (({XMLRPC::ServiceInterface})).
    Adds all methods of ((|obj|)) which are defined in ((|interface|)) to the
    server.

    This is the recommended way of adding services to a server!


--- XMLRPC::BasicServer#get_default_handler
    Returns the default-handler, which is called when no handler for
    a method-name is found.
    It is a (({Proc})) object or (({nil})).

--- XMLRPC::BasicServer#set_default_handler ( &handler )
    Sets ((|handler|)) as the default-handler, which is called when
    no handler for a method-name is found. ((|handler|)) is a code-block.
    The default-handler is called with the (XML-RPC) method-name as first argument, and
    the other arguments are the parameters given by the client-call.

    If no block is specified the default of (({XMLRPC::BasicServer})) is used, which raises a
    XMLRPC::FaultException saying "method missing".


--- XMLRPC::BasicServer#set_writer( writer )
    Sets the XML writer to use for generating XML output.
    Should be an instance of a class from module (({XMLRPC::XMLWriter})).
    If this method is not called, then (({XMLRPC::Config::DEFAULT_WRITER})) is used.

--- XMLRPC::BasicServer#set_parser( parser )
    Sets the XML parser to use for parsing XML documents.
    Should be an instance of a class from module (({XMLRPC::XMLParser})).
    If this method is not called, then (({XMLRPC::Config::DEFAULT_PARSER})) is used.

--- XMLRPC::BasicServer#add_introspection
    Adds the introspection handlers "system.listMethods", "system.methodSignature" and "system.methodHelp",
    where only the first one works.

--- XMLRPC::BasicServer#add_multicall
    Adds the multi-call handler "system.multicall".

--- XMLRPC::BasicServer#get_service_hook
    Returns the service-hook, which is called on each service request (RPC) unless it's (({nil})).

--- XMLRPC::BasicServer#set_service_hook ( &handler )
    A service-hook is called for each service request (RPC).
    You can use a service-hook for example to wrap existing methods and catch exceptions of them or
    convert values to values recognized by XMLRPC. You can disable it by passing (({nil})) as parameter
    ((|handler|)) .

    The service-hook is called with a (({Proc})) object and with the parameters for this (({Proc})).
    An example:

       server.set_service_hook {|obj, *args|
         begin
           ret = obj.call(*args)  # call the original service-method
           # could convert the return value
         rescue
           # rescue exceptions
         end
       }

=end



require "xmlrpc/parser"
require "xmlrpc/create"
require "xmlrpc/config"
require "xmlrpc/utils"         # ParserWriterChooseMixin



module XMLRPC


class BasicServer

  include ParserWriterChooseMixin
  include ParseContentType

  ERR_METHOD_MISSING        = 1
  ERR_UNCAUGHT_EXCEPTION    = 2
  ERR_MC_WRONG_PARAM        = 3
  ERR_MC_MISSING_PARAMS     = 4
  ERR_MC_MISSING_METHNAME   = 5
  ERR_MC_RECURSIVE_CALL     = 6
  ERR_MC_WRONG_PARAM_PARAMS = 7
  ERR_MC_EXPECTED_STRUCT    = 8


  def initialize(class_delim=".")
    @handler = []
    @default_handler = nil
    @service_hook = nil

    @class_delim = class_delim
    @create = nil
    @parser = nil

    add_multicall     if Config::ENABLE_MULTICALL
    add_introspection if Config::ENABLE_INTROSPECTION
  end

  def add_handler(prefix, obj_or_signature=nil, help=nil, &block)
    if block_given?
      # proc-handler
      @handler << [prefix, block, obj_or_signature, help]
    else
      if prefix.kind_of? String
        # class-handler
        raise ArgumentError, "Expected non-nil value" if obj_or_signature.nil?
        @handler << [prefix + @class_delim, obj_or_signature]
      elsif prefix.kind_of? XMLRPC::Service::BasicInterface
        # class-handler with interface
        # add all methods
        @handler += prefix.get_methods(obj_or_signature, @class_delim)
      else
        raise ArgumentError, "Wrong type for parameter 'prefix'"
      end
    end
    self
  end

  def get_service_hook
    @service_hook
  end

  def set_service_hook(&handler)
    @service_hook = handler
    self
  end

  def get_default_handler
    @default_handler
  end

  def set_default_handler (&handler)
    @default_handler = handler
    self
  end

  def add_multicall
    add_handler("system.multicall", %w(array array), "Multicall Extension") do |arrStructs|
      unless arrStructs.is_a? Array
        raise XMLRPC::FaultException.new(ERR_MC_WRONG_PARAM, "system.multicall expects an array")
      end

      arrStructs.collect {|call|
        if call.is_a? Hash
          methodName = call["methodName"]
          params     = call["params"]

          if params.nil?
            multicall_fault(ERR_MC_MISSING_PARAMS, "Missing params")
          elsif methodName.nil?
            multicall_fault(ERR_MC_MISSING_METHNAME, "Missing methodName")
          else
            if methodName == "system.multicall"
              multicall_fault(ERR_MC_RECURSIVE_CALL, "Recursive system.multicall forbidden")
            else
              unless params.is_a? Array
                multicall_fault(ERR_MC_WRONG_PARAM_PARAMS, "Parameter params have to be an Array")
              else
                ok, val = call_method(methodName, *params)
                if ok
                  # correct return value
                  [val]
                else
                  # exception
                  multicall_fault(val.faultCode, val.faultString)
                end
              end
            end
          end

        else
          multicall_fault(ERR_MC_EXPECTED_STRUCT, "system.multicall expected struct")
        end
      }
    end # end add_handler
    self
  end

  def add_introspection
    add_handler("system.listMethods",%w(array), "List methods available on this XML-RPC server") do
      methods = []
      @handler.each do |name, obj|
        if obj.kind_of? Proc
          methods << name
        else
          obj.class.public_instance_methods(false).each do |meth|
            methods << "#{name}#{meth}"
          end
        end
      end
      methods
    end

    add_handler("system.methodSignature", %w(array string), "Returns method signature") do |meth|
      sigs = []
      @handler.each do |name, obj, sig|
        if obj.kind_of? Proc and sig != nil and name == meth
          if sig[0].kind_of? Array
            # sig contains multiple signatures, e.g. [["array"], ["array", "string"]]
            sig.each {|s| sigs << s}
          else
            # sig is a single signature, e.g. ["array"]
            sigs << sig
          end
        end
      end
      sigs.uniq! || sigs  # remove eventually duplicated signatures
    end

    add_handler("system.methodHelp", %w(string string), "Returns help on using this method") do |meth|
      help = nil
      @handler.each do |name, obj, sig, hlp|
        if obj.kind_of? Proc and name == meth
          help = hlp
          break
        end
      end
      help || ""
    end

    self
  end



  def process(data)
    method, params = parser().parseMethodCall(data)
    handle(method, *params)
  end

  private # --------------------------------------------------------------

  def multicall_fault(nr, str)
    {"faultCode" => nr, "faultString" => str}
  end

  #
  # method dispatch
  #
  def dispatch(methodname, *args)
    for name, obj in @handler
      if obj.kind_of? Proc
        next unless methodname == name
      else
        next unless methodname =~ /^#{name}(.+)$/
        next unless obj.respond_to? $1
        obj = obj.method($1)
      end

      if check_arity(obj, args.size)
        if @service_hook.nil?
          return obj.call(*args)
        else
          return @service_hook.call(obj, *args)
        end
      end
    end

    if @default_handler.nil?
      raise XMLRPC::FaultException.new(ERR_METHOD_MISSING, "Method #{methodname} missing or wrong number of parameters!")
    else
      @default_handler.call(methodname, *args)
    end
  end


  #
  # returns true, if the arity of "obj" matches
  #
  def check_arity(obj, n_args)
    ary = obj.arity

    if ary >= 0
      n_args == ary
    else
      n_args >= (ary+1).abs
    end
  end



  def call_method(methodname, *args)
    begin
      [true, dispatch(methodname, *args)]
    rescue XMLRPC::FaultException => e
      [false, e]
    rescue Exception => e
      [false, XMLRPC::FaultException.new(ERR_UNCAUGHT_EXCEPTION, "Uncaught exception #{e.message} in method #{methodname}")]
    end
  end

  #
  #
  #
  def handle(methodname, *args)
    create().methodResponse(*call_method(methodname, *args))
  end


end


=begin
= XMLRPC::CGIServer
== Synopsis
    require "xmlrpc/server"

    s = XMLRPC::CGIServer.new

    s.add_handler("michael.add") do |a,b|
      a + b
    end

    s.add_handler("michael.div") do |a,b|
      if b == 0
        raise XMLRPC::FaultException.new(1, "division by zero")
      else
        a / b
      end
    end

    s.set_default_handler do |name, *args|
      raise XMLRPC::FaultException.new(-99, "Method #{name} missing" +
                                       " or wrong number of parameters!")
    end

    s.serve

== Description
Implements a CGI-based XML-RPC server.

== Superclass
((<XMLRPC::BasicServer>))

== Class Methods
--- XMLRPC::CGIServer.new( *a )
    Creates a new (({XMLRPC::CGIServer})) instance. All parameters given
    are by-passed to ((<XMLRPC::BasicServer.new>)). You can only create
    ((*one*)) (({XMLRPC::CGIServer})) instance, because more than one makes
    no sense.

== Instance Methods
--- XMLRPC::CGIServer#serve
    Call this after you have added all you handlers to the server.
    This method processes a XML-RPC methodCall and sends the answer
    back to the client.
    Make sure that you don't write to standard-output in a handler, or in
    any other part of your program, this would case a CGI-based server to fail!
=end

class CGIServer < BasicServer
  @@obj = nil

  def CGIServer.new(*a)
    @@obj = super(*a) if @@obj.nil?
    @@obj
  end

  def initialize(*a)
    super(*a)
  end

  def serve
    catch(:exit_serve) {
      length = ENV['CONTENT_LENGTH'].to_i

      http_error(405, "Method Not Allowed") unless ENV['REQUEST_METHOD'] == "POST"
      http_error(400, "Bad Request")        unless parse_content_type(ENV['CONTENT_TYPE']).first == "text/xml"
      http_error(411, "Length Required")    unless length > 0

      # TODO: do we need a call to binmode?
      $stdin.binmode if $stdin.respond_to? :binmode
      data = $stdin.read(length)

      http_error(400, "Bad Request")        if data.nil? or data.bytesize != length

      http_write(process(data), "Content-type" => "text/xml; charset=utf-8")
    }
  end


  private

  def http_error(status, message)
    err = "#{status} #{message}"
    msg = <<-"MSGEND"
      <html>
        <head>
          <title>#{err}</title>
        </head>
        <body>
          <h1>#{err}</h1>
          <p>Unexpected error occured while processing XML-RPC request!</p>
        </body>
      </html>
    MSGEND

    http_write(msg, "Status" => err, "Content-type" => "text/html")
    throw :exit_serve # exit from the #serve method
  end

  def http_write(body, header)
    h = {}
    header.each {|key, value| h[key.to_s.capitalize] = value}
    h['Status']         ||= "200 OK"
    h['Content-length'] ||= body.bytesize.to_s

    str = ""
    h.each {|key, value| str << "#{key}: #{value}\r\n"}
    str << "\r\n#{body}"

    print str
  end

end

=begin
= XMLRPC::ModRubyServer
== Description
Implements a XML-RPC server, which works with Apache mod_ruby.

Use it in the same way as CGIServer!

== Superclass
((<XMLRPC::BasicServer>))
=end

class ModRubyServer < BasicServer

  def initialize(*a)
    @ap = Apache::request
    super(*a)
  end

  def serve
    catch(:exit_serve) {
      header = {}
      @ap.headers_in.each {|key, value| header[key.capitalize] = value}

      length = header['Content-length'].to_i

      http_error(405, "Method Not Allowed") unless @ap.request_method  == "POST"
      http_error(400, "Bad Request")        unless parse_content_type(header['Content-type']).first == "text/xml"
      http_error(411, "Length Required")    unless length > 0

      # TODO: do we need a call to binmode?
      @ap.binmode
      data = @ap.read(length)

      http_error(400, "Bad Request")        if data.nil? or data.bytesize != length

      http_write(process(data), 200, "Content-type" => "text/xml; charset=utf-8")
    }
  end


  private

  def http_error(status, message)
    err = "#{status} #{message}"
    msg = <<-"MSGEND"
      <html>
        <head>
          <title>#{err}</title>
        </head>
        <body>
          <h1>#{err}</h1>
          <p>Unexpected error occured while processing XML-RPC request!</p>
        </body>
      </html>
    MSGEND

    http_write(msg, status, "Status" => err, "Content-type" => "text/html")
    throw :exit_serve # exit from the #serve method
  end

  def http_write(body, status, header)
    h = {}
    header.each {|key, value| h[key.to_s.capitalize] = value}
    h['Status']         ||= "200 OK"
    h['Content-length'] ||= body.bytesize.to_s

    h.each {|key, value| @ap.headers_out[key] = value }
    @ap.content_type = h["Content-type"]
    @ap.status = status.to_i
    @ap.send_http_header

    @ap.print body
  end

end

=begin
= XMLRPC::Server
== Synopsis
    require "xmlrpc/server"

    s = XMLRPC::Server.new(8080)

    s.add_handler("michael.add") do |a,b|
      a + b
    end

    s.add_handler("michael.div") do |a,b|
      if b == 0
        raise XMLRPC::FaultException.new(1, "division by zero")
      else
        a / b
      end
    end

    s.set_default_handler do |name, *args|
      raise XMLRPC::FaultException.new(-99, "Method #{name} missing" +
                                       " or wrong number of parameters!")
    end

    s.serve

== Description
Implements a standalone XML-RPC server. The method (({serve}))) is left if a SIGHUP is sent to the
program.

== Superclass
((<XMLRPC::WEBrickServlet>))

== Class Methods
--- XMLRPC::Server.new( port=8080, host="127.0.0.1", maxConnections=4, stdlog=$stdout, audit=true, debug=true, *a )
    Creates a new (({XMLRPC::Server})) instance, which is a XML-RPC server listening on
    port ((|port|)) and accepts requests for the host ((|host|)), which is by default only the localhost.
    The server is not started, to start it you have to call ((<serve|XMLRPC::Server#serve>)).

    Parameters ((|audit|)) and ((|debug|)) are obsolete!

    All additionally given parameters in ((|*a|)) are by-passed to ((<XMLRPC::BasicServer.new>)).

== Instance Methods
--- XMLRPC::Server#serve
    Call this after you have added all you handlers to the server.
    This method starts the server to listen for XML-RPC requests and answer them.

--- XMLRPC::Server#shutdown
    Stops and shuts the server down.

=end

class WEBrickServlet < BasicServer; end # forward declaration

class Server < WEBrickServlet

  def initialize(port=8080, host="127.0.0.1", maxConnections=4, stdlog=$stdout, audit=true, debug=true, *a)
    super(*a)
    require 'webrick'
    @server = WEBrick::HTTPServer.new(:Port => port, :BindAddress => host, :MaxClients => maxConnections,
                                      :Logger => WEBrick::Log.new(stdlog))
    @server.mount("/", self)
  end

  def serve
    signals = %w[INT TERM HUP] & Signal.list.keys
    signals.each { |signal| trap(signal) { @server.shutdown } }

    @server.start
  end

  def shutdown
    @server.shutdown
  end

end

=begin
= XMLRPC::WEBrickServlet
== Synopsis

    require "webrick"
    require "xmlrpc/server"

    s = XMLRPC::WEBrickServlet.new
    s.add_handler("michael.add") do |a,b|
      a + b
    end

    s.add_handler("michael.div") do |a,b|
      if b == 0
        raise XMLRPC::FaultException.new(1, "division by zero")
      else
        a / b
      end
    end

    s.set_default_handler do |name, *args|
      raise XMLRPC::FaultException.new(-99, "Method #{name} missing" +
                                       " or wrong number of parameters!")
    end

    httpserver = WEBrick::HTTPServer.new(:Port => 8080)
    httpserver.mount("/RPC2", s)
    trap("HUP") { httpserver.shutdown }   # use 1 instead of "HUP" on Windows
    httpserver.start

== Instance Methods

--- XMLRPC::WEBrickServlet#set_valid_ip( *ip_addr )
    Specifies the valid IP addresses that are allowed to connect to the server.
    Each IP is either a (({String})) or a (({Regexp})).

--- XMLRPC::WEBrickServlet#get_valid_ip
    Return the via method ((<set_valid_ip|XMLRPC::Server#set_valid_ip>)) specified
    valid IP addresses.

== Description
Implements a servlet for use with WEBrick, a pure Ruby (HTTP-) server framework.

== Superclass
((<XMLRPC::BasicServer>))

=end

class WEBrickServlet < BasicServer
  def initialize(*a)
    super
    require "webrick/httpstatus"
    @valid_ip = nil
  end

  # deprecated from WEBrick/1.2.2.
  # but does not break anything.
  def require_path_info?
    false
  end

  def get_instance(config, *options)
    # TODO: set config & options
    self
  end

  def set_valid_ip(*ip_addr)
    if ip_addr.size == 1 and ip_addr[0].nil?
      @valid_ip = nil
    else
      @valid_ip = ip_addr
    end
  end

  def get_valid_ip
    @valid_ip
  end

  def service(request, response)

    if @valid_ip
      raise WEBrick::HTTPStatus::Forbidden unless @valid_ip.any? { |ip| request.peeraddr[3] =~ ip }
    end

    if request.request_method != "POST"
      raise WEBrick::HTTPStatus::MethodNotAllowed,
            "unsupported method `#{request.request_method}'."
    end

    if parse_content_type(request['Content-type']).first != "text/xml"
      raise WEBrick::HTTPStatus::BadRequest
    end

    length = (request['Content-length'] || 0).to_i

    raise WEBrick::HTTPStatus::LengthRequired unless length > 0

    data = request.body

    if data.nil? or data.bytesize != length
      raise WEBrick::HTTPStatus::BadRequest
    end

    resp = process(data)
    if resp.nil? or resp.bytesize <= 0
      raise WEBrick::HTTPStatus::InternalServerError
    end

    response.status = 200
    response['Content-Length'] = resp.bytesize
    response['Content-Type']   = "text/xml; charset=utf-8"
    response.body = resp
  end
end


end # module XMLRPC


=begin
= History
    $Id$
=end

