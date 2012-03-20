#--
# Copyright 2006 by Chad Fowler, Rich Kilmer, Jim Weirich and others.
# All rights reserved.
# See LICENSE.txt for permissions.
#++

require 'rubygems/format'
require 'rubygems/exceptions'
require 'rubygems/ext'
require 'rubygems/require_paths_builder'
require 'rubygems/user_interaction'

##
# The installer class processes RubyGem .gem files and installs the files
# contained in the .gem into the Gem.path.
#
# Gem::Installer does the work of putting files in all the right places on the
# filesystem including unpacking the gem into its gem dir, installing the
# gemspec in the specifications dir, storing the cached gem in the cache dir,
# and installing either wrappers or symlinks for executables.
#
# The installer invokes pre and post install hooks.  Hooks can be added either
# through a rubygems_plugin.rb file in an installed gem or via a
# rubygems/defaults/#{RUBY_ENGINE}.rb or rubygems/defaults/operating_system.rb
# file.  See Gem.pre_install and Gem.post_install for details.

class Gem::Installer

  ##
  # Paths where env(1) might live.  Some systems are broken and have it in
  # /bin

  ENV_PATHS = %w[/usr/bin/env /bin/env]

  ##
  # Raised when there is an error while building extensions.
  #
  class ExtensionBuildError < Gem::InstallError; end

  include Gem::UserInteraction

  include Gem::RequirePathsBuilder if Gem::QUICKLOADER_SUCKAGE

  attr_reader :gem

  ##
  # The directory a gem's executables will be installed into

  attr_reader :bin_dir

  ##
  # The gem repository the gem will be installed into

  attr_reader :gem_home

  ##
  # The options passed when the Gem::Installer was instantiated.

  attr_reader :options

  @path_warning = false

  class << self

    ##
    # True if we've warned about PATH not including Gem.bindir

    attr_accessor :path_warning

    attr_writer :exec_format

    # Defaults to use Ruby's program prefix and suffix.
    def exec_format
      @exec_format ||= Gem.default_exec_format
    end

  end

  ##
  # Constructs an Installer instance that will install the gem located at
  # +gem+.  +options+ is a Hash with the following keys:
  #
  # :env_shebang:: Use /usr/bin/env in bin wrappers.
  # :force:: Overrides all version checks and security policy checks, except
  #          for a signed-gems-only policy.
  # :ignore_dependencies:: Don't raise if a dependency is missing.
  # :install_dir:: The directory to install the gem into.
  # :format_executable:: Format the executable the same as the ruby executable.
  #                      If your ruby is ruby18, foo_exec will be installed as
  #                      foo_exec18.
  # :security_policy:: Use the specified security policy.  See Gem::Security
  # :wrappers:: Install wrappers if true, symlinks if false.

  def initialize(gem, options={})
    require 'fileutils'

    @gem = gem
    @options = options
    process_options

    if options[:user_install] and not options[:unpack] then
      @gem_home = Gem.user_dir
      check_that_user_bin_dir_is_in_path
    end
  end

  ##
  # Lazy accessor for the spec's gem directory.

  def gem_dir
    @gem_dir ||= spec.gem_dir.dup.untaint
  end

  ##
  # Lazy accessor for the installer's Gem::Format instance.

  def format
    begin
      @format ||= Gem::Format.from_file_by_path gem, @security_policy
    rescue Gem::Package::FormatError
      raise Gem::InstallError, "invalid gem format for #{gem}"
    end
  end

  ##
  # Lazy accessor for the installer's spec.

  def spec
    @spec ||= format.spec
  end

  ##
  # Installs the gem and returns a loaded Gem::Specification for the installed
  # gem.
  #
  # The gem will be installed with the following structure:
  #
  #   @gem_home/
  #     cache/<gem-version>.gem #=> a cached copy of the installed gem
  #     gems/<gem-version>/... #=> extracted files
  #     specifications/<gem-version>.gemspec #=> the Gem::Specification

  def install
    current_home = Gem.dir
    current_path = Gem.paths.path

    verify_gem_home(options[:unpack])
    Gem.use_paths gem_home, current_path # HACK: shouldn't need Gem.paths.path

    # If we're forcing the install then disable security unless the security
    # policy says that we only install signed gems.
    @security_policy = nil if @force and @security_policy and
                              not @security_policy.only_signed

    unless @force
      ensure_required_ruby_version_met
      ensure_required_rubygems_version_met
      ensure_dependencies_met unless @ignore_dependencies
    end

    Gem.pre_install_hooks.each do |hook|
      result = hook.call self

      if result == false then
        location = " at #{$1}" if hook.inspect =~ /@(.*:\d+)/

        message = "pre-install hook#{location} failed for #{spec.full_name}"
        raise Gem::InstallError, message
      end
    end

    Gem.ensure_gem_subdirectories gem_home

    # Completely remove any previous gem files
    FileUtils.rm_rf(gem_dir) if File.exist? gem_dir

    FileUtils.mkdir_p gem_dir

    extract_files
    build_extensions

    Gem.post_build_hooks.each do |hook|
      result = hook.call self

      if result == false then
        FileUtils.rm_rf gem_dir

        location = " at #{$1}" if hook.inspect =~ /@(.*:\d+)/

        message = "post-build hook#{location} failed for #{spec.full_name}"
        raise Gem::InstallError, message
      end
    end

    generate_bin
    write_spec

    write_require_paths_file_if_needed if Gem::QUICKLOADER_SUCKAGE

    cache_file = spec.cache_file
    FileUtils.cp gem, cache_file unless File.exist? cache_file

    say spec.post_install_message unless spec.post_install_message.nil?

    spec.loaded_from = spec.spec_file

    Gem::Specification.add_spec spec unless Gem::Specification.include? spec

    Gem.post_install_hooks.each do |hook|
      hook.call self
    end

    return spec
  rescue Zlib::GzipFile::Error
    raise Gem::InstallError, "gzip error installing #{gem}"
  ensure
    # conditional since we might be here because we're erroring out early.
    if current_path
      Gem.use_paths current_home, current_path
    end
  end

  ##
  # Ensure that the dependency is satisfied by the current installation of
  # gem.  If it is not an exception is raised.
  #
  # spec       :: Gem::Specification
  # dependency :: Gem::Dependency

  def ensure_dependency(spec, dependency)
    unless installation_satisfies_dependency? dependency then
      raise Gem::InstallError, "#{spec.name} requires #{dependency}"
    end
    true
  end

  ##
  # True if the gems in the source_index satisfy +dependency+.

  def installation_satisfies_dependency?(dependency)
    not dependency.matching_specs.empty?
  end

  ##
  # Unpacks the gem into the given directory.

  def unpack(directory)
    @gem_dir = directory
    @format = Gem::Format.from_file_by_path gem, @security_policy
    extract_files
  end

  ##
  # Writes the .gemspec specification (in Ruby) to the gem home's
  # specifications directory.

  def write_spec
    file_name = spec.spec_file.untaint

    File.open(file_name, "w") do |file|
      file.puts spec.to_ruby_for_cache
    end
  end

  ##
  # Creates windows .bat files for easy running of commands

  def generate_windows_script(filename, bindir)
    if Gem.win_platform? then
      script_name = filename + ".bat"
      script_path = File.join bindir, File.basename(script_name)
      File.open script_path, 'w' do |file|
        file.puts windows_stub_script(bindir, filename)
      end

      say script_path if Gem.configuration.really_verbose
    end
  end

  def generate_bin
    return if spec.executables.nil? or spec.executables.empty?

    # If the user has asked for the gem to be installed in a directory that is
    # the system gem directory, then use the system bin directory, else create
    # (or use) a new bin dir under the gem_home.
    bindir = @bin_dir || Gem.bindir(gem_home)

    Dir.mkdir bindir unless File.exist? bindir
    raise Gem::FilePermissionError.new(bindir) unless File.writable? bindir

    spec.executables.each do |filename|
      filename.untaint
      bin_path = File.expand_path File.join(gem_dir, spec.bindir, filename)

      unless File.exist? bin_path
        warn "Hey?!?! Where did #{bin_path} go??"
        next
      end

      mode = File.stat(bin_path).mode | 0111
      FileUtils.chmod mode, bin_path

      if @wrappers then
        generate_bin_script filename, bindir
      else
        generate_bin_symlink filename, bindir
      end
    end
  end

  ##
  # Creates the scripts to run the applications in the gem.
  #--
  # The Windows script is generated in addition to the regular one due to a
  # bug or misfeature in the Windows shell's pipe.  See
  # http://blade.nagaokaut.ac.jp/cgi-bin/scat.rb/ruby/ruby-talk/193379

  def generate_bin_script(filename, bindir)
    bin_script_path = File.join bindir, formatted_program_filename(filename)

    FileUtils.rm_f bin_script_path # prior install may have been --no-wrappers

    File.open bin_script_path, 'wb', 0755 do |file|
      file.print app_script_text(filename)
    end

    say bin_script_path if Gem.configuration.really_verbose

    generate_windows_script filename, bindir
  end

  ##
  # Creates the symlinks to run the applications in the gem.  Moves
  # the symlink if the gem being installed has a newer version.

  def generate_bin_symlink(filename, bindir)
    if Gem.win_platform? then
      alert_warning "Unable to use symlinks on Windows, installing wrapper"
      generate_bin_script filename, bindir
      return
    end

    src = File.join gem_dir, spec.bindir, filename
    dst = File.join bindir, formatted_program_filename(filename)

    if File.exist? dst then
      if File.symlink? dst then
        link = File.readlink(dst).split File::SEPARATOR
        cur_version = Gem::Version.create(link[-3].sub(/^.*-/, ''))
        return if spec.version < cur_version
      end
      File.unlink dst
    end

    FileUtils.symlink src, dst, :verbose => Gem.configuration.really_verbose
  end

  ##
  # Generates a #! line for +bin_file_name+'s wrapper copying arguments if
  # necessary.

  def shebang(bin_file_name)
    ruby_name = Gem::ConfigMap[:ruby_install_name] if @env_shebang
    path = spec.bin_file bin_file_name
    first_line = File.open(path, "rb") {|file| file.gets}

    if /\A#!/ =~ first_line then
      # Preserve extra words on shebang line, like "-w".  Thanks RPA.
      shebang = first_line.sub(/\A\#!.*?ruby\S*(?=(\s+\S+))/, "#!#{Gem.ruby}")
      opts = $1
      shebang.strip! # Avoid nasty ^M issues.
    end

    if not ruby_name then
      "#!#{Gem.ruby}#{opts}"
    elsif opts then
      "#!/bin/sh\n'exec' #{ruby_name.dump} '-x' \"$0\" \"$@\"\n#{shebang}"
    else
      # Create a plain shebang line.
      @env_path ||= ENV_PATHS.find {|env_path| File.executable? env_path }
      "#!#{@env_path} #{ruby_name}"
    end
  end

  def ensure_required_ruby_version_met
    if rrv = spec.required_ruby_version then
      unless rrv.satisfied_by? Gem.ruby_version then
        raise Gem::InstallError, "#{spec.name} requires Ruby version #{rrv}."
      end
    end
  end

  def ensure_required_rubygems_version_met
    if rrgv = spec.required_rubygems_version then
      unless rrgv.satisfied_by? Gem::Version.new(Gem::VERSION) then
        raise Gem::InstallError,
          "#{spec.name} requires RubyGems version #{rrgv}. " +
          "Try 'gem update --system' to update RubyGems itself."
      end
    end
  end

  def ensure_dependencies_met
    deps = spec.runtime_dependencies
    deps |= spec.development_dependencies if @development

    deps.each do |dep_gem|
      ensure_dependency spec, dep_gem
    end
  end

  def process_options
    @options = {
      :bin_dir      => nil,
      :env_shebang  => false,
      :exec_format  => false,
      :force        => false,
      :install_dir  => Gem.dir,
    }.merge options

    @env_shebang         = options[:env_shebang]
    @force               = options[:force]
    @gem_home            = options[:install_dir]
    @ignore_dependencies = options[:ignore_dependencies]
    @format_executable   = options[:format_executable]
    @security_policy     = options[:security_policy]
    @wrappers            = options[:wrappers]
    @bin_dir             = options[:bin_dir]
    @development         = options[:development]

    raise "NOTE: Installer option :source_index is dead" if
      options[:source_index]
  end

  def check_that_user_bin_dir_is_in_path
    user_bin_dir = @bin_dir || Gem.bindir(gem_home)
    user_bin_dir.gsub!(File::SEPARATOR, File::ALT_SEPARATOR) if File::ALT_SEPARATOR
    unless ENV['PATH'].split(File::PATH_SEPARATOR).include? user_bin_dir then
      unless self.class.path_warning then
        alert_warning "You don't have #{user_bin_dir} in your PATH,\n\t  gem executables will not run."
        self.class.path_warning = true
      end
    end
  end

  def verify_gem_home(unpack = false)
    FileUtils.mkdir_p gem_home
    raise Gem::FilePermissionError, gem_home unless
      unpack or File.writable?(gem_home)
  end

  ##
  # Return the text for an application file.

  def app_script_text(bin_file_name)
    return <<-TEXT
#{shebang bin_file_name}
#
# This file was generated by RubyGems.
#
# The application '#{spec.name}' is installed as part of a gem, and
# this file is here to facilitate running it.
#

require 'rubygems'

version = "#{Gem::Requirement.default}"

if ARGV.first =~ /^_(.*)_$/ and Gem::Version.correct? $1 then
  version = $1
  ARGV.shift
end

gem '#{spec.name}', version
load Gem.bin_path('#{spec.name}', '#{bin_file_name}', version)
TEXT
  end

  ##
  # return the stub script text used to launch the true ruby script

  def windows_stub_script(bindir, bin_file_name)
    ruby = File.basename(Gem.ruby).chomp('"')
    return <<-TEXT
@ECHO OFF
IF NOT "%~f0" == "~f0" GOTO :WinNT
@"#{ruby}" "#{File.join(bindir, bin_file_name)}" %1 %2 %3 %4 %5 %6 %7 %8 %9
GOTO :EOF
:WinNT
@"#{ruby}" "%~dpn0" %*
TEXT

  end

  ##
  # Builds extensions.  Valid types of extensions are extconf.rb files,
  # configure scripts and rakefiles or mkrf_conf files.

  def build_extensions
    return if spec.extensions.empty?
    say "Building native extensions.  This could take a while..."
    dest_path = File.join gem_dir, spec.require_paths.first
    ran_rake = false # only run rake once

    spec.extensions.each do |extension|
      break if ran_rake
      results = []

      builder = case extension
                when /extconf/ then
                  Gem::Ext::ExtConfBuilder
                when /configure/ then
                  Gem::Ext::ConfigureBuilder
                when /rakefile/i, /mkrf_conf/i then
                  ran_rake = true
                  Gem::Ext::RakeBuilder
                else
                  results = ["No builder for extension '#{extension}'"]
                  nil
                end


      extension_dir = begin
                        File.join gem_dir, File.dirname(extension)
                      rescue TypeError # extension == nil
                        gem_dir
                      end


      begin
        Dir.chdir extension_dir do
          results = builder.build(extension, gem_dir, dest_path, results)

          say results.join("\n") if Gem.configuration.really_verbose
        end
      rescue
        results = results.join "\n"

        gem_make_out = File.join extension_dir, 'gem_make.out'

        open gem_make_out, 'wb' do |io| io.puts results end

        message = <<-EOF
ERROR: Failed to build gem native extension.

        #{results}

Gem files will remain installed in #{gem_dir} for inspection.
Results logged to #{gem_make_out}
EOF

        raise ExtensionBuildError, message
      end
    end
  end

  ##
  # Reads the file index and extracts each file into the gem directory.
  #
  # Ensures that files can't be installed outside the gem directory.

  def extract_files
    raise ArgumentError, "format required to extract from" if @format.nil?

    @format.file_entries.each do |entry, file_data|
      path = entry['path'].untaint

      if path.start_with? "/" then # for extra sanity
        raise Gem::InstallError, "attempt to install file into #{entry['path']}"
      end

      path = File.expand_path File.join(gem_dir, path)

      unless path.start_with? gem_dir then
        msg = "attempt to install file into %p under %s" %
                [entry['path'], gem_dir]
        raise Gem::InstallError, msg
      end

      FileUtils.rm_rf(path) if File.exist? path

      dir = File.dirname path
      FileUtils.mkdir_p dir unless File.exist? dir

      File.open(path, "wb") do |out|
        out.write file_data
      end

      FileUtils.chmod entry['mode'], path

      say path if Gem.configuration.really_verbose
    end
  end

  ##
  # Prefix and suffix the program filename the same as ruby.

  def formatted_program_filename(filename)
    if @format_executable then
      self.class.exec_format % File.basename(filename)
    else
      filename
    end
  end

  ##
  #
  # Return the target directory where the gem is to be installed. This
  # directory is not guaranteed to be populated.
  #

  def dir
    gem_dir.to_s
  end
end

