require 'rubygems/command'
require 'rubygems/format'
require 'rubygems/installer'
require 'rubygems/version_option'

class Gem::Commands::PristineCommand < Gem::Command

  include Gem::VersionOption

  def initialize
    super 'pristine',
          'Restores installed gems to pristine condition from files located in the gem cache',
          :version => Gem::Requirement.default, :extensions => true,
          :all => false

    add_option('--all',
               'Restore all installed gems to pristine',
               'condition') do |value, options|
      options[:all] = value
    end

    add_option('--[no-]extensions',
               'Restore gems with extensions') do |value, options|
      options[:extensions] = value
    end

    add_version_option('restore to', 'pristine condition')
  end

  def arguments # :nodoc:
    "GEMNAME       gem to restore to pristine condition (unless --all)"
  end

  def defaults_str # :nodoc:
    "--all --extensions"
  end

  def description # :nodoc:
    <<-EOF
The pristine command compares the installed gems with the contents of the
cached gem and restores any files that don't match the cached gem's copy.

If you have made modifications to your installed gems, the pristine command
will revert them.  After all the gem's files have been checked all bin stubs
for the gem are regenerated.

If the cached gem cannot be found, you will need to use `gem install` to
revert the gem.

If --no-extensions is provided pristine will not attempt to restore gems with
extensions.
    EOF
  end

  def usage # :nodoc:
    "#{program_name} [args]"
  end

  def execute
    specs = if options[:all] then
              Gem::Specification.map
            else
              get_all_gem_names.map do |gem_name|
                Gem::Specification.find_all_by_name gem_name, options[:version]
              end.flatten
            end

    if specs.to_a.empty? then
      raise Gem::Exception,
            "Failed to find gems #{options[:args]} #{options[:version]}"
    end

    install_dir = Gem.dir # TODO use installer option

    raise Gem::FilePermissionError.new(install_dir) unless
      File.writable?(install_dir)

    say "Restoring gems to pristine condition..."

    specs.each do |spec|
      unless spec.extensions.empty? or options[:extensions] then
        say "Skipped #{spec.full_name}, it needs to compile an extension"
        next
      end

      gem = spec.cache_file

      unless File.exist? gem then
        require 'rubygems/remote_fetcher'

        say "Cached gem for #{spec.full_name} not found, attempting to fetch..."
        dep = Gem::Dependency.new spec.name, spec.version
        Gem::RemoteFetcher.fetcher.download_to_cache dep
      end

      # TODO use installer options
      installer = Gem::Installer.new(gem,
                                     :wrappers => true,
                                     :force => true,
                                     :install_dir => spec.base_dir)
      installer.install

      say "Restored #{spec.full_name}"
    end
  end
end
