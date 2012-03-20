# -*- ruby -*-
#--
# Copyright 2006 by Chad Fowler, Rich Kilmer, Jim Weirich and others.
# All rights reserved.
# See LICENSE.txt for permissions.
#++

module Gem
  QUICKLOADER_SUCKAGE = RUBY_VERSION =~ /^1\.9\.1/
  GEM_PRELUDE_SUCKAGE = RUBY_VERSION =~ /^1\.9\.2/
end

if Gem::GEM_PRELUDE_SUCKAGE and defined?(Gem::QuickLoader) then
  Gem::QuickLoader.remove

  $LOADED_FEATURES.delete Gem::QuickLoader.path_to_full_rubygems_library

  if $LOADED_FEATURES.any? do |path| path.end_with? '/rubygems.rb' end then
    # TODO path does not exist here
    raise LoadError, "another rubygems is already loaded from #{path}"
  end

  class << Gem
    remove_method :try_activate if Gem.respond_to?(:try_activate, true)
  end
end

require 'rubygems/defaults'
require 'rbconfig'
require "rubygems/deprecate"

##
# RubyGems is the Ruby standard for publishing and managing third party
# libraries.
#
# For user documentation, see:
#
# * <tt>gem help</tt> and <tt>gem help [command]</tt>
# * {RubyGems User Guide}[http://docs.rubygems.org/read/book/1]
# * {Frequently Asked Questions}[http://docs.rubygems.org/read/book/3]
#
# For gem developer documentation see:
#
# * {Creating Gems}[http://docs.rubygems.org/read/chapter/5]
# * Gem::Specification
# * Gem::Version for version dependency notes
#
# Further RubyGems documentation can be found at:
#
# * {RubyGems API}[http://rubygems.rubyforge.org/rdoc] (also available from
#   <tt>gem server</tt>)
# * {RubyGems Bookshelf}[http://rubygem.org]
#
# == RubyGems Plugins
#
# As of RubyGems 1.3.2, RubyGems will load plugins installed in gems or
# $LOAD_PATH.  Plugins must be named 'rubygems_plugin' (.rb, .so, etc) and
# placed at the root of your gem's #require_path.  Plugins are discovered via
# Gem::find_files then loaded.  Take care when implementing a plugin as your
# plugin file may be loaded multiple times if multiple versions of your gem
# are installed.
#
# For an example plugin, see the graph gem which adds a `gem graph` command.
#
# == RubyGems Defaults, Packaging
#
# RubyGems defaults are stored in rubygems/defaults.rb.  If you're packaging
# RubyGems or implementing Ruby you can change RubyGems' defaults.
#
# For RubyGems packagers, provide lib/rubygems/operating_system.rb and
# override any defaults from lib/rubygems/defaults.rb.
#
# For Ruby implementers, provide lib/rubygems/#{RUBY_ENGINE}.rb and override
# any defaults from lib/rubygems/defaults.rb.
#
# If you need RubyGems to perform extra work on install or uninstall, your
# defaults override file can set pre and post install and uninstall hooks.
# See Gem::pre_install, Gem::pre_uninstall, Gem::post_install,
# Gem::post_uninstall.
#
# == Bugs
#
# You can submit bugs to the
# {RubyGems bug tracker}[http://rubyforge.org/tracker/?atid=575&group_id=126]
# on RubyForge
#
# == Credits
#
# RubyGems is currently maintained by Eric Hodel.
#
# RubyGems was originally developed at RubyConf 2003 by:
#
# * Rich Kilmer  -- rich(at)infoether.com
# * Chad Fowler  -- chad(at)chadfowler.com
# * David Black  -- dblack(at)wobblini.net
# * Paul Brannan -- paul(at)atdesk.com
# * Jim Weirch   -- jim(at)weirichhouse.org
#
# Contributors:
#
# * Gavin Sinclair     -- gsinclair(at)soyabean.com.au
# * George Marrows     -- george.marrows(at)ntlworld.com
# * Dick Davies        -- rasputnik(at)hellooperator.net
# * Mauricio Fernandez -- batsman.geo(at)yahoo.com
# * Simon Strandgaard  -- neoneye(at)adslhome.dk
# * Dave Glasser       -- glasser(at)mit.edu
# * Paul Duncan        -- pabs(at)pablotron.org
# * Ville Aine         -- vaine(at)cs.helsinki.fi
# * Eric Hodel         -- drbrain(at)segment7.net
# * Daniel Berger      -- djberg96(at)gmail.com
# * Phil Hagelberg     -- technomancy(at)gmail.com
# * Ryan Davis         -- ryand-ruby(at)zenspider.com
#
# (If your name is missing, PLEASE let us know!)
#
# Thanks!
#
# -The RubyGems Team

module Gem
  VERSION = '1.8.11'

  ##
  # Raised when RubyGems is unable to load or activate a gem.  Contains the
  # name and version requirements of the gem that either conflicts with
  # already activated gems or that RubyGems is otherwise unable to activate.

  class LoadError < ::LoadError
    # Name of gem
    attr_accessor :name

    # Version requirement of gem
    attr_accessor :requirement
  end

  # :stopdoc:

  RubyGemsVersion = VERSION

  RbConfigPriorities = %w[
    EXEEXT RUBY_SO_NAME arch bindir datadir libdir ruby_install_name
    ruby_version rubylibprefix sitedir sitelibdir vendordir vendorlibdir
  ]

  unless defined?(ConfigMap)
    ##
    # Configuration settings from ::RbConfig
    ConfigMap = Hash.new do |cm, key|
      cm[key] = RbConfig::CONFIG[key.to_s]
    end
  else
    RbConfigPriorities.each do |key|
      ConfigMap[key.to_sym] = RbConfig::CONFIG[key]
    end
  end

  RubyGemsPackageVersion = VERSION

  RUBYGEMS_DIR = File.dirname File.expand_path(__FILE__)

  # :startdoc:

  ##
  # An Array of Regexps that match windows ruby platforms.

  WIN_PATTERNS = [
    /bccwin/i,
    /cygwin/i,
    /djgpp/i,
    /mingw/i,
    /mswin/i,
    /wince/i,
  ]

  @@source_index = nil
  @@win_platform = nil

  @configuration = nil
  @loaded_specs = {}
  @platforms = []
  @ruby = nil
  @sources = nil

  @post_build_hooks     ||= []
  @post_install_hooks   ||= []
  @post_uninstall_hooks ||= []
  @pre_uninstall_hooks  ||= []
  @pre_install_hooks    ||= []
  @pre_reset_hooks      ||= []
  @post_reset_hooks     ||= []

  ##
  # Try to activate a gem containing +path+. Returns true if
  # activation succeeded or wasn't needed because it was already
  # activated. Returns false if it can't find the path in a gem.

  def self.try_activate path
    # TODO: deprecate when 1.9.3 comes out.
    # finds the _latest_ version... regardless of loaded specs and their deps

    # TODO: use find_all and bork if ambiguous

    spec = Gem::Specification.find_by_path path
    return false unless spec

    begin
      spec.activate
    rescue Gem::LoadError # this could fail due to gem dep collisions, go lax
      Gem::Specification.find_by_name(spec.name).activate
    end

    return true
  end

  ##
  # Activates an installed gem matching +dep+.  The gem must satisfy
  # +requirements+.
  #
  # Returns true if the gem is activated, false if it is already
  # loaded, or an exception otherwise.
  #
  # Gem#activate adds the library paths in +dep+ to $LOAD_PATH.  Before a Gem
  # is activated its required Gems are activated.  If the version information
  # is omitted, the highest version Gem of the supplied name is loaded.  If a
  # Gem is not found that meets the version requirements or a required Gem is
  # not found, a Gem::LoadError is raised.
  #
  # More information on version requirements can be found in the
  # Gem::Requirement and Gem::Version documentation.

  def self.activate(dep, *requirements)
    raise ArgumentError, "Deprecated use of Gem.activate(dep)" if
      Gem::Dependency === dep

    Gem::Specification.find_by_name(dep, *requirements).activate
  end

  def self.activate_dep dep, *requirements # :nodoc:
    dep.to_spec.activate
  end

  def self.activate_spec spec # :nodoc:
    spec.activate
  end

  def self.unresolved_deps
    @unresolved_deps ||= Hash.new { |h, n| h[n] = Gem::Dependency.new n }
  end

  ##
  # An Array of all possible load paths for all versions of all gems in the
  # Gem installation.

  def self.all_load_paths
    result = []

    Gem.path.each do |gemdir|
      each_load_path all_partials(gemdir) do |load_path|
        result << gemdir.add(load_path).expand_path
      end
    end

    result
  end

  ##
  # Return all the partial paths in +gemdir+.

  def self.all_partials(gemdir)
    Dir[File.join(gemdir, "gems/*")]
  end

  private_class_method :all_partials

  ##
  # See if a given gem is available.

  def self.available?(dep, *requirements)
    requirements = Gem::Requirement.default if requirements.empty?

    unless dep.respond_to?(:name) and dep.respond_to?(:requirement) then
      dep = Gem::Dependency.new dep, requirements
    end

    not dep.matching_specs(true).empty?
  end

  ##
  # Find the full path to the executable for gem +name+.  If the +exec_name+
  # is not given, the gem's default_executable is chosen, otherwise the
  # specified executable's path is returned.  +requirements+ allows
  # you to specify specific gem versions.

  def self.bin_path(name, exec_name = nil, *requirements)
    # TODO: fails test_self_bin_path_bin_file_gone_in_latest
    # Gem::Specification.find_by_name(name, *requirements).bin_file exec_name

    raise ArgumentError, "you must supply exec_name" unless exec_name

    requirements = Gem::Requirement.default if
      requirements.empty?

    specs = Gem::Dependency.new(name, requirements).matching_specs(true)

    raise Gem::GemNotFoundException,
          "can't find gem #{name} (#{requirements})" if specs.empty?

    specs = specs.find_all { |spec|
      spec.executables.include? exec_name
    } if exec_name

    unless spec = specs.last
      msg = "can't find gem #{name} (#{requirements}) with executable #{exec_name}"
      raise Gem::GemNotFoundException, msg
    end

    spec.bin_file exec_name
  end

  ##
  # The mode needed to read a file as straight binary.

  def self.binary_mode
    'rb'
  end

  ##
  # The path where gem executables are to be installed.

  def self.bindir(install_dir=Gem.dir)
    # TODO: move to Gem::Dirs
    return File.join install_dir, 'bin' unless
      install_dir.to_s == Gem.default_dir.to_s
    Gem.default_bindir
  end

  ##
  # Reset the +dir+ and +path+ values.  The next time +dir+ or +path+
  # is requested, the values will be calculated from scratch.  This is
  # mainly used by the unit tests to provide test isolation.

  def self.clear_paths
    @@source_index = nil
    @paths         = nil
    @user_home     = nil
    @searcher      = nil
    Gem::Specification.reset
  end

  ##
  # The path to standard location of the user's .gemrc file.

  def self.config_file
    @config_file ||= File.join Gem.user_home, '.gemrc'
  end

  ##
  # The standard configuration object for gems.

  def self.configuration
    @configuration ||= Gem::ConfigFile.new []
  end

  ##
  # Use the given configuration object (which implements the ConfigFile
  # protocol) as the standard configuration object.

  def self.configuration=(config)
    @configuration = config
  end

  ##
  # The path the the data directory specified by the gem name.  If the
  # package is not available as a gem, return nil.

  def self.datadir(gem_name)
# TODO: deprecate
    spec = @loaded_specs[gem_name]
    return nil if spec.nil?
    File.join spec.full_gem_path, "data", gem_name
  end

  ##
  # A Zlib::Deflate.deflate wrapper

  def self.deflate(data)
    require 'zlib'
    Zlib::Deflate.deflate data
  end

  def self.paths
    @paths ||= Gem::PathSupport.new
  end

  def self.paths=(env)
    clear_paths
    @paths = Gem::PathSupport.new env
    Gem::Specification.dirs = @paths.path # FIX: home is at end
  end

  ##
  # The path where gems are to be installed.
  #--
  # FIXME deprecate these once everything else has been done -ebh

  def self.dir
    # TODO: raise "no"
    paths.home
  end

  def self.path
    # TODO: raise "no"
    paths.path
  end

  ##
  # Expand each partial gem path with each of the required paths specified
  # in the Gem spec.  Each expanded path is yielded.

  def self.each_load_path(partials)
    partials.each do |gp|
      base = File.basename gp
      specfn = File.join(dir, "specifications", "#{base}.gemspec")
      if File.exists? specfn
        spec = eval(File.read(specfn))
        spec.require_paths.each do |rp|
          yield File.join(gp,rp)
        end
      else
        filename = File.join(gp, 'lib')
        yield(filename) if File.exists? filename
      end
    end
  end

  private_class_method :each_load_path


  ##
  # Quietly ensure the named Gem directory contains all the proper
  # subdirectories.  If we can't create a directory due to a permission
  # problem, then we will silently continue.

  def self.ensure_gem_subdirectories dir = Gem.dir
    require 'fileutils'

    old_umask = File.umask
    File.umask old_umask | 022

    %w[cache doc gems specifications].each do |name|
      subdir = File.join dir, name
      next if File.exist? subdir
      FileUtils.mkdir_p subdir rescue nil # in case of perms issues -- lame
    end
  ensure
    File.umask old_umask
  end

  ##
  # Returns a list of paths matching +glob+ that can be used by a gem to pick
  # up features from other gems.  For example:
  #
  #   Gem.find_files('rdoc/discover').each do |path| load path end
  #
  # if +check_load_path+ is true (the default), then find_files also searches
  # $LOAD_PATH for files as well as gems.
  #
  # Note that find_files will return all files even if they are from different
  # versions of the same gem.

  def self.find_files(glob, check_load_path=true)
    files = []

    if check_load_path
      files = $LOAD_PATH.map { |load_path|
        Dir["#{File.expand_path glob, load_path}#{Gem.suffix_pattern}"]
      }.flatten.select { |file| File.file? file.untaint }
    end

    files.concat Gem::Specification.map { |spec|
      spec.matches_for_glob("#{glob}#{Gem.suffix_pattern}")
    }.flatten

    # $LOAD_PATH might contain duplicate entries or reference
    # the spec dirs directly, so we prune.
    files.uniq! if check_load_path

    return files
  end

  ##
  # Finds the user's home directory.
  #--
  # Some comments from the ruby-talk list regarding finding the home
  # directory:
  #
  #   I have HOME, USERPROFILE and HOMEDRIVE + HOMEPATH. Ruby seems
  #   to be depending on HOME in those code samples. I propose that
  #   it should fallback to USERPROFILE and HOMEDRIVE + HOMEPATH (at
  #   least on Win32).
  #++
  #--
  #
  # FIXME move to pathsupport
  #
  #++

  def self.find_home
    windows = File::ALT_SEPARATOR
    if not windows or RUBY_VERSION >= '1.9' then
      File.expand_path "~"
    else
      ['HOME', 'USERPROFILE'].each do |key|
        return File.expand_path ENV[key] if ENV[key]
      end

      if ENV['HOMEDRIVE'] && ENV['HOMEPATH'] then
        File.expand_path "#{ENV['HOMEDRIVE']}#{ENV['HOMEPATH']}"
      end
    end
  rescue
    if windows then
      File.expand_path File.join(ENV['HOMEDRIVE'] || ENV['SystemDrive'], '/')
    else
      File.expand_path "/"
    end
  end

  private_class_method :find_home

  ##
  # Zlib::GzipReader wrapper that unzips +data+.

  def self.gunzip(data)
    # TODO: move to utils
    require 'stringio'
    require 'zlib'
    data = StringIO.new data

    Zlib::GzipReader.new(data).read
  end

  ##
  # Zlib::GzipWriter wrapper that zips +data+.

  def self.gzip(data)
    # TODO: move to utils
    require 'stringio'
    require 'zlib'
    zipped = StringIO.new

    Zlib::GzipWriter.wrap zipped do |io| io.write data end

    zipped.string
  end

  ##
  # A Zlib::Inflate#inflate wrapper

  def self.inflate(data)
    # TODO: move to utils
    require 'zlib'
    Zlib::Inflate.inflate data
  end

  ##
  # Get the default RubyGems API host. This is normally
  # <tt>https://rubygems.org</tt>.

  def self.host
    # TODO: move to utils
    @host ||= "https://rubygems.org"
  end

  ## Set the default RubyGems API host.

  def self.host= host
    # TODO: move to utils
    @host = host
  end

  ##
  # Return a list of all possible load paths for the latest version for all
  # gems in the Gem installation.

  def self.latest_load_paths
    result = []

    Gem.path.each do |gemdir|
      each_load_path(latest_partials(gemdir)) do |load_path|
        result << load_path
      end
    end

    result
  end

  ##
  # Return only the latest partial paths in the given +gemdir+.

  def self.latest_partials(gemdir)
    latest = {}
    all_partials(gemdir).each do |gp|
      base = File.basename gp

      if base.to_s =~ /(.*)-((\d+\.)*\d+)/ then
        name, version = $1, $2
        ver = Gem::Version.new(version)
        if latest[name].nil? || ver > latest[name][0]
          latest[name] = [ver, gp]
        end
      end
    end
    latest.collect { |k,v| v[1] }
  end

  private_class_method :latest_partials

  ##
  # The index to insert activated gem paths into the $LOAD_PATH.
  #
  # Defaults to the site lib directory unless gem_prelude.rb has loaded paths,
  # then it inserts the activated gem's paths before the gem_prelude.rb paths
  # so you can override the gem_prelude.rb default $LOAD_PATH paths.

  def self.load_path_insert_index
    index = $LOAD_PATH.index ConfigMap[:sitelibdir]

    if QUICKLOADER_SUCKAGE then
      $LOAD_PATH.each_with_index do |path, i|
        if path.instance_variables.include?(:@gem_prelude_index) or
            path.instance_variables.include?('@gem_prelude_index') then
          index = i
          break
        end
      end
    end

    index
  end

  ##
  # Loads YAML, preferring Psych

  def self.load_yaml
    begin
      gem 'psych', '~> 1.2', '>= 1.2.1' unless ENV['TEST_SYCK']
    rescue Gem::LoadError
      # It's OK if the user does not have the psych gem installed.  We will
      # attempt to require the stdlib version
    end

    begin
      # Try requiring the gem version *or* stdlib version of psych.
      require 'psych' unless ENV['TEST_SYCK']
    rescue ::LoadError
    ensure
      require 'yaml'
    end

    # Hack to handle syck's DefaultKey bug with psych.
    # See the note at the top of lib/rubygems/requirement.rb for
    # why we end up defining DefaultKey more than once.
    if !defined? YAML::Syck
      YAML.module_eval do
          const_set 'Syck', Module.new {
            const_set 'DefaultKey', Class.new
          }
        end
    end
  end

  ##
  # The file name and line number of the caller of the caller of this method.

  def self.location_of_caller
    caller[1] =~ /(.*?):(\d+).*?$/i
    file = $1
    lineno = $2.to_i

    # TODO: it is ALWAYS joined! STUPID!
    [file, lineno]
  end

  ##
  # The version of the Marshal format for your Ruby.

  def self.marshal_version
    "#{Marshal::MAJOR_VERSION}.#{Marshal::MINOR_VERSION}"
  end

  ##
  # Get the appropriate cache path.
  #
  # Pass a string to use a different base path, or nil/false (default) for
  # Gem.dir.
  #

  def self.cache_dir(custom_dir=false)
    File.join(custom_dir || Gem.dir, "cache")
  end

  ##
  # Given a gem path, find the gem in cache.
  #
  # Pass a string as the second argument to use a different base path, or
  # nil/false (default) for Gem.dir.

  def self.cache_gem(filename, user_dir=false)
    cache_dir(user_dir).add(filename)
  end

  ##
  # Set array of platforms this RubyGems supports (primarily for testing).

  def self.platforms=(platforms)
    @platforms = platforms
  end

  ##
  # Array of platforms this RubyGems supports.

  def self.platforms
    @platforms ||= []
    if @platforms.empty?
      @platforms = [Gem::Platform::RUBY, Gem::Platform.local]
    end
    @platforms
  end

  ##
  # Adds a post-build hook that will be passed an Gem::Installer instance
  # when Gem::Installer#install is called.  The hook is called after the gem
  # has been extracted and extensions have been built but before the
  # executables or gemspec has been written.  If the hook returns +false+ then
  # the gem's files will be removed and the install will be aborted.

  def self.post_build(&hook)
    @post_build_hooks << hook
  end

  ##
  # Adds a post-install hook that will be passed an Gem::Installer instance
  # when Gem::Installer#install is called

  def self.post_install(&hook)
    @post_install_hooks << hook
  end

  ##
  # Adds a hook that will get run after Gem::Specification.reset is
  # run.

  def self.post_reset(&hook)
    @post_reset_hooks << hook
  end

  ##
  # Adds a post-uninstall hook that will be passed a Gem::Uninstaller instance
  # and the spec that was uninstalled when Gem::Uninstaller#uninstall is
  # called

  def self.post_uninstall(&hook)
    @post_uninstall_hooks << hook
  end

  ##
  # Adds a pre-install hook that will be passed an Gem::Installer instance
  # when Gem::Installer#install is called.  If the hook returns +false+ then
  # the install will be aborted.

  def self.pre_install(&hook)
    @pre_install_hooks << hook
  end

  ##
  # Adds a hook that will get run before Gem::Specification.reset is
  # run.

  def self.pre_reset(&hook)
    @pre_reset_hooks << hook
  end

  ##
  # Adds a pre-uninstall hook that will be passed an Gem::Uninstaller instance
  # and the spec that will be uninstalled when Gem::Uninstaller#uninstall is
  # called

  def self.pre_uninstall(&hook)
    @pre_uninstall_hooks << hook
  end

  ##
  # The directory prefix this RubyGems was installed at. If your
  # prefix is in a standard location (ie, rubygems is installed where
  # you'd expect it to be), then prefix returns nil.

  def self.prefix
    prefix = File.dirname RUBYGEMS_DIR

    if prefix != File.expand_path(ConfigMap[:sitelibdir]) and
       prefix != File.expand_path(ConfigMap[:libdir]) and
       'lib' == File.basename(RUBYGEMS_DIR) then
      prefix
    end
  end

  ##
  # Promotes the load paths of the +gem_name+ over the load paths of
  # +over_name+.  Useful for allowing one gem to override features in another
  # using #find_files.

  def self.promote_load_path(gem_name, over_name)
    gem = Gem.loaded_specs[gem_name]
    over = Gem.loaded_specs[over_name]

    raise ArgumentError, "gem #{gem_name} is not activated" if gem.nil?
    raise ArgumentError, "gem #{over_name} is not activated" if over.nil?

    last_gem_path = Gem::Path.path(gem.full_gem_path).add(gem.require_paths.last)

    over_paths = over.require_paths.map do |path|
      Gem::Path.path(over.full_gem_path).add(path).to_s
    end

    over_paths.each do |path|
      $LOAD_PATH.delete path
    end

    gem = $LOAD_PATH.index(last_gem_path) + 1

    $LOAD_PATH.insert(gem, *over_paths)
  end

  ##
  # Refresh source_index from disk and clear searcher.

  def self.refresh
    Gem::Specification.reset
    @source_index = nil
    @searcher = nil
  end

  ##
  # Safely read a file in binary mode on all platforms.

  def self.read_binary(path)
    File.open path, binary_mode do |f| f.read end
  end

  ##
  # Report a load error during activation.  The message of load error
  # depends on whether it was a version mismatch or if there are not gems of
  # any version by the requested name.

  def self.report_activate_error(gem)
    matches = Gem::Specification.find_by_name(gem.name)

    if matches.empty? then
      error = Gem::LoadError.new(
          "Could not find RubyGem #{gem.name} (#{gem.requirement})\n")
    else
      error = Gem::LoadError.new(
          "RubyGem version error: " +
          "#{gem.name}(#{matches.first.version} not #{gem.requirement})\n")
    end

    error.name = gem.name
    error.requirement = gem.requirement
    raise error
  end

  private_class_method :report_activate_error

  ##
  # Full path to +libfile+ in +gemname+.  Searches for the latest gem unless
  # +requirements+ is given.

  def self.required_location(gemname, libfile, *requirements)
    requirements = Gem::Requirement.default if requirements.empty?

    matches = Gem::Specification.find_all_by_name gemname, *requirements

    return nil if matches.empty?

    spec = matches.last
    spec.require_paths.each do |path|
      result = Gem::Path.path(spec.full_gem_path).add(path, libfile)
      return result if result.exist?
    end

    nil
  end

  ##
  # The path to the running Ruby interpreter.

  def self.ruby
    if @ruby.nil? then
      @ruby = File.join(ConfigMap[:bindir],
                        "#{ConfigMap[:ruby_install_name]}#{ConfigMap[:EXEEXT]}")

      @ruby = "\"#{@ruby}\"" if @ruby =~ /\s/
    end

    @ruby
  end

  def self.latest_spec_for name
    dependency  = Gem::Dependency.new name
    fetcher     = Gem::SpecFetcher.fetcher
    spec_tuples = fetcher.find_matching dependency

    match = spec_tuples.select { |(n, _, p), _|
      n == name and Gem::Platform.match p
    }.sort_by { |(_, version, _), _|
      version
    }.last

    match and fetcher.fetch_spec(*match)
  end

  def self.latest_version_for name
    spec = latest_spec_for name
    spec and spec.version
  end

  def self.latest_rubygems_version
    latest_version_for "rubygems-update"
  end

  ##
  # A Gem::Version for the currently running ruby.

  def self.ruby_version
    return @ruby_version if defined? @ruby_version
    version = RUBY_VERSION.dup

    if defined?(RUBY_PATCHLEVEL) && RUBY_PATCHLEVEL != -1 then
      version << ".#{RUBY_PATCHLEVEL}"
    elsif defined?(RUBY_REVISION) then
      version << ".dev.#{RUBY_REVISION}"
    end

    @ruby_version = Gem::Version.new version
  end

  ##
  # The GemPathSearcher object used to search for matching installed gems.

  def self.searcher
    @searcher ||= Gem::GemPathSearcher.new
  end

  ##
  # Returns the Gem::SourceIndex of specifications that are in the Gem.path

  def self.source_index
    @@source_index ||= Gem::Deprecate.skip_during do
      SourceIndex.new Gem::Specification.dirs
    end
  end

  ##
  # Returns an Array of sources to fetch remote gems from.  If the sources
  # list is empty, attempts to load the "sources" gem, then uses
  # default_sources if it is not installed.

  def self.sources
    @sources ||= default_sources
  end

  ##
  # Need to be able to set the sources without calling
  # Gem.sources.replace since that would cause an infinite loop.

  def self.sources= new_sources
    @sources = new_sources
  end

  ##
  # Glob pattern for require-able path suffixes.

  def self.suffix_pattern
    @suffix_pattern ||= "{#{suffixes.join(',')}}"
  end

  def self.loaded_path? path
    # TODO: ruby needs a feature to let us query what's loaded in 1.8 and 1.9
    $LOADED_FEATURES.find { |s|
      s =~ /(^|\/)#{Regexp.escape path}#{Regexp.union(*Gem.suffixes)}$/
    }
  end

  ##
  # Suffixes for require-able paths.

  def self.suffixes
    @suffixes ||= ['',
                   '.rb',
                   *%w(DLEXT DLEXT2).map { |key|
                     val = RbConfig::CONFIG[key]
                     next unless val and not val.empty?
                     ".#{val}"
                   }
                  ].compact.uniq
  end

  ##
  # Prints the amount of time the supplied block takes to run using the debug
  # UI output.

  def self.time(msg, width = 0, display = Gem.configuration.verbose)
    now = Time.now

    value = yield

    elapsed = Time.now - now

    ui.say "%2$*1$s: %3$3.3fs" % [-width, msg, elapsed] if display

    value
  end

  ##
  # Lazily loads DefaultUserInteraction and returns the default UI.

  def self.ui
    require 'rubygems/user_interaction'

    Gem::DefaultUserInteraction.ui
  end

  ##
  # Use the +home+ and +paths+ values for Gem.dir and Gem.path.  Used mainly
  # by the unit tests to provide environment isolation.

  def self.use_paths(home, *paths)
    paths = nil if paths == [nil]
    paths = paths.first if Array === Array(paths).first
    self.paths = { "GEM_HOME" => home, "GEM_PATH" => paths }
    # TODO: self.paths = home, paths
  end

  ##
  # The home directory for the user.

  def self.user_home
    @user_home ||= find_home
  end

  ##
  # Is this a windows platform?

  def self.win_platform?
    if @@win_platform.nil? then
      @@win_platform = !!WIN_PATTERNS.find { |r| RUBY_PLATFORM =~ r }
    end

    @@win_platform
  end

  ##
  # Load +plugins+ as ruby files

  def self.load_plugin_files(plugins)
    plugins.each do |plugin|

      # Skip older versions of the GemCutter plugin: Its commands are in
      # RubyGems proper now.

      next if plugin =~ /gemcutter-0\.[0-3]/

      begin
        load plugin
      rescue ::Exception => e
        details = "#{plugin.inspect}: #{e.message} (#{e.class})"
        warn "Error loading RubyGems plugin #{details}"
      end
    end
  end

  ##
  # Find all 'rubygems_plugin' files in installed gems and load them

  def self.load_plugins
    load_plugin_files find_files('rubygems_plugin', false)
  end

  ##
  # Find all 'rubygems_plugin' files in $LOAD_PATH and load them

  def self.load_env_plugins
    path = "rubygems_plugin"

    files = []
    $LOAD_PATH.each do |load_path|
      globbed = Dir["#{File.expand_path path, load_path}#{Gem.suffix_pattern}"]

      globbed.each do |load_path_file|
        files << load_path_file if File.file?(load_path_file.untaint)
      end
    end

    load_plugin_files files
  end

  class << self

    ##
    # Hash of loaded Gem::Specification keyed by name

    attr_reader :loaded_specs

    ##
    # The list of hooks to be run before Gem::Install#install finishes
    # installation

    attr_reader :post_build_hooks

    ##
    # The list of hooks to be run before Gem::Install#install does any work

    attr_reader :post_install_hooks

    ##
    # The list of hooks to be run after Gem::Specification.reset is run.

    attr_reader :post_reset_hooks

    ##
    # The list of hooks to be run before Gem::Uninstall#uninstall does any
    # work

    attr_reader :post_uninstall_hooks

    ##
    # The list of hooks to be run after Gem::Install#install is finished

    attr_reader :pre_install_hooks

    ##
    # The list of hooks to be run before Gem::Specification.reset is run.

    attr_reader :pre_reset_hooks

    ##
    # The list of hooks to be run after Gem::Uninstall#uninstall is finished

    attr_reader :pre_uninstall_hooks
  end

  def self.cache # :nodoc:
    source_index
  end

  ##
  # Location of Marshal quick gemspecs on remote repositories

  MARSHAL_SPEC_DIR = "quick/Marshal.#{Gem.marshal_version}/"

  autoload :Version,         'rubygems/version'
  autoload :Requirement,     'rubygems/requirement'
  autoload :Dependency,      'rubygems/dependency'
  autoload :DependencyList,  'rubygems/dependency_list'
  autoload :GemPathSearcher, 'rubygems/gem_path_searcher'
  autoload :SpecFetcher,     'rubygems/spec_fetcher'
  autoload :Specification,   'rubygems/specification'
  autoload :Cache,           'rubygems/source_index'
  autoload :SourceIndex,     'rubygems/source_index'
  autoload :PathSupport,     'rubygems/path_support'
  autoload :Platform,        'rubygems/platform'
  autoload :Builder,         'rubygems/builder'
  autoload :ConfigFile,      'rubygems/config_file'
end

module Kernel

  remove_method :gem if 'method' == defined? gem # from gem_prelude.rb on 1.9

  ##
  # Use Kernel#gem to activate a specific version of +gem_name+.
  #
  # +requirements+ is a list of version requirements that the
  # specified gem must match, most commonly "= example.version.number".  See
  # Gem::Requirement for how to specify a version requirement.
  #
  # If you will be activating the latest version of a gem, there is no need to
  # call Kernel#gem, Kernel#require will do the right thing for you.
  #
  # Kernel#gem returns true if the gem was activated, otherwise false.  If the
  # gem could not be found, didn't match the version requirements, or a
  # different version was already activated, an exception will be raised.
  #
  # Kernel#gem should be called *before* any require statements (otherwise
  # RubyGems may load a conflicting library version).
  #
  # In older RubyGems versions, the environment variable GEM_SKIP could be
  # used to skip activation of specified gems, for example to test out changes
  # that haven't been installed yet.  Now RubyGems defers to -I and the
  # RUBYLIB environment variable to skip activation of a gem.
  #
  # Example:
  #
  #   GEM_SKIP=libA:libB ruby -I../libA -I../libB ./mycode.rb

  def gem(gem_name, *requirements) # :doc:
    skip_list = (ENV['GEM_SKIP'] || "").split(/:/)
    raise Gem::LoadError, "skipping #{gem_name}" if skip_list.include? gem_name
    spec = Gem::Dependency.new(gem_name, *requirements).to_spec
    spec.activate if spec
  end

  private :gem

end

##
# Return the path to the data directory associated with the named package.  If
# the package is loaded as a gem, return the gem specific data directory.
# Otherwise return a path to the share area as define by
# "#{ConfigMap[:datadir]}/#{package_name}".

def RbConfig.datadir(package_name) # :nodoc:
  warn "#{Gem.location_of_caller.join ':'}:Warning: " \
    "RbConfig.datadir is deprecated and will be removed on or after " \
    "August 2011.  " \
    "Use Gem::datadir."

  require 'rbconfig/datadir'

  Gem.datadir(package_name) || File.join(Gem::ConfigMap[:datadir], package_name)
end

require 'rubygems/exceptions'

gem_preluded = Gem::GEM_PRELUDE_SUCKAGE and defined? Gem
unless gem_preluded then # TODO: remove guard after 1.9.2 dropped
  begin
    ##
    # Defaults the operating system (or packager) wants to provide for RubyGems.

    require 'rubygems/defaults/operating_system'
  rescue LoadError
  end

  if defined?(RUBY_ENGINE) then
    begin
      ##
      # Defaults the ruby implementation wants to provide for RubyGems

      require "rubygems/defaults/#{RUBY_ENGINE}"
    rescue LoadError
    end
  end
end

##
# Enables the require hook for RubyGems.

require 'rubygems/custom_require'

module Gem
  class << self
    extend Gem::Deprecate
    deprecate :activate_dep,          "Specification#activate", 2011,  6
    deprecate :activate_spec,         "Specification#activate", 2011,  6
    deprecate :cache,                 "Gem::source_index",      2011,  8
    deprecate :activate,              "Specification#activate", 2011, 10
    deprecate :all_load_paths,        :none,                    2011, 10
    deprecate :all_partials,          :none,                    2011, 10
    deprecate :latest_load_paths,     :none,                    2011, 10
    deprecate :promote_load_path,     :none,                    2011, 10
    deprecate :available?,       "Specification::find_by_name", 2011, 11
    deprecate :cache_dir,           "Specification#cache_dir",  2011, 11
    deprecate :cache_gem,           "Specification#cache_file", 2011, 11
    deprecate :default_system_source_cache_dir, :none,          2011, 11
    deprecate :default_user_source_cache_dir,   :none,          2011, 11
    deprecate :report_activate_error, :none,                    2011, 11
    deprecate :required_location,     :none,                    2011, 11
    deprecate :searcher,              "Specification",          2011, 11
    deprecate :source_index,          "Specification",          2011, 11
  end
end
