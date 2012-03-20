require 'rubygems'
require 'rubygems/user_interaction'
require 'fileutils'
require 'rdoc'

##
# Gem::RDoc provides methods to generate RDoc and ri data for installed gems
# upon gem installation.
#
# This file is automatically required by RubyGems 1.9 and newer.

class RDoc::RubygemsHook

  include Gem::UserInteraction

  @rdoc_version = nil
  @specs = []

  ##
  # Force installation of documentation?

  attr_accessor :force

  ##
  # Generate rdoc?

  attr_accessor :generate_rdoc

  ##
  # Generate ri data?

  attr_accessor :generate_ri

  class << self

    ##
    # Loaded version of RDoc.  Set by ::load_rdoc

    attr_reader :rdoc_version

  end

  ##
  # Post installs hook that generates documentation for each specification in
  # +specs+

  def self.generation_hook installer, specs
    types     = installer.document

    generate_rdoc = types.include? 'rdoc'
    generate_ri   = types.include? 'ri'

    specs.each do |spec|
      new(spec, generate_rdoc, generate_ri).generate
    end
  end

  ##
  # Loads the RDoc generator

  def self.load_rdoc
    return if @rdoc_version

    require 'rdoc/rdoc'

    @rdoc_version = Gem::Version.new ::RDoc::VERSION
  end

  ##
  # Creates a new documentation generator for +spec+.  RDoc and ri data
  # generation can be disabled through +generate_rdoc+ and +generate_ri+
  # respectively.

  def initialize spec, generate_rdoc = true, generate_ri = true
    @doc_dir   = spec.doc_dir
    @file_info = nil
    @force     = false
    @rdoc      = nil
    @spec      = spec

    @generate_rdoc = generate_rdoc
    @generate_ri   = generate_ri

    @rdoc_dir = spec.doc_dir 'rdoc'
    @ri_dir   = spec.doc_dir 'ri'
  end

  ##
  # Removes legacy rdoc arguments from +args+
  #--
  # TODO move to RDoc::Options

  def delete_legacy_args args
    args.delete '--inline-source'
    args.delete '--promiscuous'
    args.delete '-p'
    args.delete '--one-file'
  end

  ##
  # Generates documentation using the named +generator+ ("darkfish" or "ri")
  # and following the given +options+.
  #
  # Documentation will be generated into +destination+

  def document generator, options, destination
    options = options.dup
    options.exclude ||= [] # TODO maybe move to RDoc::Options#finish
    options.setup_generator generator
    options.op_dir = destination
    options.finish

    @rdoc.options = options
    @rdoc.generator = options.generator.new options

    say "Installing #{generator} documentation for #{@spec.full_name}"

    FileUtils.mkdir_p options.op_dir

    Dir.chdir options.op_dir do
      begin
        @rdoc.class.current = @rdoc
        @rdoc.generator.generate @file_info
      ensure
        @rdoc.class.current = nil
      end
    end
  end

  ##
  # Generates RDoc and ri data

  def generate
    return unless @generate_ri or @generate_rdoc

    setup

    ::RDoc::RDoc.reset

    options = ::RDoc::Options.new
    options.default_title = "#{@spec.full_name} Documentation"
    options.files = []
    options.files.push(*@spec.require_paths)
    options.files.push(*@spec.extra_rdoc_files)

    args = @spec.rdoc_options

    case config_args = Gem.configuration[:rdoc]
    when String then
      args = args.concat config_args.split
    when Array then
      args = args.concat config_args
    end

    delete_legacy_args args
    options.parse args
    options.quiet = !Gem.configuration.really_verbose

    @rdoc = new_rdoc
    @rdoc.options = options

    Dir.chdir @spec.full_gem_path do
      @file_info = @rdoc.parse_files options.files
    end

    document 'ri',       options, @ri_dir if
      @generate_ri   and (@force or not File.exist? @ri_dir)

    document 'darkfish', options, @rdoc_dir if
      @generate_rdoc and (@force or not File.exist? @rdoc_dir)
  end

  ##
  # #new_rdoc creates a new RDoc instance.  This method is provided only to
  # make testing easier.

  def new_rdoc # :nodoc:
    ::RDoc::RDoc.new
  end

  ##
  # Is rdoc documentation installed?

  def rdoc_installed?
    File.exist? @rdoc_dir
  end

  ##
  # Removes generated RDoc and ri data

  def remove
    base_dir = @spec.base_dir

    raise Gem::FilePermissionError, base_dir unless File.writable? base_dir

    FileUtils.rm_rf @rdoc_dir
    FileUtils.rm_rf @ri_dir
  end

  ##
  # Is ri data installed?

  def ri_installed?
    File.exist? @ri_dir
  end

  ##
  # Prepares the spec for documentation generation

  def setup
    self.class.load_rdoc

    raise Gem::FilePermissionError, @doc_dir if
      File.exist?(@doc_dir) and not File.writable?(@doc_dir)

    FileUtils.mkdir_p @doc_dir unless File.exist? @doc_dir
  end

end

