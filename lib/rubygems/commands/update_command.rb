require 'rubygems/command'
require 'rubygems/command_manager'
require 'rubygems/install_update_options'
require 'rubygems/local_remote_options'
require 'rubygems/spec_fetcher'
require 'rubygems/version_option'
require 'rubygems/commands/install_command'

class Gem::Commands::UpdateCommand < Gem::Command

  include Gem::InstallUpdateOptions
  include Gem::LocalRemoteOptions
  include Gem::VersionOption

  def initialize
    super 'update',
          'Update the named gems (or all installed gems) in the local repository',
      :generate_rdoc => true,
      :generate_ri   => true,
      :force         => false

    add_install_update_options

    OptionParser.accept Gem::Version do |value|
      Gem::Version.new value

      value
    end

    add_option('--system [VERSION]', Gem::Version,
               'Update the RubyGems system software') do |value, options|
      value = true unless value

      options[:system] = value
    end

    add_local_remote_options
    add_platform_option
    add_prerelease_option "as update targets"
  end

  def arguments # :nodoc:
    "GEMNAME       name of gem to update"
  end

  def defaults_str # :nodoc:
    "--rdoc --ri --no-force --install-dir #{Gem.dir}"
  end

  def usage # :nodoc:
    "#{program_name} GEMNAME [GEMNAME ...]"
  end

  def execute
    @installer = Gem::DependencyInstaller.new options
    @updated   = []

    hig = {}

    if options[:system] then
      update_rubygems
      return
    else
      say "Updating installed gems"

      hig = {} # highest installed gems

      Gem::Specification.each do |spec|
        if hig[spec.name].nil? or hig[spec.name].version < spec.version then
          hig[spec.name] = spec
        end
      end
    end

    gems_to_update = which_to_update hig, options[:args].uniq

    updated = update_gems gems_to_update

    if updated.empty? then
      say "Nothing to update"
    else
      say "Gems updated: #{updated.map { |spec| spec.name }.join ', '}"

      if options[:generate_ri] then
        updated.each do |gem|
          Gem::DocManager.new(gem, options[:rdoc_args]).generate_ri
        end

        Gem::DocManager.update_ri_cache
      end

      if options[:generate_rdoc] then
        updated.each do |gem|
          Gem::DocManager.new(gem, options[:rdoc_args]).generate_rdoc
        end
      end
    end
  end

  def update_gem name, version = Gem::Requirement.default
    return if @updated.any? { |spec| spec.name == name }
    success = false

    say "Updating #{name}"
    begin
      @installer.install name, version
      success = true
    rescue Gem::InstallError => e
      alert_error "Error installing #{name}:\n\t#{e.message}"
      success = false
    end

    @installer.installed_gems.each do |spec|
      @updated << spec
      say "Successfully installed #{spec.full_name}" if success
    end
  end

  def update_gems gems_to_update
    gems_to_update.uniq.sort.each do |(name, version)|
      update_gem name, version
    end

    @updated
  end

  ##
  # Update RubyGems software to the latest version.

  def update_rubygems
    unless options[:args].empty? then
      alert_error "Gem names are not allowed with the --system option"
      terminate_interaction 1
    end

    options[:user_install] = false

    # TODO: rename version and other variable name conflicts
    # TODO: get rid of all this indirection on name and other BS

    version = options[:system]
    if version == true then
      version     = Gem::Version.new     Gem::VERSION
      requirement = Gem::Requirement.new ">= #{Gem::VERSION}"
    else
      version     = Gem::Version.new     version
      requirement = Gem::Requirement.new version
    end

    rubygems_update         = Gem::Specification.new
    rubygems_update.name    = 'rubygems-update'
    rubygems_update.version = version

    hig = {
      'rubygems-update' => rubygems_update
    }

    gems_to_update = which_to_update hig, options[:args], :system
    name, up_ver   = gems_to_update.first
    current_ver    = Gem::Version.new Gem::VERSION

    target = if options[:system] == true then
               up_ver
             else
               version
             end

    if current_ver == target then
      # if options[:system] != true and version == current_ver then
      say "Latest version currently installed. Aborting."
      terminate_interaction
    end

    update_gem name, target

    installed_gems = Gem::Specification.find_all_by_name 'rubygems-update', requirement
    version        = installed_gems.last.version

    args = []
    args << '--prefix' << Gem.prefix if Gem.prefix
    args << '--no-rdoc' unless options[:generate_rdoc]
    args << '--no-ri' unless options[:generate_ri]
    args << '--no-format-executable' if options[:no_format_executable]

    update_dir = File.join Gem.dir, 'gems', "rubygems-update-#{version}"

    Dir.chdir update_dir do
      say "Installing RubyGems #{version}"
      setup_cmd = "#{Gem.ruby} setup.rb #{args.join ' '}"

      # Make sure old rubygems isn't loaded
      old = ENV["RUBYOPT"]
      ENV.delete("RUBYOPT") if old
      installed = system setup_cmd
      say "RubyGems system software updated" if installed
      ENV["RUBYOPT"] = old if old
    end
  end

  def which_to_update highest_installed_gems, gem_names, system = false
    result = []

    highest_installed_gems.each do |l_name, l_spec|
      next if not gem_names.empty? and
              gem_names.all? { |name| /#{name}/ !~ l_spec.name }

      dependency = Gem::Dependency.new l_spec.name, "> #{l_spec.version}"

      fetcher = Gem::SpecFetcher.fetcher
      spec_tuples = fetcher.find_matching dependency

      matching_gems = spec_tuples.select do |(name, _, platform),|
        name == l_name and Gem::Platform.match platform
      end

      highest_remote_gem = matching_gems.sort_by do |(_, version),|
        version
      end.last

      highest_remote_gem ||= [[nil, Gem::Version.new(0), nil]] # "null" object
      highest_remote_ver = highest_remote_gem.first[1]

      if system or (l_spec.version < highest_remote_ver) then
        result << [l_spec.name, [l_spec.version, highest_remote_ver].max]
      end
    end

    result
  end

end

