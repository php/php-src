require 'rubygems'
require 'rubygems/dependency_list'
require 'rubygems/installer'
require 'rubygems/spec_fetcher'
require 'rubygems/user_interaction'

##
# Installs a gem along with all its dependencies from local and remote gems.

class Gem::DependencyInstaller

  include Gem::UserInteraction

  attr_reader :gems_to_install
  attr_reader :installed_gems

  DEFAULT_OPTIONS = {
    :env_shebang         => false,
    :domain              => :both, # HACK dup
    :force               => false,
    :format_executable   => false, # HACK dup
    :ignore_dependencies => false,
    :prerelease          => false,
    :security_policy     => nil, # HACK NoSecurity requires OpenSSL. AlmostNo? Low?
    :wrappers            => true,
  }

  ##
  # Creates a new installer instance.
  #
  # Options are:
  # :cache_dir:: Alternate repository path to store .gem files in.
  # :domain:: :local, :remote, or :both.  :local only searches gems in the
  #           current directory.  :remote searches only gems in Gem::sources.
  #           :both searches both.
  # :env_shebang:: See Gem::Installer::new.
  # :force:: See Gem::Installer#install.
  # :format_executable:: See Gem::Installer#initialize.
  # :ignore_dependencies:: Don't install any dependencies.
  # :install_dir:: See Gem::Installer#install.
  # :prerelease:: Allow prerelease versions.  See #install.
  # :security_policy:: See Gem::Installer::new and Gem::Security.
  # :user_install:: See Gem::Installer.new
  # :wrappers:: See Gem::Installer::new

  def initialize(options = {})
    if options[:install_dir] then
      @gem_home = options[:install_dir]

      Gem::Specification.dirs = @gem_home
      Gem.ensure_gem_subdirectories @gem_home
      options[:install_dir] = @gem_home # FIX: because we suck and reuse below
    end

    options = DEFAULT_OPTIONS.merge options

    @bin_dir             = options[:bin_dir]
    @development         = options[:development]
    @domain              = options[:domain]
    @env_shebang         = options[:env_shebang]
    @force               = options[:force]
    @format_executable   = options[:format_executable]
    @ignore_dependencies = options[:ignore_dependencies]
    @prerelease          = options[:prerelease]
    @security_policy     = options[:security_policy]
    @user_install        = options[:user_install]
    @wrappers            = options[:wrappers]

    @installed_gems = []

    @install_dir = options[:install_dir] || Gem.dir
    @cache_dir = options[:cache_dir] || @install_dir

    # Set with any errors that SpecFetcher finds while search through
    # gemspecs for a dep
    @errors = nil
  end

  ##
  # Returns a list of pairs of gemspecs and source_uris that match
  # Gem::Dependency +dep+ from both local (Dir.pwd) and remote (Gem.sources)
  # sources.  Gems are sorted with newer gems preferred over older gems, and
  # local gems preferred over remote gems.

  def find_gems_with_sources(dep)
    # Reset the errors
    @errors = nil
    gems_and_sources = []

    if @domain == :both or @domain == :local then
      Dir[File.join(Dir.pwd, "#{dep.name}-[0-9]*.gem")].each do |gem_file|
        spec = Gem::Format.from_file_by_path(gem_file).spec
        gems_and_sources << [spec, gem_file] if spec.name == dep.name
      end
    end

    if @domain == :both or @domain == :remote then
      begin
        # REFACTOR: all = dep.requirement.needs_all?
        requirements = dep.requirement.requirements.map do |req, ver|
          req
        end

        all = !dep.prerelease? &&
              # we only need latest if there's one requirement and it is
              # guaranteed to match the newest specs
              (requirements.length > 1 or
                (requirements.first != ">=" and requirements.first != ">"))

        found, @errors = Gem::SpecFetcher.fetcher.fetch_with_errors dep, all, true, dep.prerelease?

        gems_and_sources.push(*found)

      rescue Gem::RemoteFetcher::FetchError => e
        if Gem.configuration.really_verbose then
          say "Error fetching remote data:\t\t#{e.message}"
          say "Falling back to local-only install"
        end
        @domain = :local
      end
    end

    gems_and_sources.sort_by do |gem, source|
      [gem, source =~ /^http:\/\// ? 0 : 1] # local gems win
    end
  end

  ##
  # Gathers all dependencies necessary for the installation from local and
  # remote sources unless the ignore_dependencies was given.

  def gather_dependencies
    specs = @specs_and_sources.map { |spec,_| spec }

    # these gems were listed by the user, always install them
    keep_names = specs.map { |spec| spec.full_name }

    dependency_list = Gem::DependencyList.new @development
    dependency_list.add(*specs)
    to_do = specs.dup

    add_found_dependencies to_do, dependency_list unless @ignore_dependencies

    dependency_list.specs.reject! { |spec|
      not keep_names.include?(spec.full_name) and
      Gem::Specification.include?(spec)
    }

    unless dependency_list.ok? or @ignore_dependencies or @force then
      reason = dependency_list.why_not_ok?.map { |k,v|
        "#{k} requires #{v.join(", ")}"
      }.join("; ")
      raise Gem::DependencyError, "Unable to resolve dependencies: #{reason}"
    end

    @gems_to_install = dependency_list.dependency_order.reverse
  end

  def add_found_dependencies to_do, dependency_list
    seen = {}
    dependencies = Hash.new { |h, name| h[name] = Gem::Dependency.new name }

    until to_do.empty? do
      spec = to_do.shift
      next if spec.nil? or seen[spec.name]
      seen[spec.name] = true

      deps = spec.runtime_dependencies
      deps |= spec.development_dependencies if @development

      deps.each do |dep|
        dependencies[dep.name] = dependencies[dep.name].merge dep

        results = find_gems_with_sources(dep).reverse

        results.reject! do |dep_spec,|
          to_do.push dep_spec

          # already locally installed
          Gem::Specification.any? do |installed_spec|
            dep.name == installed_spec.name and
              dep.requirement.satisfied_by? installed_spec.version
          end
        end

        results.each do |dep_spec, source_uri|
          @specs_and_sources << [dep_spec, source_uri]

          dependency_list.add dep_spec
        end
      end
    end

    dependency_list.remove_specs_unsatisfied_by dependencies
  end

  ##
  # Finds a spec and the source_uri it came from for gem +gem_name+ and
  # +version+.  Returns an Array of specs and sources required for
  # installation of the gem.

  def find_spec_by_name_and_version(gem_name,
                                    version = Gem::Requirement.default,
                                    prerelease = false)
    spec_and_source = nil

    glob = if File::ALT_SEPARATOR then
             gem_name.gsub File::ALT_SEPARATOR, File::SEPARATOR
           else
             gem_name
           end

    local_gems = Dir["#{glob}*"].sort.reverse

    local_gems.each do |gem_file|
      next unless gem_file =~ /gem$/
      begin
        spec = Gem::Format.from_file_by_path(gem_file).spec
        spec_and_source = [spec, gem_file]
        break
      rescue SystemCallError, Gem::Package::FormatError
      end
    end

    unless spec_and_source then
      dep = Gem::Dependency.new gem_name, version
      dep.prerelease = true if prerelease
      spec_and_sources = find_gems_with_sources(dep).reverse
      spec_and_source = spec_and_sources.find { |spec, source|
        Gem::Platform.match spec.platform
      }
    end

    if spec_and_source.nil? then
      raise Gem::GemNotFoundException.new(
        "Could not find a valid gem '#{gem_name}' (#{version}) locally or in a repository",
        gem_name, version, @errors)
    end

    @specs_and_sources = [spec_and_source]
  end

  ##
  # Installs the gem +dep_or_name+ and all its dependencies.  Returns an Array
  # of installed gem specifications.
  #
  # If the +:prerelease+ option is set and there is a prerelease for
  # +dep_or_name+ the prerelease version will be installed.
  #
  # Unless explicitly specified as a prerelease dependency, prerelease gems
  # that +dep_or_name+ depend on will not be installed.
  #
  # If c-1.a depends on b-1 and a-1.a and there is a gem b-1.a available then
  # c-1.a, b-1 and a-1.a will be installed.  b-1.a will need to be installed
  # separately.

  def install dep_or_name, version = Gem::Requirement.default
    if String === dep_or_name then
      find_spec_by_name_and_version dep_or_name, version, @prerelease
    else
      dep_or_name.prerelease = @prerelease
      @specs_and_sources = [find_gems_with_sources(dep_or_name).last]
    end

    @installed_gems = []

    gather_dependencies

    last = @gems_to_install.size - 1
    @gems_to_install.each_with_index do |spec, index|
      next if Gem::Specification.include?(spec) and index != last

      # TODO: make this sorta_verbose so other users can benefit from it
      say "Installing gem #{spec.full_name}" if Gem.configuration.really_verbose

      _, source_uri = @specs_and_sources.assoc spec
      begin
        local_gem_path = Gem::RemoteFetcher.fetcher.download spec, source_uri,
                                                             @cache_dir
      rescue Gem::RemoteFetcher::FetchError
        next if @force
        raise
      end

      inst = Gem::Installer.new local_gem_path,
                                :bin_dir             => @bin_dir,
                                :development         => @development,
                                :env_shebang         => @env_shebang,
                                :force               => @force,
                                :format_executable   => @format_executable,
                                :ignore_dependencies => @ignore_dependencies,
                                :install_dir         => @install_dir,
                                :security_policy     => @security_policy,
                                :user_install        => @user_install,
                                :wrappers            => @wrappers

      spec = inst.install

      @installed_gems << spec
    end

    @installed_gems
  end
end
