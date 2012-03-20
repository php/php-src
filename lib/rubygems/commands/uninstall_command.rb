require 'rubygems/command'
require 'rubygems/version_option'
require 'rubygems/uninstaller'

##
# Gem uninstaller command line tool
#
# See `gem help uninstall`

class Gem::Commands::UninstallCommand < Gem::Command

  include Gem::VersionOption

  def initialize
    super 'uninstall', 'Uninstall gems from the local repository',
          :version => Gem::Requirement.default, :user_install => true

    add_option('-a', '--[no-]all',
      'Uninstall all matching versions'
      ) do |value, options|
      options[:all] = value
    end

    add_option('-I', '--[no-]ignore-dependencies',
               'Ignore dependency requirements while',
               'uninstalling') do |value, options|
      options[:ignore] = value
    end

    add_option('-x', '--[no-]executables',
                 'Uninstall applicable executables without',
                 'confirmation') do |value, options|
      options[:executables] = value
    end

    add_option('-i', '--install-dir DIR',
               'Directory to uninstall gem from') do |value, options|
      options[:install_dir] = File.expand_path(value)
    end

    add_option('-n', '--bindir DIR',
               'Directory to remove binaries from') do |value, options|
      options[:bin_dir] = File.expand_path(value)
    end

    add_option('--[no-]user-install',
               'Uninstall from user\'s home directory',
               'in addition to GEM_HOME.') do |value, options|
      options[:user_install] = value
    end

    add_option('--[no-]format-executable',
               'Assume executable names match Ruby\'s prefix and suffix.') do |value, options|
      options[:format_executable] = value
    end

    add_version_option
    add_platform_option
  end

  def arguments # :nodoc:
    "GEMNAME       name of gem to uninstall"
  end

  def defaults_str # :nodoc:
    "--version '#{Gem::Requirement.default}' --no-force " \
    "--install-dir #{Gem.dir}\n" \
    "--user-install"
  end

  def usage # :nodoc:
    "#{program_name} GEMNAME [GEMNAME ...]"
  end

  def execute
    original_path = Gem.path

    get_all_gem_names.each do |gem_name|
      begin
        Gem::Uninstaller.new(gem_name, options).uninstall
      rescue Gem::InstallError => e
        alert e.message
      rescue Gem::GemNotInHomeException => e
        spec = e.spec
        alert("In order to remove #{spec.name}, please execute:\n" \
              "\tgem uninstall #{spec.name} --install-dir=#{spec.installation_path}")
      ensure
        Gem.use_paths(*original_path)
      end
    end
  end

end

