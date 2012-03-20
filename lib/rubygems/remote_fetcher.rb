require 'rubygems'
require 'rubygems/user_interaction'
require 'uri'

##
# RemoteFetcher handles the details of fetching gems and gem information from
# a remote source.

class Gem::RemoteFetcher

  include Gem::UserInteraction

  ##
  # A FetchError exception wraps up the various possible IO and HTTP failures
  # that could happen while downloading from the internet.

  class FetchError < Gem::Exception

    ##
    # The URI which was being accessed when the exception happened.

    attr_accessor :uri

    def initialize(message, uri)
      super message
      @uri = uri
    end

    def to_s # :nodoc:
      "#{super} (#{uri})"
    end

  end

  @fetcher = nil

  ##
  # Cached RemoteFetcher instance.

  def self.fetcher
    @fetcher ||= self.new Gem.configuration[:http_proxy]
  end

  ##
  # Initialize a remote fetcher using the source URI and possible proxy
  # information.
  #
  # +proxy+
  # * [String]: explicit specification of proxy; overrides any environment
  #             variable setting
  # * nil: respect environment variables (HTTP_PROXY, HTTP_PROXY_USER,
  #        HTTP_PROXY_PASS)
  # * <tt>:no_proxy</tt>: ignore environment variables and _don't_ use a proxy

  def initialize(proxy = nil)
    require 'net/http'
    require 'stringio'
    require 'time'
    require 'uri'

    Socket.do_not_reverse_lookup = true

    @connections = {}
    @requests = Hash.new 0
    @proxy_uri =
      case proxy
      when :no_proxy then nil
      when nil then get_proxy_from_env
      when URI::HTTP then proxy
      else URI.parse(proxy)
      end
    @user_agent = user_agent
  end

  ##
  # Given a name and requirement, downloads this gem into cache and returns the
  # filename. Returns nil if the gem cannot be located.
  #--
  # Should probably be integrated with #download below, but that will be a
  # larger, more emcompassing effort. -erikh

  def download_to_cache dependency
    found = Gem::SpecFetcher.fetcher.fetch dependency, true, true,
                                           dependency.prerelease?

    return if found.empty?

    spec, source_uri = found.sort_by { |(s,_)| s.version }.last

    download spec, source_uri
  end

  ##
  # Moves the gem +spec+ from +source_uri+ to the cache dir unless it is
  # already there.  If the source_uri is local the gem cache dir copy is
  # always replaced.

  def download(spec, source_uri, install_dir = Gem.dir)
    Gem.ensure_gem_subdirectories(install_dir) rescue nil

    if File.writable?(install_dir)
      cache_dir = File.join install_dir, "cache"
    else
      cache_dir = File.join Gem.user_dir, "cache"
    end

    gem_file_name = File.basename spec.cache_file
    local_gem_path = File.join cache_dir, gem_file_name

    FileUtils.mkdir_p cache_dir rescue nil unless File.exist? cache_dir

   # Always escape URI's to deal with potential spaces and such
    unless URI::Generic === source_uri
      source_uri = URI.parse(URI.const_defined?(:DEFAULT_PARSER) ?
                             URI::DEFAULT_PARSER.escape(source_uri.to_s) :
                             URI.escape(source_uri.to_s))
    end

    scheme = source_uri.scheme

    # URI.parse gets confused by MS Windows paths with forward slashes.
    scheme = nil if scheme =~ /^[a-z]$/i

    case scheme
    when 'http', 'https' then
      unless File.exist? local_gem_path then
        begin
          say "Downloading gem #{gem_file_name}" if
            Gem.configuration.really_verbose

          remote_gem_path = source_uri + "gems/#{gem_file_name}"

          gem = self.fetch_path remote_gem_path
        rescue Gem::RemoteFetcher::FetchError
          raise if spec.original_platform == spec.platform

          alternate_name = "#{spec.original_name}.gem"

          say "Failed, downloading gem #{alternate_name}" if
            Gem.configuration.really_verbose

          remote_gem_path = source_uri + "gems/#{alternate_name}"

          gem = self.fetch_path remote_gem_path
        end

        File.open local_gem_path, 'wb' do |fp|
          fp.write gem
        end
      end
    when 'file' then
      begin
        path = source_uri.path
        path = File.dirname(path) if File.extname(path) == '.gem'

        remote_gem_path = correct_for_windows_path(File.join(path, 'gems', gem_file_name))

        FileUtils.cp(remote_gem_path, local_gem_path)
      rescue Errno::EACCES
        local_gem_path = source_uri.to_s
      end

      say "Using local gem #{local_gem_path}" if
        Gem.configuration.really_verbose
    when nil then # TODO test for local overriding cache
      source_path = if Gem.win_platform? && source_uri.scheme &&
                       !source_uri.path.include?(':') then
                      "#{source_uri.scheme}:#{source_uri.path}"
                    else
                      source_uri.path
                    end

      source_path = unescape source_path

      begin
        FileUtils.cp source_path, local_gem_path unless
          File.expand_path(source_path) == File.expand_path(local_gem_path)
      rescue Errno::EACCES
        local_gem_path = source_uri.to_s
      end

      say "Using local gem #{local_gem_path}" if
        Gem.configuration.really_verbose
    else
      raise Gem::InstallError, "unsupported URI scheme #{source_uri.scheme}"
    end

    local_gem_path
  end

  ##
  # File Fetcher. Dispatched by +fetch_path+. Use it instead.

  def fetch_file uri, *_
    Gem.read_binary correct_for_windows_path uri.path
  end

  ##
  # HTTP Fetcher. Dispatched by +fetch_path+. Use it instead.

  def fetch_http uri, last_modified = nil, head = false, depth = 0
    fetch_type = head ? Net::HTTP::Head : Net::HTTP::Get
    response   = request uri, fetch_type, last_modified

    case response
    when Net::HTTPOK, Net::HTTPNotModified then
      head ? response : response.body
    when Net::HTTPMovedPermanently, Net::HTTPFound, Net::HTTPSeeOther,
         Net::HTTPTemporaryRedirect then
      raise FetchError.new('too many redirects', uri) if depth > 10

      location = URI.parse response['Location']
      fetch_http(location, last_modified, head, depth + 1)
    else
      raise FetchError.new("bad response #{response.message} #{response.code}", uri)
    end
  end

  alias :fetch_https :fetch_http

  ##
  # Downloads +uri+ and returns it as a String.

  def fetch_path(uri, mtime = nil, head = false)
    uri = URI.parse uri unless URI::Generic === uri

    raise ArgumentError, "bad uri: #{uri}" unless uri
    raise ArgumentError, "uri scheme is invalid: #{uri.scheme.inspect}" unless
      uri.scheme

    data = send "fetch_#{uri.scheme}", uri, mtime, head
    data = Gem.gunzip data if data and not head and uri.to_s =~ /gz$/
    data
  rescue FetchError
    raise
  rescue Timeout::Error
    raise FetchError.new('timed out', uri.to_s)
  rescue IOError, SocketError, SystemCallError => e
    raise FetchError.new("#{e.class}: #{e}", uri.to_s)
  end

  ##
  # Returns the size of +uri+ in bytes.

  def fetch_size(uri) # TODO: phase this out
    response = fetch_path(uri, nil, true)

    response['content-length'].to_i
  end

  def escape(str)
    return unless str
    @uri_parser ||= uri_escaper
    @uri_parser.escape str
  end

  def unescape(str)
    return unless str
    @uri_parser ||= uri_escaper
    @uri_parser.unescape str
  end

  def uri_escaper
    URI::Parser.new
  rescue NameError
    URI
  end

  ##
  # Returns an HTTP proxy URI if one is set in the environment variables.

  def get_proxy_from_env
    env_proxy = ENV['http_proxy'] || ENV['HTTP_PROXY']

    return nil if env_proxy.nil? or env_proxy.empty?

    uri = URI.parse(normalize_uri(env_proxy))

    if uri and uri.user.nil? and uri.password.nil? then
      # Probably we have http_proxy_* variables?
      uri.user = escape(ENV['http_proxy_user'] || ENV['HTTP_PROXY_USER'])
      uri.password = escape(ENV['http_proxy_pass'] || ENV['HTTP_PROXY_PASS'])
    end

    uri
  end

  ##
  # Normalize the URI by adding "http://" if it is missing.

  def normalize_uri(uri)
    (uri =~ /^(https?|ftp|file):/) ? uri : "http://#{uri}"
  end

  ##
  # Creates or an HTTP connection based on +uri+, or retrieves an existing
  # connection, using a proxy if needed.

  def connection_for(uri)
    net_http_args = [uri.host, uri.port]

    if @proxy_uri then
      net_http_args += [
        @proxy_uri.host,
        @proxy_uri.port,
        @proxy_uri.user,
        @proxy_uri.password
      ]
    end

    connection_id = [Thread.current.object_id, *net_http_args].join ':'
    @connections[connection_id] ||= Net::HTTP.new(*net_http_args)
    connection = @connections[connection_id]

    if uri.scheme == 'https' and not connection.started? then
      require 'net/https'
      connection.use_ssl = true
      connection.verify_mode = OpenSSL::SSL::VERIFY_NONE
    end

    connection.start unless connection.started?

    connection
  rescue Errno::EHOSTDOWN => e
    raise FetchError.new(e.message, uri)
  end

  def correct_for_windows_path(path)
    if path[0].chr == '/' && path[1].chr =~ /[a-z]/i && path[2].chr == ':'
      path = path[1..-1]
    else
      path
    end
  end

  ##
  # Read the data from the (source based) URI, but if it is a file:// URI,
  # read from the filesystem instead.

  def open_uri_or_path(uri, last_modified = nil, head = false, depth = 0)
    raise "NO: Use fetch_path instead"
    # TODO: deprecate for fetch_path
  end

  ##
  # Performs a Net::HTTP request of type +request_class+ on +uri+ returning
  # a Net::HTTP response object.  request maintains a table of persistent
  # connections to reduce connect overhead.

  def request(uri, request_class, last_modified = nil)
    request = request_class.new uri.request_uri

    unless uri.nil? || uri.user.nil? || uri.user.empty? then
      request.basic_auth uri.user, uri.password
    end

    request.add_field 'User-Agent', @user_agent
    request.add_field 'Connection', 'keep-alive'
    request.add_field 'Keep-Alive', '30'

    if last_modified then
      last_modified = last_modified.utc
      request.add_field 'If-Modified-Since', last_modified.rfc2822
    end

    yield request if block_given?

    connection = connection_for uri

    retried = false
    bad_response = false

    begin
      @requests[connection.object_id] += 1

      say "#{request.method} #{uri}" if
        Gem.configuration.really_verbose

      file_name = File.basename(uri.path)
      # perform download progress reporter only for gems
      if request.response_body_permitted? && file_name =~ /\.gem$/
        reporter = ui.download_reporter
        response = connection.request(request) do |incomplete_response|
          if Net::HTTPOK === incomplete_response
            reporter.fetch(file_name, incomplete_response.content_length)
            downloaded = 0
            data = ''

            incomplete_response.read_body do |segment|
              data << segment
              downloaded += segment.length
              reporter.update(downloaded)
            end
            reporter.done
            if incomplete_response.respond_to? :body=
              incomplete_response.body = data
            else
              incomplete_response.instance_variable_set(:@body, data)
            end
          end
        end
      else
        response = connection.request request
      end

      say "#{response.code} #{response.message}" if
        Gem.configuration.really_verbose

    rescue Net::HTTPBadResponse
      say "bad response" if Gem.configuration.really_verbose

      reset connection

      raise FetchError.new('too many bad responses', uri) if bad_response

      bad_response = true
      retry
    # HACK work around EOFError bug in Net::HTTP
    # NOTE Errno::ECONNABORTED raised a lot on Windows, and make impossible
    # to install gems.
    rescue EOFError, Timeout::Error,
           Errno::ECONNABORTED, Errno::ECONNRESET, Errno::EPIPE

      requests = @requests[connection.object_id]
      say "connection reset after #{requests} requests, retrying" if
        Gem.configuration.really_verbose

      raise FetchError.new('too many connection resets', uri) if retried

      reset connection

      retried = true
      retry
    end

    response
  end

  ##
  # Resets HTTP connection +connection+.

  def reset(connection)
    @requests.delete connection.object_id

    connection.finish
    connection.start
  end

  def user_agent
    ua = "RubyGems/#{Gem::VERSION} #{Gem::Platform.local}"

    ruby_version = RUBY_VERSION
    ruby_version += 'dev' if RUBY_PATCHLEVEL == -1

    ua << " Ruby/#{ruby_version} (#{RUBY_RELEASE_DATE}"
    if RUBY_PATCHLEVEL >= 0 then
      ua << " patchlevel #{RUBY_PATCHLEVEL}"
    elsif defined?(RUBY_REVISION) then
      ua << " revision #{RUBY_REVISION}"
    end
    ua << ")"

    ua << " #{RUBY_ENGINE}" if defined?(RUBY_ENGINE) and RUBY_ENGINE != 'ruby'

    ua
  end

end

