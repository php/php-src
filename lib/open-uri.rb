require 'uri'
require 'stringio'
require 'time'

module Kernel
  private
  alias open_uri_original_open open # :nodoc:
  class << self
    alias open_uri_original_open open # :nodoc:
  end

  # Allows the opening of various resources including URIs.
  #
  # If the first argument responds to the 'open' method, 'open' is called on
  # it with the rest of the arguments.
  #
  # If the first argument is a string that begins with xxx://, it is parsed by
  # URI.parse.  If the parsed object responds to the 'open' method,
  # 'open' is called on it with the rest of the arguments.
  #
  # Otherwise, the original Kernel#open is called.
  #
  # Since open-uri.rb provides URI::HTTP#open, URI::HTTPS#open and
  # URI::FTP#open, Kernel[#.]open can accept URIs and strings that begin with
  # http://, https:// and ftp://.  In these cases, the opened file object is
  # extended by OpenURI::Meta.
  def open(name, *rest, &block) # :doc:
    if name.respond_to?(:open)
      name.open(*rest, &block)
    elsif name.respond_to?(:to_str) &&
          %r{\A[A-Za-z][A-Za-z0-9+\-\.]*://} =~ name &&
          (uri = URI.parse(name)).respond_to?(:open)
      uri.open(*rest, &block)
    else
      open_uri_original_open(name, *rest, &block)
    end
  end
  module_function :open
end

# OpenURI is an easy-to-use wrapper for net/http, net/https and net/ftp.
#
#== Example
#
# It is possible to open an http, https or ftp URL as though it were a file:
#
#   open("http://www.ruby-lang.org/") {|f|
#     f.each_line {|line| p line}
#   }
#
# The opened file has several getter methods for its meta-information, as
# follows, since it is extended by OpenURI::Meta.
#
#   open("http://www.ruby-lang.org/en") {|f|
#     f.each_line {|line| p line}
#     p f.base_uri         # <URI::HTTP:0x40e6ef2 URL:http://www.ruby-lang.org/en/>
#     p f.content_type     # "text/html"
#     p f.charset          # "iso-8859-1"
#     p f.content_encoding # []
#     p f.last_modified    # Thu Dec 05 02:45:02 UTC 2002
#   }
#
# Additional header fields can be specified by an optional hash argument.
#
#   open("http://www.ruby-lang.org/en/",
#     "User-Agent" => "Ruby/#{RUBY_VERSION}",
#     "From" => "foo@bar.invalid",
#     "Referer" => "http://www.ruby-lang.org/") {|f|
#     # ...
#   }
#
# The environment variables such as http_proxy, https_proxy and ftp_proxy
# are in effect by default.  :proxy => nil disables proxy.
#
#   open("http://www.ruby-lang.org/en/raa.html", :proxy => nil) {|f|
#     # ...
#   }
#
# URI objects can be opened in a similar way.
#
#   uri = URI.parse("http://www.ruby-lang.org/en/")
#   uri.open {|f|
#     # ...
#   }
#
# URI objects can be read directly. The returned string is also extended by
# OpenURI::Meta.
#
#   str = uri.read
#   p str.base_uri
#
# Author:: Tanaka Akira <akr@m17n.org>

module OpenURI
  Options = {
    :proxy => true,
    :proxy_http_basic_authentication => true,
    :progress_proc => true,
    :content_length_proc => true,
    :http_basic_authentication => true,
    :read_timeout => true,
    :ssl_ca_cert => nil,
    :ssl_verify_mode => nil,
    :ftp_active_mode => false,
    :redirect => true,
  }

  def OpenURI.check_options(options) # :nodoc:
    options.each {|k, v|
      next unless Symbol === k
      unless Options.include? k
        raise ArgumentError, "unrecognized option: #{k}"
      end
    }
  end

  def OpenURI.scan_open_optional_arguments(*rest) # :nodoc:
    if !rest.empty? && (String === rest.first || Integer === rest.first)
      mode = rest.shift
      if !rest.empty? && Integer === rest.first
        perm = rest.shift
      end
    end
    return mode, perm, rest
  end

  def OpenURI.open_uri(name, *rest) # :nodoc:
    uri = URI::Generic === name ? name : URI.parse(name)
    mode, _, rest = OpenURI.scan_open_optional_arguments(*rest)
    options = rest.shift if !rest.empty? && Hash === rest.first
    raise ArgumentError.new("extra arguments") if !rest.empty?
    options ||= {}
    OpenURI.check_options(options)

    if /\Arb?(?:\Z|:([^:]+))/ =~ mode
      encoding, = $1,Encoding.find($1) if $1
      mode = nil
    end

    unless mode == nil ||
           mode == 'r' || mode == 'rb' ||
           mode == File::RDONLY
      raise ArgumentError.new("invalid access mode #{mode} (#{uri.class} resource is read only.)")
    end

    io = open_loop(uri, options)
    io.set_encoding(encoding) if encoding
    if block_given?
      begin
        yield io
      ensure
        io.close
      end
    else
      io
    end
  end

  def OpenURI.open_loop(uri, options) # :nodoc:
    proxy_opts = []
    proxy_opts << :proxy_http_basic_authentication if options.include? :proxy_http_basic_authentication
    proxy_opts << :proxy if options.include? :proxy
    proxy_opts.compact!
    if 1 < proxy_opts.length
      raise ArgumentError, "multiple proxy options specified"
    end
    case proxy_opts.first
    when :proxy_http_basic_authentication
      opt_proxy, proxy_user, proxy_pass = options.fetch(:proxy_http_basic_authentication)
      proxy_user = proxy_user.to_str
      proxy_pass = proxy_pass.to_str
      if opt_proxy == true
        raise ArgumentError.new("Invalid authenticated proxy option: #{options[:proxy_http_basic_authentication].inspect}")
      end
    when :proxy
      opt_proxy = options.fetch(:proxy)
      proxy_user = nil
      proxy_pass = nil
    when nil
      opt_proxy = true
      proxy_user = nil
      proxy_pass = nil
    end
    case opt_proxy
    when true
      find_proxy = lambda {|u| pxy = u.find_proxy; pxy ? [pxy, nil, nil] : nil}
    when nil, false
      find_proxy = lambda {|u| nil}
    when String
      opt_proxy = URI.parse(opt_proxy)
      find_proxy = lambda {|u| [opt_proxy, proxy_user, proxy_pass]}
    when URI::Generic
      find_proxy = lambda {|u| [opt_proxy, proxy_user, proxy_pass]}
    else
      raise ArgumentError.new("Invalid proxy option: #{opt_proxy}")
    end

    uri_set = {}
    buf = nil
    while true
      redirect = catch(:open_uri_redirect) {
        buf = Buffer.new
        uri.buffer_open(buf, find_proxy.call(uri), options)
        nil
      }
      if redirect
        if redirect.relative?
          # Although it violates RFC2616, Location: field may have relative
          # URI.  It is converted to absolute URI using uri as a base URI.
          redirect = uri + redirect
        end
        if !options.fetch(:redirect, true)
          raise HTTPRedirect.new(buf.io.status.join(' '), buf.io, redirect)
        end
        unless OpenURI.redirectable?(uri, redirect)
          raise "redirection forbidden: #{uri} -> #{redirect}"
        end
        if options.include? :http_basic_authentication
          # send authentication only for the URI directly specified.
          options = options.dup
          options.delete :http_basic_authentication
        end
        uri = redirect
        raise "HTTP redirection loop: #{uri}" if uri_set.include? uri.to_s
        uri_set[uri.to_s] = true
      else
        break
      end
    end
    io = buf.io
    io.base_uri = uri
    io
  end

  def OpenURI.redirectable?(uri1, uri2) # :nodoc:
    # This test is intended to forbid a redirection from http://... to
    # file:///etc/passwd, file:///dev/zero, etc.  CVE-2011-1521
    # https to http redirect is also forbidden intentionally.
    # It avoids sending secure cookie or referer by non-secure HTTP protocol.
    # (RFC 2109 4.3.1, RFC 2965 3.3, RFC 2616 15.1.3)
    # However this is ad hoc.  It should be extensible/configurable.
    uri1.scheme.downcase == uri2.scheme.downcase ||
    (/\A(?:http|ftp)\z/i =~ uri1.scheme && /\A(?:http|ftp)\z/i =~ uri2.scheme)
  end

  def OpenURI.open_http(buf, target, proxy, options) # :nodoc:
    if proxy
      proxy_uri, proxy_user, proxy_pass = proxy
      raise "Non-HTTP proxy URI: #{proxy_uri}" if proxy_uri.class != URI::HTTP
    end

    if target.userinfo && "1.9.0" <= RUBY_VERSION
      # don't raise for 1.8 because compatibility.
      raise ArgumentError, "userinfo not supported.  [RFC3986]"
    end

    header = {}
    options.each {|k, v| header[k] = v if String === k }

    require 'net/http'
    klass = Net::HTTP
    if URI::HTTP === target
      # HTTP or HTTPS
      if proxy
        if proxy_user && proxy_pass
          klass = Net::HTTP::Proxy(proxy_uri.hostname, proxy_uri.port, proxy_user, proxy_pass)
        else
          klass = Net::HTTP::Proxy(proxy_uri.hostname, proxy_uri.port)
        end
      end
      target_host = target.hostname
      target_port = target.port
      request_uri = target.request_uri
    else
      # FTP over HTTP proxy
      target_host = proxy_uri.hostname
      target_port = proxy_uri.port
      request_uri = target.to_s
      if proxy_user && proxy_pass
        header["Proxy-Authorization"] = 'Basic ' + ["#{proxy_user}:#{proxy_pass}"].pack('m').delete("\r\n")
      end
    end

    http = klass.new(target_host, target_port)
    if target.class == URI::HTTPS
      require 'net/https'
      http.use_ssl = true
      http.verify_mode = options[:ssl_verify_mode] || OpenSSL::SSL::VERIFY_PEER
      store = OpenSSL::X509::Store.new
      if options[:ssl_ca_cert]
        if File.directory? options[:ssl_ca_cert]
          store.add_path options[:ssl_ca_cert]
        else
          store.add_file options[:ssl_ca_cert]
        end
      else
        store.set_default_paths
      end
      http.cert_store = store
    end
    if options.include? :read_timeout
      http.read_timeout = options[:read_timeout]
    end

    resp = nil
    http.start {
      req = Net::HTTP::Get.new(request_uri, header)
      if options.include? :http_basic_authentication
        user, pass = options[:http_basic_authentication]
        req.basic_auth user, pass
      end
      http.request(req) {|response|
        resp = response
        if options[:content_length_proc] && Net::HTTPSuccess === resp
          if resp.key?('Content-Length')
            options[:content_length_proc].call(resp['Content-Length'].to_i)
          else
            options[:content_length_proc].call(nil)
          end
        end
        resp.read_body {|str|
          buf << str
          if options[:progress_proc] && Net::HTTPSuccess === resp
            options[:progress_proc].call(buf.size)
          end
        }
      }
    }
    io = buf.io
    io.rewind
    io.status = [resp.code, resp.message]
    resp.each {|name,value| buf.io.meta_add_field name, value }
    case resp
    when Net::HTTPSuccess
    when Net::HTTPMovedPermanently, # 301
         Net::HTTPFound, # 302
         Net::HTTPSeeOther, # 303
         Net::HTTPTemporaryRedirect # 307
      begin
        loc_uri = URI.parse(resp['location'])
      rescue URI::InvalidURIError
        raise OpenURI::HTTPError.new(io.status.join(' ') + ' (Invalid Location URI)', io)
      end
      throw :open_uri_redirect, loc_uri
    else
      raise OpenURI::HTTPError.new(io.status.join(' '), io)
    end
  end

  class HTTPError < StandardError
    def initialize(message, io)
      super(message)
      @io = io
    end
    attr_reader :io
  end

  class HTTPRedirect < HTTPError
    def initialize(message, io, uri)
      super(message, io)
      @uri = uri
    end
    attr_reader :uri
  end

  class Buffer # :nodoc:
    def initialize
      @io = StringIO.new
      @size = 0
    end
    attr_reader :size

    StringMax = 10240
    def <<(str)
      @io << str
      @size += str.length
      if StringIO === @io && StringMax < @size
        require 'tempfile'
        io = Tempfile.new('open-uri')
        io.binmode
        Meta.init io, @io if Meta === @io
        io << @io.string
        @io = io
      end
    end

    def io
      Meta.init @io unless Meta === @io
      @io
    end
  end

  # Mixin for holding meta-information.
  module Meta
    def Meta.init(obj, src=nil) # :nodoc:
      obj.extend Meta
      obj.instance_eval {
        @base_uri = nil
        @meta = {}
      }
      if src
        obj.status = src.status
        obj.base_uri = src.base_uri
        src.meta.each {|name, value|
          obj.meta_add_field(name, value)
        }
      end
    end

    # returns an Array that consists of status code and message.
    attr_accessor :status

    # returns a URI that is the base of relative URIs in the data.
    # It may differ from the URI supplied by a user due to redirection.
    attr_accessor :base_uri

    # returns a Hash that represents header fields.
    # The Hash keys are downcased for canonicalization.
    attr_reader :meta

    def meta_setup_encoding # :nodoc:
      charset = self.charset
      enc = nil
      if charset
        begin
          enc = Encoding.find(charset)
        rescue ArgumentError
        end
      end
      enc = Encoding::ASCII_8BIT unless enc
      if self.respond_to? :force_encoding
        self.force_encoding(enc)
      elsif self.respond_to? :string
        self.string.force_encoding(enc)
      else # Tempfile
        self.set_encoding enc
      end
    end

    def meta_add_field(name, value) # :nodoc:
      name = name.downcase
      @meta[name] = value
      meta_setup_encoding if name == 'content-type'
    end

    # returns a Time that represents the Last-Modified field.
    def last_modified
      if v = @meta['last-modified']
        Time.httpdate(v)
      else
        nil
      end
    end

    RE_LWS = /[\r\n\t ]+/n
    RE_TOKEN = %r{[^\x00- ()<>@,;:\\"/\[\]?={}\x7f]+}n
    RE_QUOTED_STRING = %r{"(?:[\r\n\t !#-\[\]-~\x80-\xff]|\\[\x00-\x7f])*"}n
    RE_PARAMETERS = %r{(?:;#{RE_LWS}?#{RE_TOKEN}#{RE_LWS}?=#{RE_LWS}?(?:#{RE_TOKEN}|#{RE_QUOTED_STRING})#{RE_LWS}?)*}n

    def content_type_parse # :nodoc:
      v = @meta['content-type']
      # The last (?:;#{RE_LWS}?)? matches extra ";" which violates RFC2045.
      if v && %r{\A#{RE_LWS}?(#{RE_TOKEN})#{RE_LWS}?/(#{RE_TOKEN})#{RE_LWS}?(#{RE_PARAMETERS})(?:;#{RE_LWS}?)?\z}no =~ v
        type = $1.downcase
        subtype = $2.downcase
        parameters = []
        $3.scan(/;#{RE_LWS}?(#{RE_TOKEN})#{RE_LWS}?=#{RE_LWS}?(?:(#{RE_TOKEN})|(#{RE_QUOTED_STRING}))/no) {|att, val, qval|
          if qval
            val = qval[1...-1].gsub(/[\r\n\t !#-\[\]-~\x80-\xff]+|(\\[\x00-\x7f])/n) { $1 ? $1[1,1] : $& }
          end
          parameters << [att.downcase, val]
        }
        ["#{type}/#{subtype}", *parameters]
      else
        nil
      end
    end

    # returns "type/subtype" which is MIME Content-Type.
    # It is downcased for canonicalization.
    # Content-Type parameters are stripped.
    def content_type
      type, *_ = content_type_parse
      type || 'application/octet-stream'
    end

    # returns a charset parameter in Content-Type field.
    # It is downcased for canonicalization.
    #
    # If charset parameter is not given but a block is given,
    # the block is called and its result is returned.
    # It can be used to guess charset.
    #
    # If charset parameter and block is not given,
    # nil is returned except text type in HTTP.
    # In that case, "iso-8859-1" is returned as defined by RFC2616 3.7.1.
    def charset
      type, *parameters = content_type_parse
      if pair = parameters.assoc('charset')
        pair.last.downcase
      elsif block_given?
        yield
      elsif type && %r{\Atext/} =~ type &&
            @base_uri && /\Ahttp\z/i =~ @base_uri.scheme
        "iso-8859-1" # RFC2616 3.7.1
      else
        nil
      end
    end

    # returns a list of encodings in Content-Encoding field
    # as an Array of String.
    # The encodings are downcased for canonicalization.
    def content_encoding
      v = @meta['content-encoding']
      if v && %r{\A#{RE_LWS}?#{RE_TOKEN}#{RE_LWS}?(?:,#{RE_LWS}?#{RE_TOKEN}#{RE_LWS}?)*}o =~ v
        v.scan(RE_TOKEN).map {|content_coding| content_coding.downcase}
      else
        []
      end
    end
  end

  # Mixin for HTTP and FTP URIs.
  module OpenRead
    # OpenURI::OpenRead#open provides `open' for URI::HTTP and URI::FTP.
    #
    # OpenURI::OpenRead#open takes optional 3 arguments as:
    #
    #   OpenURI::OpenRead#open([mode [, perm]] [, options]) [{|io| ... }]
    #
    # OpenURI::OpenRead#open returns an IO-like object if block is not given.
    # Otherwise it yields the IO object and return the value of the block.
    # The IO object is extended with OpenURI::Meta.
    #
    # +mode+ and +perm+ are the same as Kernel#open.
    #
    # However, +mode+ must be read mode because OpenURI::OpenRead#open doesn't
    # support write mode (yet).
    # Also +perm+ is ignored because it is meaningful only for file creation.
    #
    # +options+ must be a hash.
    #
    # Each option with a string key specifies an extra header field for HTTP.
    # I.e., it is ignored for FTP without HTTP proxy.
    #
    # The hash may include other options, where keys are symbols:
    #
    # [:proxy]
    #  Synopsis:
    #    :proxy => "http://proxy.foo.com:8000/"
    #    :proxy => URI.parse("http://proxy.foo.com:8000/")
    #    :proxy => true
    #    :proxy => false
    #    :proxy => nil
    #
    #  If :proxy option is specified, the value should be String, URI,
    #  boolean or nil.
    #
    #  When String or URI is given, it is treated as proxy URI.
    #
    #  When true is given or the option itself is not specified,
    #  environment variable `scheme_proxy' is examined.
    #  `scheme' is replaced by `http', `https' or `ftp'.
    #
    #  When false or nil is given, the environment variables are ignored and
    #  connection will be made to a server directly.
    #
    # [:proxy_http_basic_authentication]
    #  Synopsis:
    #    :proxy_http_basic_authentication =>
    #      ["http://proxy.foo.com:8000/", "proxy-user", "proxy-password"]
    #    :proxy_http_basic_authentication =>
    #      [URI.parse("http://proxy.foo.com:8000/"),
    #       "proxy-user", "proxy-password"]
    #
    #  If :proxy option is specified, the value should be an Array with 3
    #  elements.  It should contain a proxy URI, a proxy user name and a proxy
    #  password.  The proxy URI should be a String, an URI or nil.  The proxy
    #  user name and password should be a String.
    #
    #  If nil is given for the proxy URI, this option is just ignored.
    #
    #  If :proxy and :proxy_http_basic_authentication is specified,
    #  ArgumentError is raised.
    #
    # [:http_basic_authentication]
    #  Synopsis:
    #    :http_basic_authentication=>[user, password]
    #
    #  If :http_basic_authentication is specified,
    #  the value should be an array which contains 2 strings:
    #  username and password.
    #  It is used for HTTP Basic authentication defined by RFC 2617.
    #
    # [:content_length_proc]
    #  Synopsis:
    #    :content_length_proc => lambda {|content_length| ... }
    #
    #  If :content_length_proc option is specified, the option value procedure
    #  is called before actual transfer is started.
    #  It takes one argument, which is expected content length in bytes.
    #
    #  If two or more transfer is done by HTTP redirection, the procedure
    #  is called only one for a last transfer.
    #
    #  When expected content length is unknown, the procedure is called with
    #  nil.  This happens when the HTTP response has no Content-Length header.
    #
    # [:progress_proc]
    #  Synopsis:
    #    :progress_proc => lambda {|size| ...}
    #
    #  If :progress_proc option is specified, the proc is called with one
    #  argument each time when `open' gets content fragment from network.
    #  The argument +size+ is the accumulated transferred size in bytes.
    #
    #  If two or more transfer is done by HTTP redirection, the procedure
    #  is called only one for a last transfer.
    #
    #  :progress_proc and :content_length_proc are intended to be used for
    #  progress bar.
    #  For example, it can be implemented as follows using Ruby/ProgressBar.
    #
    #    pbar = nil
    #    open("http://...",
    #      :content_length_proc => lambda {|t|
    #        if t && 0 < t
    #          pbar = ProgressBar.new("...", t)
    #          pbar.file_transfer_mode
    #        end
    #      },
    #      :progress_proc => lambda {|s|
    #        pbar.set s if pbar
    #      }) {|f| ... }
    #
    # [:read_timeout]
    #  Synopsis:
    #    :read_timeout=>nil     (no timeout)
    #    :read_timeout=>10      (10 second)
    #
    #  :read_timeout option specifies a timeout of read for http connections.
    #
    # [:ssl_ca_cert]
    #  Synopsis:
    #    :ssl_ca_cert=>filename
    #
    #  :ssl_ca_cert is used to specify CA certificate for SSL.
    #  If it is given, default certificates are not used.
    #
    # [:ssl_verify_mode]
    #  Synopsis:
    #    :ssl_verify_mode=>mode
    #
    #  :ssl_verify_mode is used to specify openssl verify mode.
    #
    # [:ftp_active_mode]
    #  Synopsis:
    #    :ftp_active_mode=>bool
    #
    #  <tt>:ftp_active_mode => true</tt> is used to make ftp active mode.
    #  Ruby 1.9 uses passive mode by default.
    #  Note that the active mode is default in Ruby 1.8 or prior.
    #
    # [:redirect]
    #  Synopsis:
    #    :redirect=>bool
    #
    #  +:redirect+ is true by default.  <tt>:redirect => false</tt> is used to
    #  disable all HTTP redirects.
    #
    #  OpenURI::HTTPRedirect exception raised on redirection.
    #  Using +true+ also means that redirections between http and ftp are
    #  permitted.
    #
    def open(*rest, &block)
      OpenURI.open_uri(self, *rest, &block)
    end

    # OpenURI::OpenRead#read([options]) reads a content referenced by self and
    # returns the content as string.
    # The string is extended with OpenURI::Meta.
    # The argument +options+ is same as OpenURI::OpenRead#open.
    def read(options={})
      self.open(options) {|f|
        str = f.read
        Meta.init str, f
        str
      }
    end
  end
end

module URI
  class Generic
    # returns a proxy URI.
    # The proxy URI is obtained from environment variables such as http_proxy,
    # ftp_proxy, no_proxy, etc.
    # If there is no proper proxy, nil is returned.
    #
    # Note that capitalized variables (HTTP_PROXY, FTP_PROXY, NO_PROXY, etc.)
    # are examined too.
    #
    # But http_proxy and HTTP_PROXY is treated specially under CGI environment.
    # It's because HTTP_PROXY may be set by Proxy: header.
    # So HTTP_PROXY is not used.
    # http_proxy is not used too if the variable is case insensitive.
    # CGI_HTTP_PROXY can be used instead.
    def find_proxy
      name = self.scheme.downcase + '_proxy'
      proxy_uri = nil
      if name == 'http_proxy' && ENV.include?('REQUEST_METHOD') # CGI?
        # HTTP_PROXY conflicts with *_proxy for proxy settings and
        # HTTP_* for header information in CGI.
        # So it should be careful to use it.
        pairs = ENV.reject {|k, v| /\Ahttp_proxy\z/i !~ k }
        case pairs.length
        when 0 # no proxy setting anyway.
          proxy_uri = nil
        when 1
          k, _ = pairs.shift
          if k == 'http_proxy' && ENV[k.upcase] == nil
            # http_proxy is safe to use because ENV is case sensitive.
            proxy_uri = ENV[name]
          else
            proxy_uri = nil
          end
        else # http_proxy is safe to use because ENV is case sensitive.
          proxy_uri = ENV.to_hash[name]
        end
        if !proxy_uri
          # Use CGI_HTTP_PROXY.  cf. libwww-perl.
          proxy_uri = ENV["CGI_#{name.upcase}"]
        end
      elsif name == 'http_proxy'
        unless proxy_uri = ENV[name]
          if proxy_uri = ENV[name.upcase]
            warn 'The environment variable HTTP_PROXY is discouraged.  Use http_proxy.'
          end
        end
      else
        proxy_uri = ENV[name] || ENV[name.upcase]
      end

      if proxy_uri && self.hostname
        require 'socket'
        begin
          addr = IPSocket.getaddress(self.hostname)
          proxy_uri = nil if /\A127\.|\A::1\z/ =~ addr
        rescue SocketError
        end
      end

      if proxy_uri
        proxy_uri = URI.parse(proxy_uri)
        name = 'no_proxy'
        if no_proxy = ENV[name] || ENV[name.upcase]
          no_proxy.scan(/([^:,]*)(?::(\d+))?/) {|host, port|
            if /(\A|\.)#{Regexp.quote host}\z/i =~ self.host &&
               (!port || self.port == port.to_i)
              proxy_uri = nil
              break
            end
          }
        end
        proxy_uri
      else
        nil
      end
    end
  end

  class HTTP
    def buffer_open(buf, proxy, options) # :nodoc:
      OpenURI.open_http(buf, self, proxy, options)
    end

    include OpenURI::OpenRead
  end

  class FTP
    def buffer_open(buf, proxy, options) # :nodoc:
      if proxy
        OpenURI.open_http(buf, self, proxy, options)
        return
      end
      require 'net/ftp'

      path = self.path
      path = path.sub(%r{\A/}, '%2F') # re-encode the beginning slash because uri library decodes it.
      directories = path.split(%r{/}, -1)
      directories.each {|d|
        d.gsub!(/%([0-9A-Fa-f][0-9A-Fa-f])/) { [$1].pack("H2") }
      }
      unless filename = directories.pop
        raise ArgumentError, "no filename: #{self.inspect}"
      end
      directories.each {|d|
        if /[\r\n]/ =~ d
          raise ArgumentError, "invalid directory: #{d.inspect}"
        end
      }
      if /[\r\n]/ =~ filename
        raise ArgumentError, "invalid filename: #{filename.inspect}"
      end
      typecode = self.typecode
      if typecode && /\A[aid]\z/ !~ typecode
        raise ArgumentError, "invalid typecode: #{typecode.inspect}"
      end

      # The access sequence is defined by RFC 1738
      ftp = Net::FTP.new
      ftp.connect(self.hostname, self.port)
      ftp.passive = true if !options[:ftp_active_mode]
      # todo: extract user/passwd from .netrc.
      user = 'anonymous'
      passwd = nil
      user, passwd = self.userinfo.split(/:/) if self.userinfo
      ftp.login(user, passwd)
      directories.each {|cwd|
        ftp.voidcmd("CWD #{cwd}")
      }
      if typecode
        # xxx: typecode D is not handled.
        ftp.voidcmd("TYPE #{typecode.upcase}")
      end
      if options[:content_length_proc]
        options[:content_length_proc].call(ftp.size(filename))
      end
      ftp.retrbinary("RETR #{filename}", 4096) { |str|
        buf << str
        options[:progress_proc].call(buf.size) if options[:progress_proc]
      }
      ftp.close
      buf.io.rewind
    end

    include OpenURI::OpenRead
  end
end
