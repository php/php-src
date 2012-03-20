#--
# Copyright 2006 by Chad Fowler, Rich Kilmer, Jim Weirich and others.
# All rights reserved.
# See LICENSE.txt for permissions.
#++

require 'rubygems/version'
require 'rubygems/requirement'
require 'rubygems/platform'
require "rubygems/deprecate"

# :stopdoc:
class Date; end # for ruby_code if date.rb wasn't required
# :startdoc:

##
# The Specification class contains the metadata for a Gem.  Typically
# defined in a .gemspec file or a Rakefile, and looks like this:
#
#   spec = Gem::Specification.new do |s|
#     s.name = 'example'
#     s.version = '1.0'
#     s.summary = 'Example gem specification'
#     ...
#   end
#
# For a great way to package gems, use Hoe.

class Gem::Specification

  ##
  # The the version number of a specification that does not specify one
  # (i.e. RubyGems 0.7 or earlier).

  NONEXISTENT_SPECIFICATION_VERSION = -1

  ##
  # The specification version applied to any new Specification instances
  # created.  This should be bumped whenever something in the spec format
  # changes.
  #
  # Specification Version History:
  #
  #   spec   ruby
  #    ver    ver yyyy-mm-dd description
  #     -1 <0.8.0            pre-spec-version-history
  #      1  0.8.0 2004-08-01 Deprecated "test_suite_file" for "test_files"
  #                          "test_file=x" is a shortcut for "test_files=[x]"
  #      2  0.9.5 2007-10-01 Added "required_rubygems_version"
  #                          Now forward-compatible with future versions
  #      3  1.3.2 2009-01-03 Added Fixnum validation to specification_version
  #--
  # When updating this number, be sure to also update #to_ruby.
  #
  # NOTE RubyGems < 1.2 cannot load specification versions > 2.

  CURRENT_SPECIFICATION_VERSION = 3

  # :stopdoc:

  # version => # of fields
  MARSHAL_FIELDS = { -1 => 16, 1 => 16, 2 => 16, 3 => 17 }

  today = Time.now.utc
  TODAY = Time.utc(today.year, today.month, today.day)

  # :startdoc:

  ##
  # List of attribute names: [:name, :version, ...]

  @@required_attributes = [:rubygems_version,
                           :specification_version,
                           :name,
                           :version,
                           :date,
                           :summary,
                           :require_paths]

  ##
  # Map of attribute names to default values.

  @@default_value = {
    :authors                   => [],
    :autorequire               => nil,
    :bindir                    => 'bin',
    :cert_chain                => [],
    :date                      => TODAY,
    :dependencies              => [],
    :description               => nil,
    :email                     => nil,
    :executables               => [],
    :extensions                => [],
    :extra_rdoc_files          => [],
    :files                     => [],
    :homepage                  => nil,
    :licenses                  => [],
    :name                      => nil,
    :platform                  => Gem::Platform::RUBY,
    :post_install_message      => nil,
    :rdoc_options              => [],
    :require_paths             => ['lib'],
    :required_ruby_version     => Gem::Requirement.default,
    :required_rubygems_version => Gem::Requirement.default,
    :requirements              => [],
    :rubyforge_project         => nil,
    :rubygems_version          => Gem::VERSION,
    :signing_key               => nil,
    :specification_version     => CURRENT_SPECIFICATION_VERSION,
    :summary                   => nil,
    :test_files                => [],
    :version                   => nil,
  }

  @@attributes = @@default_value.keys.sort_by { |s| s.to_s }
  @@array_attributes = @@default_value.reject { |k,v| v != [] }.keys
  @@nil_attributes, @@non_nil_attributes = @@default_value.keys.partition { |k|
    @@default_value[k].nil?
  }

  ######################################################################
  # :section: Required gemspec attributes

  ##
  # This gem's name

  attr_accessor :name

  ##
  # This gem's version

  attr_reader :version

  ##
  # Paths in the gem to add to $LOAD_PATH when this gem is activated.
  #
  # The default ['lib'] is typically sufficient.

  attr_accessor :require_paths

  ##
  # The version of RubyGems used to create this gem.
  #
  # Do not set this, it is set automatically when the gem is packaged.

  attr_accessor :rubygems_version

  ##
  # The Gem::Specification version of this gemspec.
  #
  # Do not set this, it is set automatically when the gem is packaged.

  attr_accessor :specification_version

  ##
  # A short summary of this gem's description.  Displayed in `gem list -d`.
  #
  # The description should be more detailed than the summary.  For example,
  # you might wish to copy the entire README into the description.

  attr_reader :summary

  ######################################################################
  # :section: Optional gemspec attributes

  ##
  # Autorequire was used by old RubyGems to automatically require a file.
  #
  # Deprecated: It is neither supported nor functional.

  attr_accessor :autorequire

  ##
  # The path in the gem for executable scripts.  Usually 'bin'

  attr_accessor :bindir

  ##
  # The certificate chain used to sign this gem.  See Gem::Security for
  # details.

  attr_accessor :cert_chain

  ##
  # A long description of this gem

  attr_reader :description

  ##
  # Sets the default executable for this gem.
  #
  # Deprecated: You must now specify the executable name to  Gem.bin_path.

  attr_writer :default_executable

  ##
  # A contact email for this gem
  #
  # If you are providing multiple authors and multiple emails they should be
  # in the same order such that:
  #
  #   Hash[*spec.authors.zip(spec.emails).flatten]
  #
  # Gives a hash of author name to email address.

  attr_accessor :email

  ##
  # The URL of this gem's home page

  attr_accessor :homepage

  ##
  # True when this gemspec has been activated. This attribute is not persisted.

  attr_accessor :loaded # :nodoc:

  alias :loaded? :loaded # :nodoc:

  ##
  # True when this gemspec has been activated. This attribute is not persisted.

  attr_accessor :activated

  alias :activated? :activated

  ##
  # Path this gemspec was loaded from.  This attribute is not persisted.

  attr_reader :loaded_from

  ##
  # Allows deinstallation of gems with legacy platforms.

  attr_writer :original_platform # :nodoc:

  ##
  # A message that gets displayed after the gem is installed

  attr_accessor :post_install_message

  ##
  # The version of ruby required by this gem

  attr_reader :required_ruby_version

  ##
  # The RubyGems version required by this gem

  attr_reader :required_rubygems_version

  ##
  # The rubyforge project this gem lives under.  i.e. RubyGems'
  # rubyforge_project is "rubygems".

  attr_accessor :rubyforge_project

  ##
  # The key used to sign this gem.  See Gem::Security for details.

  attr_accessor :signing_key

  def self._all # :nodoc:
    unless defined?(@@all) && @@all then
      specs = []

      self.dirs.reverse_each { |dir|
        Dir[File.join(dir, "*.gemspec")].each { |path|
          spec = Gem::Specification.load path.untaint
          # #load returns nil if the spec is bad, so we just ignore
          # it at this stage
          specs << spec if spec
        }
      }

      @@all = specs
      _resort!
    end
    @@all
  end

  def self._resort! # :nodoc:
    @@all.sort! { |a, b|
      names = a.name <=> b.name
      next names if names.nonzero?
      b.version <=> a.version
    }
  end

  ##
  # Adds +spec+ to the known specifications, keeping the collection
  # properly sorted.

  def self.add_spec spec
    # TODO: find all extraneous adds
    # puts
    # p :add_spec => [spec.full_name, caller.reject { |s| s =~ /minitest/ }]

    # TODO: flush the rest of the crap from the tests
    # raise "no dupes #{spec.full_name} in #{all_names.inspect}" if
    #   _all.include? spec

    raise "nil spec!" unless spec # TODO: remove once we're happy with tests

    return if _all.include? spec

    _all << spec
    _resort!
  end

  ##
  # Adds multiple specs to the known specifications.

  def self.add_specs *specs
    raise "nil spec!" if specs.any?(&:nil?) # TODO: remove once we're happy

    # TODO: this is much more efficient, but we need the extra checks for now
    # _all.concat specs
    # _resort!

    specs.each do |spec| # TODO: slow
      add_spec spec
    end
  end

  ##
  # Returns all specifications. This method is discouraged from use.
  # You probably want to use one of the Enumerable methods instead.

  def self.all
    warn "NOTE: Specification.all called from #{caller.first}" unless
      Gem::Deprecate.skip
    _all
  end

  ##
  # Sets the known specs to +specs+. Not guaranteed to work for you in
  # the future. Use at your own risk. Caveat emptor. Doomy doom doom.
  # Etc etc.
  #
  #--
  # Makes +specs+ the known specs
  # Listen, time is a river
  # Winter comes, code breaks
  #
  # -- wilsonb

  def self.all= specs
    @@all = specs
  end

  ##
  # Return full names of all specs in sorted order.

  def self.all_names
    self._all.map(&:full_name)
  end

  ##
  # Return the list of all array-oriented instance variables.
  #--
  # Not sure why we need to use so much stupid reflection in here...

  def self.array_attributes
    @@array_attributes.dup
  end

  ##
  # Return the list of all instance variables.
  #--
  # Not sure why we need to use so much stupid reflection in here...

  def self.attribute_names
    @@attributes.dup
  end

  ##
  # Return the directories that Specification uses to find specs.

  def self.dirs
    @@dirs ||= Gem.path.collect { |dir|
      File.join dir, "specifications"
    }
  end

  ##
  # Set the directories that Specification uses to find specs. Setting
  # this resets the list of known specs.

  def self.dirs= dirs
    # TODO: find extra calls to dir=
    # warn "NOTE: dirs= called from #{caller.first} for #{dirs.inspect}"

    self.reset

    # ugh
    @@dirs = Array(dirs).map { |dir| File.join dir, "specifications" }
  end

  extend Enumerable

  ##
  # Enumerate every known spec.  See ::dirs= and ::add_spec to set the list of
  # specs.

  def self.each
    return enum_for(:each) unless block_given?

    self._all.each do |x|
      yield x
    end
  end

  ##
  # Returns every spec that matches +name+ and optional +requirements+.

  def self.find_all_by_name name, *requirements
    requirements = Gem::Requirement.default if requirements.empty?

    # TODO: maybe try: find_all { |s| spec === dep }

    Gem::Dependency.new(name, *requirements).matching_specs
  end

  ##
  # Find the best specification matching a +name+ and +requirements+. Raises
  # if the dependency doesn't resolve to a valid specification.

  def self.find_by_name name, *requirements
    requirements = Gem::Requirement.default if requirements.empty?

    # TODO: maybe try: find { |s| spec === dep }

    Gem::Dependency.new(name, *requirements).to_spec
  end

  ##
  # Return the best specification that contains the file matching +path+.

  def self.find_by_path path
    self.find { |spec|
      spec.contains_requirable_file? path
    }
  end

  ##
  # Return currently unresolved specs that contain the file matching +path+.

  def self.find_in_unresolved path
    # TODO: do we need these?? Kill it
    specs = Gem.unresolved_deps.values.map { |dep| dep.to_specs }.flatten

    specs.find_all { |spec| spec.contains_requirable_file? path }
  end

  ##
  # Search through all unresolved deps and sub-dependencies and return
  # specs that contain the file matching +path+.

  def self.find_in_unresolved_tree path
    specs = Gem.unresolved_deps.values.map { |dep| dep.to_specs }.flatten

    specs.reverse_each do |spec|
      trails = []
      spec.traverse do |from_spec, dep, to_spec, trail|
        next unless to_spec.conflicts.empty?
        trails << trail if to_spec.contains_requirable_file? path
      end

      next if trails.empty?

      return trails.map(&:reverse).sort.first.reverse
    end

    []
  end

  ##
  # Special loader for YAML files.  When a Specification object is loaded
  # from a YAML file, it bypasses the normal Ruby object initialization
  # routine (#initialize).  This method makes up for that and deals with
  # gems of different ages.
  #
  # +input+ can be anything that YAML.load() accepts: String or IO.

  def self.from_yaml(input)
    input = normalize_yaml_input input
    spec = YAML.load input

    if spec && spec.class == FalseClass then
      raise Gem::EndOfYAMLException
    end

    unless Gem::Specification === spec then
      raise Gem::Exception, "YAML data doesn't evaluate to gem specification"
    end

    unless (spec.instance_variables.include? '@specification_version' or
            spec.instance_variables.include? :@specification_version) and
           spec.instance_variable_get :@specification_version
      spec.instance_variable_set :@specification_version,
                                 NONEXISTENT_SPECIFICATION_VERSION
    end

    spec
  end

  ##
  # Return the latest specs, optionally including prerelease specs if
  # +prerelease+ is true.

  def self.latest_specs prerelease = false
    result = Hash.new { |h,k| h[k] = {} }
    native = {}

    Gem::Specification._all.reverse_each do |spec|
      next if spec.version.prerelease? unless prerelease

      native[spec.name] = spec.version if spec.platform == Gem::Platform::RUBY
      result[spec.name][spec.platform] = spec
    end

    result.map(&:last).map(&:values).flatten.reject { |spec|
      minimum = native[spec.name]
      minimum && spec.version < minimum
    }
  end

  ##
  # Loads Ruby format gemspec from +file+.

  def self.load file
    return unless file && File.file?(file)

    file = file.dup.untaint

    code = if defined? Encoding
             File.read file, :encoding => "UTF-8"
           else
             File.read file
           end

    code.untaint

    begin
      spec = eval code, binding, file

      if Gem::Specification === spec
        spec.loaded_from = file.to_s
        return spec
      end

      warn "[#{file}] isn't a Gem::Specification (#{spec.class} instead)."
    rescue SignalException, SystemExit
      raise
    rescue SyntaxError, Exception => e
      warn "Invalid gemspec in [#{file}]: #{e}"
    end

    nil
  end

  ##
  # Specification attributes that must be non-nil

  def self.non_nil_attributes
    @@non_nil_attributes.dup
  end

  ##
  # Make sure the YAML specification is properly formatted with dashes

  def self.normalize_yaml_input(input)
    result = input.respond_to?(:read) ? input.read : input
    result = "--- " + result unless result =~ /\A--- /
    result.gsub!(/ !!null \n/, " \n")
    # date: 2011-04-26 00:00:00.000000000Z
    # date: 2011-04-26 00:00:00.000000000 Z
    result.gsub!(/^(date: \d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}\.\d+?)Z/, '\1 Z')
    result
  end

  ##
  # Return a list of all outdated specifications. This method is HEAVY
  # as it must go fetch specifications from the server.

  def self.outdated
    outdateds = []

    # TODO: maybe we should switch to rubygems' version service?
    fetcher = Gem::SpecFetcher.fetcher

    latest_specs.each do |local|
      dependency = Gem::Dependency.new local.name, ">= #{local.version}"
      remotes    = fetcher.find_matching dependency
      remotes    = remotes.map { |(_, version, _), _| version }
      latest     = remotes.sort.last

      outdateds << local.name if latest and local.version < latest
    end

    outdateds
  end

  ##
  # Removes +spec+ from the known specs.

  def self.remove_spec spec
    # TODO: beat on the tests
    raise "wtf: #{spec.full_name} not in #{all_names.inspect}" unless
      _all.include? spec
    _all.delete spec
  end

  ##
  # Is +name+ a required attribute?

  def self.required_attribute?(name)
    @@required_attributes.include? name.to_sym
  end

  ##
  # Required specification attributes

  def self.required_attributes
    @@required_attributes.dup
  end

  ##
  # Reset the list of known specs, running pre and post reset hooks
  # registered in Gem.

  def self.reset
    @@dirs = nil
    # from = caller.first(10).reject { |s| s =~ /minitest/ }
    # warn ""
    # warn "NOTE: Specification.reset from #{from.inspect}"
    Gem.pre_reset_hooks.each  { |hook| hook.call }
    @@all = nil
    Gem.post_reset_hooks.each { |hook| hook.call }
  end

  ##
  # Load custom marshal format, re-initializing defaults as needed

  def self._load(str)
    array = Marshal.load str

    spec = Gem::Specification.new
    spec.instance_variable_set :@specification_version, array[1]

    current_version = CURRENT_SPECIFICATION_VERSION

    field_count = if spec.specification_version > current_version then
                    spec.instance_variable_set :@specification_version,
                                               current_version
                    MARSHAL_FIELDS[current_version]
                  else
                    MARSHAL_FIELDS[spec.specification_version]
                  end

    if array.size < field_count then
      raise TypeError, "invalid Gem::Specification format #{array.inspect}"
    end

    spec.instance_variable_set :@rubygems_version,          array[0]
    # spec version
    spec.instance_variable_set :@name,                      array[2]
    spec.instance_variable_set :@version,                   array[3]
    spec.instance_variable_set :@date,                      array[4]
    spec.instance_variable_set :@summary,                   array[5]
    spec.instance_variable_set :@required_ruby_version,     array[6]
    spec.instance_variable_set :@required_rubygems_version, array[7]
    spec.instance_variable_set :@original_platform,         array[8]
    spec.instance_variable_set :@dependencies,              array[9]
    spec.instance_variable_set :@rubyforge_project,         array[10]
    spec.instance_variable_set :@email,                     array[11]
    spec.instance_variable_set :@authors,                   array[12]
    spec.instance_variable_set :@description,               array[13]
    spec.instance_variable_set :@homepage,                  array[14]
    spec.instance_variable_set :@has_rdoc,                  array[15]
    spec.instance_variable_set :@new_platform,              array[16]
    spec.instance_variable_set :@platform,                  array[16].to_s
    spec.instance_variable_set :@license,                   array[17]
    spec.instance_variable_set :@loaded,                    false
    spec.instance_variable_set :@activated,                 false

    spec
  end

  def <=>(other) # :nodoc:
    sort_obj <=> other.sort_obj
  end

  def == other # :nodoc:
    self.class === other &&
      name == other.name &&
      version == other.version &&
      platform == other.platform
  end

  ##
  # Dump only crucial instance variables.
  #--
  # MAINTAIN ORDER!
  # (down with the man)

  def _dump(limit)
    Marshal.dump [
      @rubygems_version,
      @specification_version,
      @name,
      @version,
      date,
      @summary,
      @required_ruby_version,
      @required_rubygems_version,
      @original_platform,
      @dependencies,
      @rubyforge_project,
      @email,
      @authors,
      @description,
      @homepage,
      true, # has_rdoc
      @new_platform,
      @licenses
    ]
  end

  ##
  # Activate this spec, registering it as a loaded spec and adding
  # it's lib paths to $LOAD_PATH. Returns true if the spec was
  # activated, false if it was previously activated. Freaks out if
  # there are conflicts upon activation.

  def activate
    raise_if_conflicts

    return false if Gem.loaded_specs[self.name]

    activate_dependencies
    add_self_to_load_path

    Gem.loaded_specs[self.name] = self
    @activated = true
    @loaded = true

    return true
  end

  ##
  # Activate all unambiguously resolved runtime dependencies of this
  # spec. Add any ambigous dependencies to the unresolved list to be
  # resolved later, as needed.

  def activate_dependencies
    self.runtime_dependencies.each do |spec_dep|
      # TODO: check for conflicts! not just name!
      next if Gem.loaded_specs.include? spec_dep.name
      specs = spec_dep.to_specs

      if specs.size == 1 then
        specs.first.activate
      else
        name = spec_dep.name
        Gem.unresolved_deps[name] = Gem.unresolved_deps[name].merge spec_dep
      end
    end

    Gem.unresolved_deps.delete self.name
  end

  ##
  # Returns an array with bindir attached to each executable in the
  # +executables+ list

  def add_bindir(executables)
    return nil if executables.nil?

    if @bindir then
      Array(executables).map { |e| File.join(@bindir, e) }
    else
      executables
    end
  rescue
    return nil
  end

  ##
  # Adds a dependency on gem +dependency+ with type +type+ that requires
  # +requirements+.  Valid types are currently <tt>:runtime</tt> and
  # <tt>:development</tt>.

  def add_dependency_with_type(dependency, type, *requirements)
    requirements = if requirements.empty? then
                     Gem::Requirement.default
                   else
                     requirements.flatten
                   end

    unless dependency.respond_to?(:name) &&
      dependency.respond_to?(:version_requirements)

      dependency = Gem::Dependency.new(dependency, requirements, type)
    end

    dependencies << dependency
  end

  private :add_dependency_with_type

  ##
  # Adds a development dependency named +gem+ with +requirements+ to this
  # Gem.  For example:
  #
  #   spec.add_development_dependency 'example', '~> 1.1', '>= 1.1.4'
  #
  # Development dependencies aren't installed by default and aren't
  # activated when a gem is required.

  def add_development_dependency(gem, *requirements)
    add_dependency_with_type(gem, :development, *requirements)
  end

  ##
  # Adds a runtime dependency named +gem+ with +requirements+ to this Gem.
  # For example:
  #
  #   spec.add_runtime_dependency 'example', '~> 1.1', '>= 1.1.4'

  def add_runtime_dependency(gem, *requirements)
    add_dependency_with_type(gem, :runtime, *requirements)
  end

  alias add_dependency add_runtime_dependency

  ##
  # Adds this spec's require paths to LOAD_PATH, in the proper location.

  def add_self_to_load_path
    paths = require_paths.map do |path|
      File.join full_gem_path, path
    end

    # gem directories must come after -I and ENV['RUBYLIB']
    insert_index = Gem.load_path_insert_index

    if insert_index then
      # gem directories must come after -I and ENV['RUBYLIB']
      $LOAD_PATH.insert(insert_index, *paths)
    else
      # we are probably testing in core, -I and RUBYLIB don't apply
      $LOAD_PATH.unshift(*paths)
    end
  end

  ##
  # Singular reader for #authors

  def author
    val = authors and val.first
  end

  ##
  # Singular writer for #authors

  def author= o
    self.authors = [o]
  end

  ##
  # The list of author names who wrote this gem.
  #
  # If you are providing multiple authors and multiple emails they should be
  # in the same order such that:
  #
  #   Hash[*spec.authors.zip(spec.emails).flatten]
  #
  # Gives a hash of author name to email address.

  def authors
    @authors ||= []
  end

  ##
  # Sets the list of authors, ensuring it is an array.

  def authors= value
    @authors = Array(value).flatten.grep(String)
  end

  ##
  # Returns the full path to the base gem directory.
  #
  # eg: /usr/local/lib/ruby/gems/1.8

  def base_dir
    return Gem.dir unless loaded_from
    @base_dir ||= File.dirname File.dirname loaded_from
  end

  ##
  # Returns the full path to installed gem's bin directory.
  #
  # NOTE: do not confuse this with +bindir+, which is just 'bin', not
  # a full path.

  def bin_dir
    @bin_dir ||= File.join gem_dir, bindir # TODO: this is unfortunate
  end

  ##
  # Returns the full path to an executable named +name+ in this gem.

  def bin_file name
    File.join bin_dir, name
  end

  ##
  # Returns the full path to the cache directory containing this
  # spec's cached gem.

  def cache_dir
    @cache_dir ||= File.join base_dir, "cache"
  end

  ##
  # Returns the full path to the cached gem for this spec.

  def cache_file
    @cache_file ||= File.join cache_dir, "#{full_name}.gem"
  end

  alias :cache_gem :cache_file

  ##
  # Return any possible conflicts against the currently loaded specs.

  def conflicts
    conflicts = {}
    Gem.loaded_specs.values.each do |spec|
      bad = self.runtime_dependencies.find_all { |dep|
        spec.name == dep.name and not spec.satisfies_requirement? dep
      }

      conflicts[spec] = bad unless bad.empty?
    end
    conflicts
  end

  ##
  # Return true if this spec can require +file+.

  def contains_requirable_file? file
    root = full_gem_path

    require_paths.each do |lib|
      base = "#{root}/#{lib}/#{file}"
      Gem.suffixes.each do |suf|
        path = "#{base}#{suf}"
        return true if File.file? path
      end
    end

    return false
  end

  ##
  # The date this gem was created. Lazily defaults to TODAY.

  def date
    @date ||= TODAY
  end

  ##
  # The date this gem was created
  #
  # Do not set this, it is set automatically when the gem is packaged.

  def date= date
    # We want to end up with a Time object with one-day resolution.
    # This is the cleanest, most-readable, faster-than-using-Date
    # way to do it.
    @date = case date
            when String then
              if /\A(\d{4})-(\d{2})-(\d{2})\Z/ =~ date then
                Time.utc($1.to_i, $2.to_i, $3.to_i)
              else
                raise(Gem::InvalidSpecificationException,
                      "invalid date format in specification: #{date.inspect}")
              end
            when Time, Date then
              Time.utc(date.year, date.month, date.day)
            else
              TODAY
            end
  end

  ##
  # The default executable for this gem.
  #
  # Deprecated: The name of the gem is assumed to be the name of the
  # executable now.  See Gem.bin_path.

  def default_executable
    if defined?(@default_executable) and @default_executable
      result = @default_executable
    elsif @executables and @executables.size == 1
      result = Array(@executables).first
    else
      result = nil
    end
    result
  end

  ##
  # The default value for specification attribute +name+

  def default_value name
    @@default_value[name]
  end

  ##
  # A list of Gem::Dependency objects this gem depends on.
  #
  # Use #add_dependency or #add_development_dependency to add dependencies to
  # a gem.

  def dependencies
    @dependencies ||= []
  end

  ##
  # Return a list of all gems that have a dependency on this gemspec.  The
  # list is structured with entries that conform to:
  #
  #   [depending_gem, dependency, [list_of_gems_that_satisfy_dependency]]

  def dependent_gems
    out = []
    Gem::Specification.each do |spec|
      spec.dependencies.each do |dep|
        if self.satisfies_requirement?(dep) then
          sats = []
          find_all_satisfiers(dep) do |sat|
            sats << sat
          end
          out << [spec, dep, sats]
        end
      end
    end
    out
  end

  ##
  # Returns all specs that matches this spec's runtime dependencies.

  def dependent_specs
    runtime_dependencies.map { |dep| dep.to_specs }.flatten
  end

  ##
  # A long description of this gem

  def description= str
    @description = str.to_s
  end

  ##
  # List of dependencies that are used for development

  def development_dependencies
    dependencies.select { |d| d.type == :development }
  end

  ##
  # Returns the full path to this spec's documentation directory.

  def doc_dir
    @doc_dir ||= File.join base_dir, 'doc', full_name
  end

  def encode_with coder # :nodoc:
    mark_version

    coder.add 'name', @name
    coder.add 'version', @version
    platform = case @original_platform
               when nil, '' then
                 'ruby'
               when String then
                 @original_platform
               else
                 @original_platform.to_s
               end
    coder.add 'platform', platform

    attributes = @@attributes.map(&:to_s) - %w[name version platform]
    attributes.each do |name|
      coder.add name, instance_variable_get("@#{name}")
    end
  end

  def eql? other # :nodoc:
    self.class === other && same_attributes?(other)
  end

  ##
  # Singular accessor for #executables

  def executable
    val = executables and val.first
  end

  ##
  # Singular accessor for #executables

  def executable=o
    self.executables = [o]
  end

  ##
  # Executables included in the gem.

  def executables
    @executables ||= []
  end

  ##
  # Sets executables to +value+, ensuring it is an array. Don't
  # use this, push onto the array instead.

  def executables= value
    # TODO: warn about setting instead of pushing
    @executables = Array(value)
  end

  ##
  # Extensions to build when installing the gem.  See
  # Gem::Installer#build_extensions for valid values.

  def extensions
    @extensions ||= []
  end

  ##
  # Sets extensions to +extensions+, ensuring it is an array. Don't
  # use this, push onto the array instead.

  def extensions= extensions
    # TODO: warn about setting instead of pushing
    @extensions = Array extensions
  end

  ##
  # Extra files to add to RDoc such as README or doc/examples.txt

  def extra_rdoc_files
    @extra_rdoc_files ||= []
  end

  ##
  # Sets extra_rdoc_files to +files+, ensuring it is an array. Don't
  # use this, push onto the array instead.

  def extra_rdoc_files= files
    # TODO: warn about setting instead of pushing
    @extra_rdoc_files = Array files
  end

  ##
  # The default (generated) file name of the gem.  See also #spec_name.
  #
  #   spec.file_name # => "example-1.0.gem"

  def file_name
    "#{full_name}.gem"
  end

  ##
  # Files included in this gem.  You cannot append to this accessor, you must
  # assign to it.
  #
  # Only add files you can require to this list, not directories, etc.
  #
  # Directories are automatically stripped from this list when building a gem,
  # other non-files cause an error.

  def files
    # DO NOT CHANGE TO ||= ! This is not a normal accessor. (yes, it sucks)
    @files = [@files,
              @test_files,
              add_bindir(@executables),
              @extra_rdoc_files,
              @extensions,
             ].flatten.uniq.compact
  end

  ##
  # Sets files to +files+, ensuring it is an array.

  def files= files
    @files = Array files
  end

  ##
  # Finds all gems that satisfy +dep+

  def find_all_satisfiers dep
    Gem::Specification.each do |spec|
      yield spec if spec.satisfies_requirement? dep
    end
  end

  private :find_all_satisfiers

  ##
  # Creates a duplicate spec without large blobs that aren't used at runtime.

  def for_cache
    spec = dup

    spec.files = nil
    spec.test_files = nil

    spec
  end

  ##
  # The full path to the gem (install path + full name).

  def full_gem_path
    # TODO: try to get rid of this... or the awkward
    # TODO: also, shouldn't it default to full_name if it hasn't been written?
    return @full_gem_path if defined?(@full_gem_path) && @full_gem_path

    @full_gem_path = File.expand_path File.join(gems_dir, full_name)

    return @full_gem_path if File.directory? @full_gem_path

    @full_gem_path = File.expand_path File.join(gems_dir, original_name)
  end

  ##
  # Returns the full name (name-version) of this Gem.  Platform information
  # is included (name-version-platform) if it is specified and not the
  # default Ruby platform.

  def full_name
    if platform == Gem::Platform::RUBY or platform.nil? then
      "#{@name}-#{@version}"
    else
      "#{@name}-#{@version}-#{platform}"
    end
  end

  ##
  # Returns the full path to this spec's gem directory.
  # eg: /usr/local/lib/ruby/1.8/gems/mygem-1.0

  def gem_dir
    @gem_dir ||= File.expand_path File.join(gems_dir, full_name)
  end

  ##
  # Returns the full path to the gems directory containing this spec's
  # gem directory. eg: /usr/local/lib/ruby/1.8/gems

  def gems_dir
    # TODO: this logic seems terribly broken, but tests fail if just base_dir
    @gems_dir ||= File.join(loaded_from && base_dir || Gem.dir, "gems")
  end

  ##
  # Deprecated and ignored, defaults to true.
  #
  # Formerly used to indicate this gem was RDoc-capable.

  def has_rdoc
    true
  end

  ##
  # Deprecated and ignored.
  #
  # Formerly used to indicate this gem was RDoc-capable.

  def has_rdoc= ignored
    @has_rdoc = true
  end

  alias :has_rdoc? :has_rdoc

  ##
  # True if this gem has files in test_files

  def has_unit_tests?
    not test_files.empty?
  end

  # :stopdoc:
  alias has_test_suite? has_unit_tests?
  # :startdoc:

  def hash # :nodoc:
    @@attributes.inject(0) { |hash_code, (name, _)|
      hash_code ^ self.send(name).hash
    }
  end

  def init_with coder # :nodoc:
    yaml_initialize coder.tag, coder.map
  end

  ##
  # Specification constructor. Assigns the default values to the attributes
  # and yields itself for further initialization.  Optionally takes +name+ and
  # +version+.

  def initialize name = nil, version = nil
    @loaded = false
    @activated = false
    @loaded_from = nil
    @original_platform = nil

    @@nil_attributes.each do |key|
      instance_variable_set "@#{key}", nil
    end

    @@non_nil_attributes.each do |key|
      default = default_value(key)
      value = case default
              when Time, Numeric, Symbol, true, false, nil then default
              else default.dup
              end

      instance_variable_set "@#{key}", value
    end

    @new_platform = Gem::Platform::RUBY

    self.name = name if name
    self.version = version if version

    yield self if block_given?
  end

  ##
  # Duplicates array_attributes from +other_spec+ so state isn't shared.

  def initialize_copy other_spec
    other_ivars = other_spec.instance_variables
    other_ivars = other_ivars.map { |ivar| ivar.intern } if # for 1.9
      String === other_ivars.first

    self.class.array_attributes.each do |name|
      name = :"@#{name}"
      next unless other_ivars.include? name

      begin
        val = other_spec.instance_variable_get(name)
        if val then
          instance_variable_set name, val.dup
        else
          warn "WARNING: #{full_name} has an invalid nil value for #{name}"
        end
      rescue TypeError
        e = Gem::FormatException.new \
          "#{full_name} has an invalid value for #{name}"

        e.file_path = loaded_from
        raise e
      end
    end
  end

  ##
  # The directory that this gem was installed into.
  # TODO: rename - horrible. this is the base_dir for a gem path

  def installation_path
    loaded_from && base_dir
  end

  ##
  # Returns a string usable in Dir.glob to match all requirable paths
  # for this spec.

  def lib_dirs_glob
    dirs = if self.require_paths.size > 1 then
             "{#{self.require_paths.join(',')}}"
           else
             self.require_paths.first
           end

    "#{self.full_gem_path}/#{dirs}"
  end

  ##
  # Files in the Gem under one of the require_paths

  def lib_files
    @files.select do |file|
      require_paths.any? do |path|
        file.index(path) == 0
      end
    end
  end

  ##
  # Singular accessor for #licenses

  def license
    val = licenses and val.first
  end

  ##
  # Singular accessor for #licenses

  def license=o
    self.licenses = [o]
  end

  ##
  # The license(s) for the library.  Each license must be a short name, no
  # more than 64 characters.

  def licenses
    @licenses ||= []
  end

  ##
  # Set licenses to +licenses+, ensuring it is an array.

  def licenses= licenses
    @licenses = Array licenses
  end

  ##
  # Set the location a Specification was loaded from. +obj+ is converted
  # to a String.

  def loaded_from= path
    @loaded_from = path.to_s
  end

  ##
  # Sets the rubygems_version to the current RubyGems version.

  def mark_version
    @rubygems_version = Gem::VERSION
  end

  ##
  # Return all files in this gem that match for +glob+.

  def matches_for_glob glob # TODO: rename?
    # TODO: do we need these?? Kill it
    glob = File.join(self.lib_dirs_glob, glob)

    Dir[glob].map { |f| f.untaint } # FIX our tests are broken, run w/ SAFE=1
  end

  ##
  # Warn about unknown attributes while loading a spec.

  def method_missing(sym, *a, &b) # :nodoc:
    if @specification_version > CURRENT_SPECIFICATION_VERSION and
      sym.to_s =~ /=$/ then
      warn "ignoring #{sym} loading #{full_name}" if $DEBUG
    else
      super
    end
  end

  ##
  # Normalize the list of files so that:
  # * All file lists have redundancies removed.
  # * Files referenced in the extra_rdoc_files are included in the package
  #   file list.

  def normalize
    if defined?(@extra_rdoc_files) and @extra_rdoc_files then
      @extra_rdoc_files.uniq!
      @files ||= []
      @files.concat(@extra_rdoc_files)
    end

    @files            = @files.uniq if @files
    @extensions       = @extensions.uniq if @extensions
    @test_files       = @test_files.uniq if @test_files
    @executables      = @executables.uniq if @executables
    @extra_rdoc_files = @extra_rdoc_files.uniq if @extra_rdoc_files
  end

  ##
  # Returns the full name (name-version) of this gemspec using the original
  # platform.  For use with legacy gems.

  def original_name # :nodoc:
    if platform == Gem::Platform::RUBY or platform.nil? then
      "#{@name}-#{@version}"
    else
      "#{@name}-#{@version}-#{@original_platform}"
    end
  end

  ##
  # Cruft. Use +platform+.

  def original_platform # :nodoc:
    @original_platform ||= platform
  end

  ##
  # The platform this gem runs on.  See Gem::Platform for details.

  def platform
    @new_platform ||= Gem::Platform::RUBY
  end

  ##
  # The platform this gem runs on.  See Gem::Platform for details.
  #
  # Setting this to any value other than Gem::Platform::RUBY or
  # Gem::Platform::CURRENT is probably wrong.

  def platform= platform
    if @original_platform.nil? or
       @original_platform == Gem::Platform::RUBY then
      @original_platform = platform
    end

    case platform
    when Gem::Platform::CURRENT then
      @new_platform = Gem::Platform.local
      @original_platform = @new_platform.to_s

    when Gem::Platform then
      @new_platform = platform

    # legacy constants
    when nil, Gem::Platform::RUBY then
      @new_platform = Gem::Platform::RUBY
    when 'mswin32' then # was Gem::Platform::WIN32
      @new_platform = Gem::Platform.new 'x86-mswin32'
    when 'i586-linux' then # was Gem::Platform::LINUX_586
      @new_platform = Gem::Platform.new 'x86-linux'
    when 'powerpc-darwin' then # was Gem::Platform::DARWIN
      @new_platform = Gem::Platform.new 'ppc-darwin'
    else
      @new_platform = Gem::Platform.new platform
    end

    @platform = @new_platform.to_s

    @new_platform
  end

  def pretty_print(q) # :nodoc:
    q.group 2, 'Gem::Specification.new do |s|', 'end' do
      q.breakable

      # REFACTOR: each_attr - use in to_yaml as well
      @@attributes.each do |attr_name|
        current_value = self.send attr_name
        if current_value != default_value(attr_name) or
           self.class.required_attribute? attr_name then

          q.text "s.#{attr_name} = "

          if attr_name == :date then
            current_value = current_value.utc

            q.text "Time.utc(#{current_value.year}, #{current_value.month}, #{current_value.day})"
          else
            q.pp current_value
          end

          q.breakable
        end
      end
    end
  end

  ##
  # Check the spec for possible conflicts and freak out if there are any.

  def raise_if_conflicts
    other = Gem.loaded_specs[self.name]

    if other and self.version != other.version then
      # This gem is already loaded.  If the currently loaded gem is not in the
      # list of candidate gems, then we have a version conflict.

      msg = "can't activate #{full_name}, already activated #{other.full_name}"

      e = Gem::LoadError.new msg
      e.name = self.name
      # TODO: e.requirement = dep.requirement

      raise e
    end

    conf = self.conflicts

    unless conf.empty? then
      y = conf.map { |act,con|
        "#{act.full_name} conflicts with #{con.join(", ")}"
      }.join ", "

      # TODO: improve message by saying who activated `con`

      raise Gem::LoadError, "Unable to activate #{self.full_name}, because #{y}"
    end
  end

  ##
  # An ARGV style array of options to RDoc

  def rdoc_options
    @rdoc_options ||= []
  end

  ##
  # Sets rdoc_options to +value+, ensuring it is an array. Don't
  # use this, push onto the array instead.

  def rdoc_options= options
    # TODO: warn about setting instead of pushing
    @rdoc_options = Array options
  end

  ##
  # Singular accessor for #require_paths

  def require_path
    val = require_paths and val.first
  end

  ##
  # Singular accessor for #require_paths

  def require_path= path
    self.require_paths = [path]
  end

  ##
  # The version of ruby required by this gem

  def required_ruby_version= req
    @required_ruby_version = Gem::Requirement.create req
  end

  ##
  # The RubyGems version required by this gem

  def required_rubygems_version= req
    @required_rubygems_version = Gem::Requirement.create req
  end

  ##
  # An array or things required by this gem.  Not used by anything
  # presently.

  def requirements
    @requirements ||= []
  end

  ##
  # Set requirements to +req+, ensuring it is an array. Don't
  # use this, push onto the array instead.

  def requirements= req
    # TODO: warn about setting instead of pushing
    @requirements = Array req
  end

  ##
  # Returns the full path to this spec's ri directory.

  def ri_dir
    @ri_dir ||= File.join base_dir, 'ri', full_name
  end

  ##
  # Return a string containing a Ruby code representation of the given
  # object.

  def ruby_code(obj)
    case obj
    when String            then obj.dump
    when Array             then '[' + obj.map { |x| ruby_code x }.join(", ") + ']'
    when Gem::Version      then obj.to_s.dump
    when Date              then obj.strftime('%Y-%m-%d').dump
    when Time              then obj.strftime('%Y-%m-%d').dump
    when Numeric           then obj.inspect
    when true, false, nil  then obj.inspect
    when Gem::Platform     then "Gem::Platform.new(#{obj.to_a.inspect})"
    when Gem::Requirement  then "Gem::Requirement.new(#{obj.to_s.inspect})"
    else raise Gem::Exception, "ruby_code case not handled: #{obj.class}"
    end
  end

  private :ruby_code

  ##
  # List of dependencies that will automatically be activated at runtime.

  def runtime_dependencies
    dependencies.select { |d| d.type == :runtime }
  end

  ##
  # True if this gem has the same attributes as +other+.

  def same_attributes? spec
    @@attributes.all? { |name, default| self.send(name) == spec.send(name) }
  end

  private :same_attributes?

  ##
  # Checks if this specification meets the requirement of +dependency+.

  def satisfies_requirement? dependency
    return @name == dependency.name &&
      dependency.requirement.satisfied_by?(@version)
  end

  ##
  # Returns an object you can use to sort specifications in #sort_by.

  def sort_obj
    # TODO: this is horrible. Deprecate it.
    [@name, @version, @new_platform == Gem::Platform::RUBY ? -1 : 1]
  end

  ##
  # Returns the full path to the directory containing this spec's
  # gemspec file. eg: /usr/local/lib/ruby/gems/1.8/specifications

  def spec_dir
    @spec_dir ||= File.join base_dir, "specifications"
  end

  ##
  # Returns the full path to this spec's gemspec file.
  # eg: /usr/local/lib/ruby/gems/1.8/specifications/mygem-1.0.gemspec

  def spec_file
    @spec_file ||= File.join spec_dir, "#{full_name}.gemspec"
  end

  ##
  # The default name of the gemspec.  See also #file_name
  #
  #   spec.spec_name # => "example-1.0.gemspec"

  def spec_name
    "#{full_name}.gemspec"
  end

  ##
  # A short summary of this gem's description.

  def summary= str
    @summary = str.to_s.strip.
      gsub(/(\w-)\n[ \t]*(\w)/, '\1\2').gsub(/\n[ \t]*/, " ") # so. weird.
  end

  ##
  # Singular accessor for #test_files

  def test_file
    val = test_files and val.first
  end

  ##
  # Singular accessor for #test_files

  def test_file= file
    self.test_files = [file]
  end

  ##
  # Test files included in this gem.  You cannot append to this accessor, you
  # must assign to it.

  def test_files
    # Handle the possibility that we have @test_suite_file but not
    # @test_files.  This will happen when an old gem is loaded via
    # YAML.
    if defined? @test_suite_file then
      @test_files = [@test_suite_file].flatten
      @test_suite_file = nil
    end
    if defined?(@test_files) and @test_files then
      @test_files
    else
      @test_files = []
    end
  end

  ##
  # Set test_files to +files+, ensuring it is an array.

  def test_files= files
    @test_files = Array files
  end

  def test_suite_file # :nodoc:
    # TODO: deprecate
    test_files.first
  end

  def test_suite_file= file # :nodoc:
    # TODO: deprecate
    @test_files = [] unless defined? @test_files
    @test_files << file
  end

  ##
  # Returns a Ruby code representation of this specification, such that it can
  # be eval'ed and reconstruct the same specification later.  Attributes that
  # still have their default values are omitted.

  def to_ruby
    mark_version
    result = []
    result << "# -*- encoding: utf-8 -*-"
    result << nil
    result << "Gem::Specification.new do |s|"

    result << "  s.name = #{ruby_code name}"
    result << "  s.version = #{ruby_code version}"
    unless platform.nil? or platform == Gem::Platform::RUBY then
      result << "  s.platform = #{ruby_code original_platform}"
    end
    result << ""
    result << "  s.required_rubygems_version = #{ruby_code required_rubygems_version} if s.respond_to? :required_rubygems_version="

    handled = [
      :dependencies,
      :name,
      :platform,
      :required_rubygems_version,
      :specification_version,
      :version,
      :has_rdoc,
      :default_executable,
    ]

    @@attributes.each do |attr_name|
      next if handled.include? attr_name
      current_value = self.send(attr_name)
      if current_value != default_value(attr_name) or
         self.class.required_attribute? attr_name then
        result << "  s.#{attr_name} = #{ruby_code current_value}"
      end
    end

    result << nil
    result << "  if s.respond_to? :specification_version then"
    result << "    s.specification_version = #{specification_version}"
    result << nil

    result << "    if Gem::Version.new(Gem::VERSION) >= Gem::Version.new('1.2.0') then"

    dependencies.each do |dep|
      req = dep.requirements_list.inspect
      dep.instance_variable_set :@type, :runtime if dep.type.nil? # HACK
      result << "      s.add_#{dep.type}_dependency(%q<#{dep.name}>, #{req})"
    end

    result << "    else"

    dependencies.each do |dep|
      version_reqs_param = dep.requirements_list.inspect
      result << "      s.add_dependency(%q<#{dep.name}>, #{version_reqs_param})"
    end

    result << '    end'

    result << "  else"
      dependencies.each do |dep|
        version_reqs_param = dep.requirements_list.inspect
        result << "    s.add_dependency(%q<#{dep.name}>, #{version_reqs_param})"
      end
    result << "  end"

    result << "end"
    result << nil

    result.join "\n"
  end

  ##
  # Returns a Ruby lighter-weight code representation of this specification,
  # used for indexing only.
  #
  # See #to_ruby.

  def to_ruby_for_cache
    for_cache.to_ruby
  end

  def to_s # :nodoc:
    "#<Gem::Specification name=#{@name} version=#{@version}>"
  end

  def to_yaml(opts = {}) # :nodoc:
    if YAML.const_defined?(:ENGINE) && !YAML::ENGINE.syck? then
      super.gsub(/ !!null \n/, " \n")
    else
      YAML.quick_emit object_id, opts do |out|
        out.map taguri, to_yaml_style do |map|
          encode_with map
        end
      end
    end
  end

  ##
  # Recursively walk dependencies of this spec, executing the +block+ for each
  # hop.

  def traverse trail = [], &block
    trail = trail + [self]
    runtime_dependencies.each do |dep|
      dep.to_specs.each do |dep_spec|
        block[self, dep, dep_spec, trail + [dep_spec]]
        dep_spec.traverse(trail, &block) unless
          trail.map(&:name).include? dep_spec.name
      end
    end
  end

  ##
  # Checks that the specification contains all required fields, and does a
  # very basic sanity check.
  #
  # Raises InvalidSpecificationException if the spec does not pass the
  # checks..

  def validate packaging = true
    require 'rubygems/user_interaction'
    extend Gem::UserInteraction
    normalize

    nil_attributes = self.class.non_nil_attributes.find_all do |name|
      instance_variable_get("@#{name}").nil?
    end

    unless nil_attributes.empty? then
      raise Gem::InvalidSpecificationException,
        "#{nil_attributes.join ', '} must not be nil"
    end

    if packaging and rubygems_version != Gem::VERSION then
      raise Gem::InvalidSpecificationException,
            "expected RubyGems version #{Gem::VERSION}, was #{rubygems_version}"
    end

    @@required_attributes.each do |symbol|
      unless self.send symbol then
        raise Gem::InvalidSpecificationException,
              "missing value for attribute #{symbol}"
      end
    end

    unless String === name then
      raise Gem::InvalidSpecificationException,
            "invalid value for attribute name: \"#{name.inspect}\""
    end

    if require_paths.empty? then
      raise Gem::InvalidSpecificationException,
            'specification must have at least one require_path'
    end

    @files.delete_if            { |x| File.directory?(x) }
    @test_files.delete_if       { |x| File.directory?(x) }
    @executables.delete_if      { |x| File.directory?(File.join(@bindir, x)) }
    @extra_rdoc_files.delete_if { |x| File.directory?(x) }
    @extensions.delete_if       { |x| File.directory?(x) }

    non_files = files.reject { |x| File.file?(x) }

    unless not packaging or non_files.empty? then
      raise Gem::InvalidSpecificationException,
            "[\"#{non_files.join "\", \""}\"] are not files"
    end

    unless specification_version.is_a?(Fixnum)
      raise Gem::InvalidSpecificationException,
            'specification_version must be a Fixnum (did you mean version?)'
    end

    case platform
    when Gem::Platform, Gem::Platform::RUBY then # ok
    else
      raise Gem::InvalidSpecificationException,
            "invalid platform #{platform.inspect}, see Gem::Platform"
    end

    self.class.array_attributes.each do |field|
      val = self.send field
      klass = case field
              when :dependencies
                Gem::Dependency
              else
                String
              end

      unless Array === val and val.all? { |x| x.kind_of?(klass) } then
        raise(Gem::InvalidSpecificationException,
              "#{field} must be an Array of #{klass}")
      end
    end

    [:authors].each do |field|
      val = self.send field
      raise Gem::InvalidSpecificationException, "#{field} may not be empty" if
        val.empty?
    end

    licenses.each { |license|
      if license.length > 64
        raise Gem::InvalidSpecificationException,
          "each license must be 64 characters or less"
      end
    }

    # reject lazy developers:

    lazy = '"FIxxxXME" or "TOxxxDO"'.gsub(/xxx/, '')

    unless authors.grep(/FI XME|TO DO/x).empty? then
      raise Gem::InvalidSpecificationException, "#{lazy} is not an author"
    end

    unless Array(email).grep(/FI XME|TO DO/x).empty? then
      raise Gem::InvalidSpecificationException, "#{lazy} is not an email"
    end

    if description =~ /FI XME|TO DO/x then
      raise Gem::InvalidSpecificationException, "#{lazy} is not a description"
    end

    if summary =~ /FI XME|TO DO/x then
      raise Gem::InvalidSpecificationException, "#{lazy} is not a summary"
    end

    if homepage and not homepage.empty? and
       homepage !~ /\A[a-z][a-z\d+.-]*:/i then
      raise Gem::InvalidSpecificationException,
            "\"#{homepage}\" is not a URI"
    end

    # Warnings

    %w[author description email homepage summary].each do |attribute|
      value = self.send attribute
      alert_warning "no #{attribute} specified" if value.nil? or value.empty?
    end

    if description == summary then
      alert_warning 'description and summary are identical'
    end

    # TODO: raise at some given date
    alert_warning "deprecated autorequire specified" if autorequire

    executables.each do |executable|
      executable_path = File.join(bindir, executable)
      shebang = File.read(executable_path, 2) == '#!'

      alert_warning "#{executable_path} is missing #! line" unless shebang
    end

    true
  end

  ##
  # Set the version to +version+, potentially also setting
  # required_rubygems_version if +version+ indicates it is a
  # prerelease.

  def version= version
    @version = Gem::Version.create(version)
    self.required_rubygems_version = '> 1.3.1' if @version.prerelease?
    return @version
  end

  # FIX: have this handle the platform/new_platform/original_platform bullshit
  def yaml_initialize(tag, vals) # :nodoc:
    vals.each do |ivar, val|
      instance_variable_set "@#{ivar}", val
    end

    @original_platform = @platform # for backwards compatibility
    self.platform = Gem::Platform.new @platform
  end

  extend Gem::Deprecate

  deprecate :test_suite_file,     :test_file,  2011, 10
  deprecate :test_suite_file=,    :test_file=, 2011, 10
  deprecate :loaded,              :activated,  2011, 10
  deprecate :loaded?,             :activated?, 2011, 10
  deprecate :loaded=,             :activated=, 2011, 10
  deprecate :installation_path,   :base_dir,   2011, 10
  deprecate :cache_gem,           :cache_file, 2011, 10
  # TODO:
  # deprecate :has_rdoc,            :none,       2011, 10
  # deprecate :has_rdoc?,           :none,       2011, 10
  # deprecate :has_rdoc=,           :none,       2011, 10
  # deprecate :default_executable,  :none,       2011, 10
  # deprecate :default_executable=, :none,       2011, 10
  # deprecate :spec_name,           :spec_file,  2011, 10
  # deprecate :file_name,           :cache_file, 2011, 10
  # deprecate :full_gem_path,     :cache_file, 2011, 10
end

Gem.clear_paths

