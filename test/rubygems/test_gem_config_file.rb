require 'rubygems/test_case'
require 'rubygems/config_file'

class TestGemConfigFile < Gem::TestCase

  def setup
    super

    @temp_conf = File.join @tempdir, '.gemrc'

    @cfg_args = %W[--config-file #{@temp_conf}]

    @orig_SYSTEM_WIDE_CONFIG_FILE = Gem::ConfigFile::SYSTEM_WIDE_CONFIG_FILE
    Gem::ConfigFile.send :remove_const, :SYSTEM_WIDE_CONFIG_FILE
    Gem::ConfigFile.send :const_set, :SYSTEM_WIDE_CONFIG_FILE,
                         File.join(@tempdir, 'system-gemrc')
    Gem::ConfigFile::OPERATING_SYSTEM_DEFAULTS.clear
    Gem::ConfigFile::PLATFORM_DEFAULTS.clear

    util_config_file
  end

  def teardown
    Gem::ConfigFile::OPERATING_SYSTEM_DEFAULTS.clear
    Gem::ConfigFile::PLATFORM_DEFAULTS.clear
    Gem::ConfigFile.send :remove_const, :SYSTEM_WIDE_CONFIG_FILE
    Gem::ConfigFile.send :const_set, :SYSTEM_WIDE_CONFIG_FILE,
                         @orig_SYSTEM_WIDE_CONFIG_FILE

    super
  end

  def test_initialize
    assert_equal @temp_conf, @cfg.config_file_name

    assert_equal false, @cfg.backtrace
    assert_equal true, @cfg.update_sources
    assert_equal false, @cfg.benchmark
    assert_equal Gem::ConfigFile::DEFAULT_BULK_THRESHOLD, @cfg.bulk_threshold
    assert_equal true, @cfg.verbose
    assert_equal [@gem_repo], Gem.sources

    File.open @temp_conf, 'w' do |fp|
      fp.puts ":backtrace: true"
      fp.puts ":update_sources: false"
      fp.puts ":benchmark: true"
      fp.puts ":bulk_threshold: 10"
      fp.puts ":verbose: false"
      fp.puts ":sources:"
      fp.puts "  - http://more-gems.example.com"
      fp.puts "install: --wrappers"
      fp.puts ":gempath:"
      fp.puts "- /usr/ruby/1.8/lib/ruby/gems/1.8"
      fp.puts "- /var/ruby/1.8/gem_home"
    end

    util_config_file

    assert_equal true, @cfg.backtrace
    assert_equal true, @cfg.benchmark
    assert_equal 10, @cfg.bulk_threshold
    assert_equal false, @cfg.verbose
    assert_equal false, @cfg.update_sources
    assert_equal %w[http://more-gems.example.com], Gem.sources
    assert_equal '--wrappers', @cfg[:install]
    assert_equal(['/usr/ruby/1.8/lib/ruby/gems/1.8', '/var/ruby/1.8/gem_home'],
                 @cfg.path)
  end

  def test_initialize_handle_arguments_config_file
    util_config_file %W[--config-file #{@temp_conf}]

    assert_equal @temp_conf, @cfg.config_file_name
  end

  def test_initialize_handle_arguments_config_file_with_other_params
    util_config_file %W[--config-file #{@temp_conf} --backtrace]

    assert_equal @temp_conf, @cfg.config_file_name
  end

  def test_initialize_handle_arguments_config_file_equals
    util_config_file %W[--config-file=#{@temp_conf}]

    assert_equal @temp_conf, @cfg.config_file_name
  end

  def test_initialize_operating_system_override
    Gem::ConfigFile::OPERATING_SYSTEM_DEFAULTS[:bulk_threshold] = 1
    Gem::ConfigFile::OPERATING_SYSTEM_DEFAULTS['install'] = '--no-env-shebang'

    Gem::ConfigFile::PLATFORM_DEFAULTS[:bulk_threshold] = 2

    util_config_file

    assert_equal 2, @cfg.bulk_threshold
    assert_equal '--no-env-shebang', @cfg[:install]
  end

  def test_initialize_platform_override
    Gem::ConfigFile::PLATFORM_DEFAULTS[:bulk_threshold] = 2
    Gem::ConfigFile::PLATFORM_DEFAULTS['install'] = '--no-env-shebang'

    File.open Gem::ConfigFile::SYSTEM_WIDE_CONFIG_FILE, 'w' do |fp|
      fp.puts ":bulk_threshold: 3"
    end

    util_config_file

    assert_equal 3, @cfg.bulk_threshold
    assert_equal '--no-env-shebang', @cfg[:install]
  end

  def test_initialize_system_wide_override
    File.open Gem::ConfigFile::SYSTEM_WIDE_CONFIG_FILE, 'w' do |fp|
      fp.puts ":backtrace: false"
      fp.puts ":bulk_threshold: 2048"
    end

    File.open @temp_conf, 'w' do |fp|
      fp.puts ":backtrace: true"
    end

    util_config_file

    assert_equal 2048, @cfg.bulk_threshold
    assert_equal true, @cfg.backtrace
  end

  def test_handle_arguments
    args = %w[--backtrace --bunch --of --args here]

    @cfg.handle_arguments args

    assert_equal %w[--bunch --of --args here], @cfg.args
  end

  def test_handle_arguments_backtrace
    assert_equal false, @cfg.backtrace

    args = %w[--backtrace]

    @cfg.handle_arguments args

    assert_equal true, @cfg.backtrace
  end

  def test_handle_arguments_benchmark
    assert_equal false, @cfg.benchmark

    args = %w[--benchmark]

    @cfg.handle_arguments args

    assert_equal true, @cfg.benchmark
  end

  def test_handle_arguments_debug
    old_dollar_DEBUG = $DEBUG
    assert_equal false, $DEBUG

    args = %w[--debug]

    @cfg.handle_arguments args

    assert_equal true, $DEBUG
  ensure
    $DEBUG = old_dollar_DEBUG
  end

  def test_handle_arguments_override
    File.open @temp_conf, 'w' do |fp|
      fp.puts ":benchmark: false"
    end

    util_config_file %W[--benchmark --config-file=#{@temp_conf}]

    assert_equal true, @cfg.benchmark
  end

  def test_handle_arguments_traceback
    assert_equal false, @cfg.backtrace

    args = %w[--traceback]

    @cfg.handle_arguments args

    assert_equal true, @cfg.backtrace
  end

  def test_really_verbose
    assert_equal false, @cfg.really_verbose

    @cfg.verbose = true

    assert_equal false, @cfg.really_verbose

    @cfg.verbose = 1

    assert_equal true, @cfg.really_verbose
  end

  def test_write
    @cfg.backtrace = true
    @cfg.benchmark = true
    @cfg.update_sources = false
    @cfg.bulk_threshold = 10
    @cfg.verbose = false
    Gem.sources.replace %w[http://more-gems.example.com]
    @cfg[:install] = '--wrappers'

    @cfg.write

    util_config_file

    # These should not be written out to the config file.
    assert_equal false, @cfg.backtrace,     'backtrace'
    assert_equal false, @cfg.benchmark,     'benchmark'
    assert_equal Gem::ConfigFile::DEFAULT_BULK_THRESHOLD, @cfg.bulk_threshold,
                 'bulk_threshold'
    assert_equal true, @cfg.update_sources, 'update_sources'
    assert_equal true, @cfg.verbose,        'verbose'

    assert_equal '--wrappers', @cfg[:install], 'install'

    # this should be written out to the config file.
    assert_equal %w[http://more-gems.example.com], Gem.sources
  end

  def test_write_from_hash
    File.open @temp_conf, 'w' do |fp|
      fp.puts ":backtrace: true"
      fp.puts ":benchmark: true"
      fp.puts ":bulk_threshold: 10"
      fp.puts ":update_sources: false"
      fp.puts ":verbose: false"
      fp.puts ":sources:"
      fp.puts "  - http://more-gems.example.com"
      fp.puts "install: --wrappers"
    end

    util_config_file

    @cfg.backtrace = :junk
    @cfg.benchmark = :junk
    @cfg.update_sources = :junk
    @cfg.bulk_threshold = 20
    @cfg.verbose = :junk
    Gem.sources.replace %w[http://even-more-gems.example.com]
    @cfg[:install] = '--wrappers --no-rdoc'

    @cfg.write

    util_config_file

    # These should not be written out to the config file
    assert_equal true,  @cfg.backtrace,      'backtrace'
    assert_equal true,  @cfg.benchmark,      'benchmark'
    assert_equal 10,    @cfg.bulk_threshold, 'bulk_threshold'
    assert_equal false, @cfg.update_sources, 'update_sources'
    assert_equal false, @cfg.verbose,        'verbose'

    assert_equal '--wrappers --no-rdoc', @cfg[:install], 'install'

    assert_equal %w[http://even-more-gems.example.com], Gem.sources
  end

  def test_load_rubygems_api_key_from_credentials
    temp_cred = File.join Gem.user_home, '.gem', 'credentials'
    FileUtils.mkdir File.dirname(temp_cred)
    File.open temp_cred, 'w' do |fp|
      fp.puts ":rubygems_api_key: 701229f217cdf23b1344c7b4b54ca97"
    end

    util_config_file

    assert_equal "701229f217cdf23b1344c7b4b54ca97", @cfg.rubygems_api_key
  end

  def test_load_api_keys_from_config
    temp_cred = File.join Gem.user_home, '.gem', 'credentials'
    FileUtils.mkdir File.dirname(temp_cred)
    File.open temp_cred, 'w' do |fp|
      fp.puts ":rubygems_api_key: 701229f217cdf23b1344c7b4b54ca97"
      fp.puts ":other: a5fdbb6ba150cbb83aad2bb2fede64c"
    end

    util_config_file

    assert_equal({:rubygems => '701229f217cdf23b1344c7b4b54ca97',
                  :other => 'a5fdbb6ba150cbb83aad2bb2fede64c'}, @cfg.api_keys)
  end

  def util_config_file(args = @cfg_args)
    @cfg = Gem::ConfigFile.new args
  end

end

