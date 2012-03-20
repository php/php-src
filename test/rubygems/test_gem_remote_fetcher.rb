require 'rubygems/test_case'
require 'ostruct'
require 'webrick'
require 'rubygems/remote_fetcher'
require 'rubygems/format'

# = Testing Proxy Settings
#
# These tests check the proper proxy server settings by running two
# web servers.  The web server at http://localhost:#{SERVER_PORT}
# represents the normal gem server and returns a gemspec with a rake
# version of 0.4.11.  The web server at http://localhost:#{PROXY_PORT}
# represents the proxy server and returns a different dataset where
# rake has version 0.4.2.  This allows us to detect which server is
# returning the data.
#
# Note that the proxy server is not a *real* proxy server.  But our
# software doesn't really care, as long as we hit the proxy URL when a
# proxy is configured.

class TestGemRemoteFetcher < Gem::TestCase

  include Gem::DefaultUserInteraction

  SERVER_DATA = <<-EOY
--- !ruby/object:Gem::Cache
gems:
  rake-0.4.11: !ruby/object:Gem::Specification
    rubygems_version: "0.7"
    specification_version: 1
    name: rake
    version: !ruby/object:Gem::Version
      version: 0.4.11
    date: 2004-11-12
    summary: Ruby based make-like utility.
    require_paths:
      - lib
    author: Jim Weirich
    email: jim@weirichhouse.org
    homepage: http://rake.rubyforge.org
    rubyforge_project: rake
    description: Rake is a Make-like program implemented in Ruby. Tasks and dependencies are specified in standard Ruby syntax.
    autorequire:
    default_executable: rake
    bindir: bin
    has_rdoc: true
    required_ruby_version: !ruby/object:Gem::Version::Requirement
      requirements:
        -
          - ">"
          - !ruby/object:Gem::Version
            version: 0.0.0
      version:
    platform: ruby
    files:
      - README
    test_files: []
    library_stubs:
    rdoc_options:
    extra_rdoc_files:
    executables:
      - rake
    extensions: []
    requirements: []
    dependencies: []
  EOY

  PROXY_DATA = SERVER_DATA.gsub(/0.4.11/, '0.4.2')

  # don't let 1.8 and 1.9 autotest collide
  RUBY_VERSION =~ /(\d+)\.(\d+)\.(\d+)/
  # don't let parallel runners collide
  PROXY_PORT = process_based_port + 100 + $1.to_i * 100 + $2.to_i * 10 + $3.to_i
  SERVER_PORT = process_based_port + 200 + $1.to_i * 100 + $2.to_i * 10 + $3.to_i

  def setup
    super
    self.class.start_servers
    self.class.enable_yaml = true
    self.class.enable_zip = false
    ENV.delete 'http_proxy'
    ENV.delete 'HTTP_PROXY'
    ENV.delete 'http_proxy_user'
    ENV.delete 'HTTP_PROXY_USER'
    ENV.delete 'http_proxy_pass'
    ENV.delete 'HTTP_PROXY_PASS'

    base_server_uri = "http://localhost:#{SERVER_PORT}"
    @proxy_uri = "http://localhost:#{PROXY_PORT}"

    @server_uri = base_server_uri + "/yaml"
    @server_z_uri = base_server_uri + "/yaml.Z"

    # REFACTOR: copied from test_gem_dependency_installer.rb
    @gems_dir = File.join @tempdir, 'gems'
    @cache_dir = File.join @gemhome, "cache"
    FileUtils.mkdir @gems_dir

    # TODO: why does the remote fetcher need it written to disk?
    @a1, @a1_gem = util_gem 'a', '1' do |s| s.executables << 'a_bin' end

    Gem::RemoteFetcher.fetcher = nil

    @fetcher = Gem::RemoteFetcher.fetcher
  end

  def teardown
    super
    Gem.configuration[:http_proxy] = nil
  end

  def test_self_fetcher
    fetcher = Gem::RemoteFetcher.fetcher
    refute_nil fetcher
    assert_kind_of Gem::RemoteFetcher, fetcher
  end

  def test_self_fetcher_with_proxy
    proxy_uri = 'http://proxy.example.com'
    Gem.configuration[:http_proxy] = proxy_uri
    Gem::RemoteFetcher.fetcher = nil

    fetcher = Gem::RemoteFetcher.fetcher

    refute_nil fetcher
    assert_kind_of Gem::RemoteFetcher, fetcher
    assert_equal proxy_uri, fetcher.instance_variable_get(:@proxy_uri).to_s
  end

  def test_self_fetcher_with_proxy_URI
    proxy_uri = URI.parse 'http://proxy.example.com'
    Gem.configuration[:http_proxy] = proxy_uri
    Gem::RemoteFetcher.fetcher = nil

    fetcher = Gem::RemoteFetcher.fetcher
    refute_nil fetcher

    assert_kind_of Gem::RemoteFetcher, fetcher
    assert_equal proxy_uri, fetcher.instance_variable_get(:@proxy_uri)
  end

  def test_fetch_size_bad_uri
    fetcher = Gem::RemoteFetcher.new nil

    e = assert_raises ArgumentError do
      fetcher.fetch_size 'gems.example.com/yaml'
    end

    assert_equal 'uri scheme is invalid: nil', e.message
  end

  def test_fetch_size_socket_error
    fetcher = Gem::RemoteFetcher.new nil
    def fetcher.connection_for(uri)
      raise SocketError, "tarded"
    end

    uri = 'http://gems.example.com/yaml'
    e = assert_raises Gem::RemoteFetcher::FetchError do
      fetcher.fetch_size uri
    end

    assert_equal "SocketError: tarded (#{uri})", e.message
  end

  def test_no_proxy
    use_ui @ui do
      assert_data_from_server @fetcher.fetch_path(@server_uri)
      assert_equal SERVER_DATA.size, @fetcher.fetch_size(@server_uri)
    end
  end

  def util_fuck_with_fetcher data, blow = false
    fetcher = Gem::RemoteFetcher.fetcher
    fetcher.instance_variable_set :@test_data, data

    unless blow then
      def fetcher.fetch_path arg
        @test_arg = arg
        @test_data
      end
    else
      def fetcher.fetch_path arg
        # OMG I'm such an ass
        class << self; remove_method :fetch_path; end
        def self.fetch_path arg
          @test_arg = arg
          @test_data
        end

        raise Gem::RemoteFetcher::FetchError.new("haha!", nil)
      end
    end

    fetcher
  end

  def test_download
    a1_data = nil
    File.open @a1_gem, 'rb' do |fp|
      a1_data = fp.read
    end

    fetcher = util_fuck_with_fetcher a1_data

    a1_cache_gem = @a1.cache_file
    assert_equal a1_cache_gem, fetcher.download(@a1, 'http://gems.example.com')
    assert_equal("http://gems.example.com/gems/a-1.gem",
                 fetcher.instance_variable_get(:@test_arg).to_s)
    assert File.exist?(a1_cache_gem)
  end

  def test_download_cached
    FileUtils.mv @a1_gem, @cache_dir

    inst = Gem::RemoteFetcher.fetcher

    assert_equal @a1.cache_file, inst.download(@a1, 'http://gems.example.com')
  end

  def test_download_local
    FileUtils.mv @a1_gem, @tempdir
    local_path = File.join @tempdir, @a1.file_name
    inst = nil

    Dir.chdir @tempdir do
      inst = Gem::RemoteFetcher.fetcher
    end

    assert_equal @a1.cache_file, inst.download(@a1, local_path)
  end

  def test_download_local_space
    space_path = File.join @tempdir, 'space path'
    FileUtils.mkdir space_path
    FileUtils.mv @a1_gem, space_path
    local_path = File.join space_path, @a1.file_name
    inst = nil

    Dir.chdir @tempdir do
      inst = Gem::RemoteFetcher.fetcher
    end

    assert_equal @a1.cache_file, inst.download(@a1, local_path)
  end

  def test_download_install_dir
    a1_data = File.open @a1_gem, 'rb' do |fp|
      fp.read
    end

    fetcher = util_fuck_with_fetcher a1_data

    install_dir = File.join @tempdir, 'more_gems'

    a1_cache_gem = File.join install_dir, "cache", @a1.file_name
    FileUtils.mkdir_p(File.dirname(a1_cache_gem))
    actual = fetcher.download(@a1, 'http://gems.example.com', install_dir)

    assert_equal a1_cache_gem, actual
    assert_equal("http://gems.example.com/gems/a-1.gem",
                 fetcher.instance_variable_get(:@test_arg).to_s)

    assert File.exist?(a1_cache_gem)
  end

  unless win_platform? # File.chmod doesn't work
    def test_download_local_read_only
      FileUtils.mv @a1_gem, @tempdir
      local_path = File.join @tempdir, @a1.file_name
      inst = nil
      FileUtils.chmod 0555, @a1.cache_dir

      Dir.chdir @tempdir do
        inst = Gem::RemoteFetcher.fetcher
      end

      assert_equal File.join(@tempdir, @a1.file_name),
        inst.download(@a1, local_path)
    ensure
      FileUtils.chmod 0755, @a1.cache_dir
    end

    def test_download_read_only
      FileUtils.chmod 0555, @a1.cache_dir
      FileUtils.chmod 0555, @gemhome

      fetcher = util_fuck_with_fetcher File.read(@a1_gem)
      fetcher.download(@a1, 'http://gems.example.com')
      a1_cache_gem = File.join Gem.user_dir, "cache", @a1.file_name
      assert File.exist? a1_cache_gem
    ensure
      FileUtils.chmod 0755, @gemhome
      FileUtils.chmod 0755, @a1.cache_dir
    end
  end

  def test_download_platform_legacy
    original_platform = 'old-platform'

    e1, e1_gem = util_gem 'e', '1' do |s|
      s.platform = Gem::Platform::CURRENT
      s.instance_variable_set :@original_platform, original_platform
    end

    e1_data = nil
    File.open e1_gem, 'rb' do |fp|
      e1_data = fp.read
    end

    fetcher = util_fuck_with_fetcher e1_data, :blow_chunks

    e1_cache_gem = e1.cache_file

    assert_equal e1_cache_gem, fetcher.download(e1, 'http://gems.example.com')

    assert_equal("http://gems.example.com/gems/#{e1.original_name}.gem",
                 fetcher.instance_variable_get(:@test_arg).to_s)
    assert File.exist?(e1_cache_gem)
  end

  def test_download_same_file
    FileUtils.mv @a1_gem, @tempdir
    local_path = File.join @tempdir, @a1.file_name
    inst = nil

    Dir.chdir @tempdir do
      inst = Gem::RemoteFetcher.fetcher
    end

    cache_path = @a1.cache_file
    FileUtils.mv local_path, cache_path

    gem = Gem::Format.from_file_by_path cache_path

    assert_equal cache_path, inst.download(gem.spec, cache_path)
  end

  def test_download_unsupported
    inst = Gem::RemoteFetcher.fetcher

    e = assert_raises Gem::InstallError do
      inst.download @a1, 'ftp://gems.rubyforge.org'
    end

    assert_equal 'unsupported URI scheme ftp', e.message
  end

  def test_download_to_cache
    @a2, @a2_gem = util_gem 'a', '2'

    util_setup_spec_fetcher @a1, @a2
    @fetcher.instance_variable_set :@a1, @a1
    @fetcher.instance_variable_set :@a2, @a2
    def @fetcher.fetch_path uri, mtime = nil, head = false
      case uri.request_uri
      when /#{@a1.spec_name}/ then
        Gem.deflate Marshal.dump @a1
      when /#{@a2.spec_name}/ then
        Gem.deflate Marshal.dump @a2
      else
        uri.to_s
      end
    end

    gem = Gem::RemoteFetcher.fetcher.download_to_cache dep 'a'

    assert_equal @a2.file_name, File.basename(gem)
  end

  def test_explicit_proxy
    use_ui @ui do
      fetcher = Gem::RemoteFetcher.new @proxy_uri
      assert_equal PROXY_DATA.size, fetcher.fetch_size(@server_uri)
      assert_data_from_proxy fetcher.fetch_path(@server_uri)
    end
  end

  def test_explicit_proxy_with_user_auth
    use_ui @ui do
      uri = URI.parse @proxy_uri
      uri.user, uri.password = 'foo', 'bar'
      fetcher = Gem::RemoteFetcher.new uri.to_s
      proxy = fetcher.instance_variable_get("@proxy_uri")
      assert_equal 'foo', proxy.user
      assert_equal 'bar', proxy.password
      assert_data_from_proxy fetcher.fetch_path(@server_uri)
    end

    use_ui @ui do
      uri = URI.parse @proxy_uri
      uri.user, uri.password = 'domain%5Cuser', 'bar'
      fetcher = Gem::RemoteFetcher.new uri.to_s
      proxy = fetcher.instance_variable_get("@proxy_uri")
      assert_equal 'domain\user', fetcher.unescape(proxy.user)
      assert_equal 'bar', proxy.password
      assert_data_from_proxy fetcher.fetch_path(@server_uri)
    end

    use_ui @ui do
      uri = URI.parse @proxy_uri
      uri.user, uri.password = 'user', 'my%20pass'
      fetcher = Gem::RemoteFetcher.new uri.to_s
      proxy = fetcher.instance_variable_get("@proxy_uri")
      assert_equal 'user', proxy.user
      assert_equal 'my pass', fetcher.unescape(proxy.password)
      assert_data_from_proxy fetcher.fetch_path(@server_uri)
    end
  end

  def test_explicit_proxy_with_user_auth_in_env
    use_ui @ui do
      ENV['http_proxy'] = @proxy_uri
      ENV['http_proxy_user'] = 'foo'
      ENV['http_proxy_pass'] = 'bar'
      fetcher = Gem::RemoteFetcher.new nil
      proxy = fetcher.instance_variable_get("@proxy_uri")
      assert_equal 'foo', proxy.user
      assert_equal 'bar', proxy.password
      assert_data_from_proxy fetcher.fetch_path(@server_uri)
    end

    use_ui @ui do
      ENV['http_proxy'] = @proxy_uri
      ENV['http_proxy_user'] = 'foo\user'
      ENV['http_proxy_pass'] = 'my bar'
      fetcher = Gem::RemoteFetcher.new nil
      proxy = fetcher.instance_variable_get("@proxy_uri")
      assert_equal 'foo\user', fetcher.unescape(proxy.user)
      assert_equal 'my bar', fetcher.unescape(proxy.password)
      assert_data_from_proxy fetcher.fetch_path(@server_uri)
    end
  end

  def test_fetch_path_gzip
    fetcher = Gem::RemoteFetcher.new nil

    def fetcher.fetch_http(uri, mtime, head = nil)
      Gem.gzip 'foo'
    end

    assert_equal 'foo', fetcher.fetch_path(@uri + 'foo.gz')
  end

  def test_fetch_path_gzip_unmodified
    fetcher = Gem::RemoteFetcher.new nil

    def fetcher.fetch_http(uri, mtime, head = nil)
      nil
    end

    assert_equal nil, fetcher.fetch_path(@uri + 'foo.gz', Time.at(0))
  end

  def test_fetch_path_io_error
    fetcher = Gem::RemoteFetcher.new nil

    def fetcher.fetch_http(*)
      raise EOFError
    end

    url = 'http://example.com/uri'

    e = assert_raises Gem::RemoteFetcher::FetchError do
      fetcher.fetch_path url
    end

    assert_equal "EOFError: EOFError (#{url})", e.message
    assert_equal url, e.uri
  end

  def test_fetch_path_socket_error
    fetcher = Gem::RemoteFetcher.new nil

    def fetcher.fetch_http(uri, mtime, head = nil)
      raise SocketError
    end

    url = 'http://example.com/uri'

    e = assert_raises Gem::RemoteFetcher::FetchError do
      fetcher.fetch_path url
    end

    assert_equal "SocketError: SocketError (#{url})", e.message
    assert_equal url, e.uri
  end

  def test_fetch_path_system_call_error
    fetcher = Gem::RemoteFetcher.new nil

    def fetcher.fetch_http(uri, mtime = nil, head = nil)
      raise Errno::ECONNREFUSED, 'connect(2)'
    end

    url = 'http://example.com/uri'

    e = assert_raises Gem::RemoteFetcher::FetchError do
      fetcher.fetch_path url
    end

    assert_match %r|ECONNREFUSED:.*connect\(2\) \(#{Regexp.escape url}\)\z|,
                 e.message
    assert_equal url, e.uri
  end

  def test_fetch_path_unmodified
    fetcher = Gem::RemoteFetcher.new nil

    def fetcher.fetch_http(uri, mtime, head = nil)
      nil
    end

    assert_equal nil, fetcher.fetch_path(URI.parse(@gem_repo), Time.at(0))
  end

  def test_get_proxy_from_env_auto_normalizes
    fetcher = Gem::RemoteFetcher.new(nil)
    ENV['HTTP_PROXY'] = 'fakeurl:12345'

    assert_equal('http://fakeurl:12345', fetcher.get_proxy_from_env.to_s)
  end

  def test_get_proxy_from_env_empty
    orig_env_HTTP_PROXY = ENV['HTTP_PROXY']
    orig_env_http_proxy = ENV['http_proxy']

    ENV['HTTP_PROXY'] = ''
    ENV.delete 'http_proxy'

    fetcher = Gem::RemoteFetcher.new nil

    assert_equal nil, fetcher.send(:get_proxy_from_env)

  ensure
    orig_env_HTTP_PROXY.nil? ? ENV.delete('HTTP_PROXY') :
                               ENV['HTTP_PROXY'] = orig_env_HTTP_PROXY
    orig_env_http_proxy.nil? ? ENV.delete('http_proxy') :
                               ENV['http_proxy'] = orig_env_http_proxy
  end

  def test_implicit_no_proxy
    use_ui @ui do
      ENV['http_proxy'] = 'http://fakeurl:12345'
      fetcher = Gem::RemoteFetcher.new :no_proxy
      assert_data_from_server fetcher.fetch_path(@server_uri)
    end
  end

  def test_implicit_proxy
    use_ui @ui do
      ENV['http_proxy'] = @proxy_uri
      fetcher = Gem::RemoteFetcher.new nil
      assert_data_from_proxy fetcher.fetch_path(@server_uri)
    end
  end

  def test_implicit_upper_case_proxy
    use_ui @ui do
      ENV['HTTP_PROXY'] = @proxy_uri
      fetcher = Gem::RemoteFetcher.new nil
      assert_data_from_proxy fetcher.fetch_path(@server_uri)
    end
  end

  def test_implicit_proxy_no_env
    use_ui @ui do
      fetcher = Gem::RemoteFetcher.new nil
      assert_data_from_server fetcher.fetch_path(@server_uri)
    end
  end

  def test_fetch_http
    fetcher = Gem::RemoteFetcher.new nil
    url = 'http://gems.example.com/redirect'

    conn = Object.new
    def conn.started?() true end
    def conn.request(req)
      url = 'http://gems.example.com/redirect'
      unless defined? @requested then
        @requested = true
        res = Net::HTTPMovedPermanently.new nil, 301, nil
        res.add_field 'Location', url
        res
      else
        res = Net::HTTPOK.new nil, 200, nil
        def res.body() 'real_path' end
        res
      end
    end

    conn = { "#{Thread.current.object_id}:gems.example.com:80" => conn }
    fetcher.instance_variable_set :@connections, conn

    data = fetcher.fetch_http URI.parse(url)

    assert_equal 'real_path', data
  end

  def test_fetch_http_redirects
    fetcher = Gem::RemoteFetcher.new nil
    url = 'http://gems.example.com/redirect'

    conn = Object.new
    def conn.started?() true end
    def conn.request(req)
      url = 'http://gems.example.com/redirect'
      res = Net::HTTPMovedPermanently.new nil, 301, nil
      res.add_field 'Location', url
      res
    end

    conn = { "#{Thread.current.object_id}:gems.example.com:80" => conn }
    fetcher.instance_variable_set :@connections, conn

    e = assert_raises Gem::RemoteFetcher::FetchError do
      fetcher.fetch_http URI.parse(url)
    end

    assert_equal "too many redirects (#{url})", e.message
  end

  def test_request
    uri = URI.parse "#{@gem_repo}/specs.#{Gem.marshal_version}"
    util_stub_connection_for :body => :junk, :code => 200

    response = @fetcher.request uri, Net::HTTP::Get

    assert_equal 200, response.code
    assert_equal :junk, response.body
  end

  def test_request_head
    uri = URI.parse "#{@gem_repo}/specs.#{Gem.marshal_version}"
    util_stub_connection_for :body => '', :code => 200
    response = @fetcher.request uri, Net::HTTP::Head

    assert_equal 200, response.code
    assert_equal '', response.body
  end

  def test_request_unmodified
    uri = URI.parse "#{@gem_repo}/specs.#{Gem.marshal_version}"
    conn = util_stub_connection_for :body => '', :code => 304

    t = Time.now
    response = @fetcher.request uri, Net::HTTP::Head, t

    assert_equal 304, response.code
    assert_equal '', response.body

    assert_equal t.rfc2822, conn.payload['if-modified-since']
  end

  def test_user_agent
    ua = @fetcher.user_agent

    assert_match %r%^RubyGems/\S+ \S+ Ruby/\S+ \(.*?\)%,          ua
    assert_match %r%RubyGems/#{Regexp.escape Gem::VERSION}%,      ua
    assert_match %r% #{Regexp.escape Gem::Platform.local.to_s} %, ua
    assert_match %r%Ruby/#{Regexp.escape RUBY_VERSION}%,          ua
    assert_match %r%\(#{Regexp.escape RUBY_RELEASE_DATE} %,       ua
  end

  def test_user_agent_engine
    util_save_version

    Object.send :remove_const, :RUBY_ENGINE if defined?(RUBY_ENGINE)
    Object.send :const_set,    :RUBY_ENGINE, 'vroom'

    ua = @fetcher.user_agent

    assert_match %r%\) vroom%, ua
  ensure
    util_restore_version
  end

  def test_user_agent_engine_ruby
    util_save_version

    Object.send :remove_const, :RUBY_ENGINE if defined?(RUBY_ENGINE)
    Object.send :const_set,    :RUBY_ENGINE, 'ruby'

    ua = @fetcher.user_agent

    assert_match %r%\)%, ua
  ensure
    util_restore_version
  end

  def test_user_agent_patchlevel
    util_save_version

    Object.send :remove_const, :RUBY_PATCHLEVEL
    Object.send :const_set,    :RUBY_PATCHLEVEL, 5

    ua = @fetcher.user_agent

    assert_match %r% patchlevel 5\)%, ua
  ensure
    util_restore_version
  end

  def test_user_agent_revision
    util_save_version

    Object.send :remove_const, :RUBY_PATCHLEVEL
    Object.send :const_set,    :RUBY_PATCHLEVEL, -1
    Object.send :remove_const, :RUBY_REVISION if defined?(RUBY_REVISION)
    Object.send :const_set,    :RUBY_REVISION, 6

    ua = @fetcher.user_agent

    assert_match %r% revision 6\)%, ua
    assert_match %r%Ruby/#{Regexp.escape RUBY_VERSION}dev%, ua
  ensure
    util_restore_version
  end

  def test_user_agent_revision_missing
    util_save_version

    Object.send :remove_const, :RUBY_PATCHLEVEL
    Object.send :const_set,    :RUBY_PATCHLEVEL, -1
    Object.send :remove_const, :RUBY_REVISION if defined?(RUBY_REVISION)

    ua = @fetcher.user_agent

    assert_match %r%\(#{Regexp.escape RUBY_RELEASE_DATE}\)%, ua
  ensure
    util_restore_version
  end

  def test_yaml_error_on_size
    use_ui @ui do
      self.class.enable_yaml = false
      fetcher = Gem::RemoteFetcher.new nil
      assert_error { fetcher.size }
    end
  end

  def util_stub_connection_for hash
    def @fetcher.connection= conn
      @conn = conn
    end

    def @fetcher.connection_for uri
      @conn
    end

    @fetcher.connection = Conn.new OpenStruct.new(hash)
  end

  def assert_error(exception_class=Exception)
    got_exception = false

    begin
      yield
    rescue exception_class
      got_exception = true
    end

    assert got_exception, "Expected exception conforming to #{exception_class}"
  end

  def assert_data_from_server(data)
    assert_block("Data is not from server") { data =~ /0\.4\.11/ }
  end

  def assert_data_from_proxy(data)
    assert_block("Data is not from proxy") { data =~ /0\.4\.2/ }
  end

  class Conn
    attr_accessor :payload

    def initialize(response)
      @response = response
      self.payload = nil
    end

    def request(req)
      self.payload = req
      @response
    end
  end

  class NilLog < WEBrick::Log
    def log(level, data) #Do nothing
    end
  end

  class << self
    attr_reader :normal_server, :proxy_server
    attr_accessor :enable_zip, :enable_yaml

    def start_servers
      @normal_server ||= start_server(SERVER_PORT, SERVER_DATA)
      @proxy_server  ||= start_server(PROXY_PORT, PROXY_DATA)
      @enable_yaml = true
      @enable_zip = false
    end

    private

    def start_server(port, data)
      Thread.new do
        begin
          null_logger = NilLog.new
          s = WEBrick::HTTPServer.new(
            :Port            => port,
            :DocumentRoot    => nil,
            :Logger          => null_logger,
            :AccessLog       => null_logger
            )
          s.mount_proc("/kill") { |req, res| s.shutdown }
          s.mount_proc("/yaml") { |req, res|
            if @enable_yaml
              res.body = data
              res['Content-Type'] = 'text/plain'
              res['content-length'] = data.size
            else
              res.status = "404"
              res.body = "<h1>NOT FOUND</h1>"
              res['Content-Type'] = 'text/html'
            end
          }
          s.mount_proc("/yaml.Z") { |req, res|
            if @enable_zip
              res.body = Zlib::Deflate.deflate(data)
              res['Content-Type'] = 'text/plain'
            else
              res.status = "404"
              res.body = "<h1>NOT FOUND</h1>"
              res['Content-Type'] = 'text/html'
            end
          }
          s.start
        rescue Exception => ex
          abort ex.message
          puts "ERROR during server thread: #{ex.message}"
        end
      end
      sleep 0.2                 # Give the servers time to startup
    end
  end

  def test_correct_for_windows_path
    path = "/C:/WINDOWS/Temp/gems"
    assert_equal "C:/WINDOWS/Temp/gems", @fetcher.correct_for_windows_path(path)

    path = "/home/skillet"
    assert_equal "/home/skillet", @fetcher.correct_for_windows_path(path)
  end

  def util_save_version
    @orig_RUBY_ENGINE     = RUBY_ENGINE if defined? RUBY_ENGINE
    @orig_RUBY_PATCHLEVEL = RUBY_PATCHLEVEL
    @orig_RUBY_REVISION   = RUBY_REVISION if defined? RUBY_REVISION
  end

  def util_restore_version
    Object.send :remove_const, :RUBY_ENGINE if defined?(RUBY_ENGINE)
    Object.send :const_set,    :RUBY_ENGINE, @orig_RUBY_ENGINE if
      defined?(@orig_RUBY_ENGINE)

    Object.send :remove_const, :RUBY_PATCHLEVEL
    Object.send :const_set,    :RUBY_PATCHLEVEL, @orig_RUBY_PATCHLEVEL

    Object.send :remove_const, :RUBY_REVISION if defined?(RUBY_REVISION)
    Object.send :const_set,    :RUBY_REVISION, @orig_RUBY_REVISION if
      defined?(@orig_RUBY_REVISION)
  end

end

