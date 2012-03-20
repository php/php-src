#--
# Copyright 2006 by Chad Fowler, Rich Kilmer, Jim Weirich and others.
# All rights reserved.
# See LICENSE.txt for permissions.
#++

require 'rubygems'

##
# The documentation manager generates RDoc and RI for RubyGems.

class Gem::DocManager

  include Gem::UserInteraction

  @configured_args = []

  def self.configured_args
    @configured_args ||= []
  end

  def self.configured_args=(args)
    case args
    when Array
      @configured_args = args
    when String
      @configured_args = args.split
    end
  end

  ##
  # Load RDoc from a gem if it is available, otherwise from Ruby's stdlib

  def self.load_rdoc
    begin
      gem 'rdoc'
    rescue Gem::LoadError
      # use built-in RDoc
    end

    begin
      require 'rdoc/rdoc'

      @rdoc_version = if defined? RDoc::VERSION then
                        Gem::Version.new RDoc::VERSION
                      else
                        Gem::Version.new '1.0.1' # HACK parsing is hard
                      end

    rescue LoadError => e
      raise Gem::DocumentError,
          "ERROR: RDoc documentation generator not installed: #{e}"
    end
  end

  def self.rdoc_version
    @rdoc_version
  end

  ##
  # Updates the RI cache for RDoc 2 if it is installed

  def self.update_ri_cache
    load_rdoc rescue return

    return unless defined? RDoc::VERSION # RDoc 1 does not have VERSION

    require 'rdoc/ri/driver'

    options = {
      :use_cache => true,
      :use_system => true,
      :use_site => true,
      :use_home => true,
      :use_gems => true,
      :formatter => RDoc::RI::Formatter,
    }

    RDoc::RI::Driver.new(options).class_cache
  end

  ##
  # Create a document manager for +spec+. +rdoc_args+ contains arguments for
  # RDoc (template etc.) as a String.

  def initialize(spec, rdoc_args="")
    require 'fileutils'
    @spec = spec
    @doc_dir = spec.doc_dir
    @rdoc_args = rdoc_args.nil? ? [] : rdoc_args.split
  end

  ##
  # Is the RDoc documentation installed?

  def rdoc_installed?
    File.exist?(File.join(@doc_dir, "rdoc"))
  end

  ##
  # Is the RI documentation installed?

  def ri_installed?
    File.exist?(File.join(@doc_dir, "ri"))
  end

  ##
  # Generate the RI documents for this gem spec.
  #
  # Note that if both RI and RDoc documents are generated from the same
  # process, the RI docs should be done first (a likely bug in RDoc will cause
  # RI docs generation to fail if run after RDoc).

  def generate_ri
    setup_rdoc
    install_ri # RDoc bug, ri goes first

    FileUtils.mkdir_p @doc_dir unless File.exist?(@doc_dir)
  end

  ##
  # Generate the RDoc documents for this gem spec.
  #
  # Note that if both RI and RDoc documents are generated from the same
  # process, the RI docs should be done first (a likely bug in RDoc will cause
  # RI docs generation to fail if run after RDoc).

  def generate_rdoc
    setup_rdoc
    install_rdoc

    FileUtils.mkdir_p @doc_dir unless File.exist?(@doc_dir)
  end

  ##
  # Generate and install RDoc into the documentation directory

  def install_rdoc
    rdoc_dir = File.join @doc_dir, 'rdoc'

    FileUtils.rm_rf rdoc_dir

    say "Installing RDoc documentation for #{@spec.full_name}..."
    run_rdoc '--op', rdoc_dir
  end

  ##
  # Generate and install RI into the documentation directory

  def install_ri
    ri_dir = File.join @doc_dir, 'ri'

    FileUtils.rm_rf ri_dir

    say "Installing ri documentation for #{@spec.full_name}..."
    run_rdoc '--ri', '--op', ri_dir
  end

  ##
  # Run RDoc with +args+, which is an ARGV style argument list

  def run_rdoc(*args)
    args << @spec.rdoc_options
    args << self.class.configured_args
    args << @spec.require_paths.clone
    args << @spec.extra_rdoc_files
    args << '--title' << "#{@spec.full_name} Documentation"
    args << '--quiet'
    args = args.flatten.map do |arg| arg.to_s end

    if self.class.rdoc_version >= Gem::Version.new('2.4.0') then
      args.delete '--inline-source'
      args.delete '--promiscuous'
      args.delete '-p'
      args.delete '--one-file'
      # HACK more
    end

    debug_args = args.dup

    r = RDoc::RDoc.new

    old_pwd = Dir.pwd
    Dir.chdir @spec.full_gem_path

    say "rdoc #{args.join ' '}" if Gem.configuration.really_verbose

    begin
      r.document args
    rescue Errno::EACCES => e
      dirname = File.dirname e.message.split("-")[1].strip
      raise Gem::FilePermissionError.new(dirname)
    rescue Interrupt => e
      raise e
    rescue Exception => ex
      alert_error "While generating documentation for #{@spec.full_name}"
      ui.errs.puts "... MESSAGE:   #{ex}"
      ui.errs.puts "... RDOC args: #{debug_args.join(' ')}"
      ui.errs.puts "\t#{ex.backtrace.join "\n\t"}" if
        Gem.configuration.backtrace
      terminate_interaction 1
    ensure
      Dir.chdir old_pwd
    end
  end

  def setup_rdoc
    if File.exist?(@doc_dir) && !File.writable?(@doc_dir) then
      raise Gem::FilePermissionError.new(@doc_dir)
    end

    FileUtils.mkdir_p @doc_dir unless File.exist?(@doc_dir)

    self.class.load_rdoc
  end

  ##
  # Remove RDoc and RI documentation

  def uninstall_doc
    base_dir = @spec.base_dir
    raise Gem::FilePermissionError.new base_dir unless File.writable? base_dir

    # TODO: ok... that's twice... ugh
    old_name = [
      @spec.name, @spec.version, @spec.original_platform].join '-'

    doc_dir = @spec.doc_dir
    unless File.directory? doc_dir then
      doc_dir = File.join File.dirname(doc_dir), old_name
    end

    ri_dir = @spec.ri_dir
    unless File.directory? ri_dir then
      ri_dir = File.join File.dirname(ri_dir), old_name
    end

    FileUtils.rm_rf doc_dir
    FileUtils.rm_rf ri_dir
  end

end

