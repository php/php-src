require 'rubygems/command'
require 'rubygems/local_remote_options'
require 'rubygems/spec_fetcher'
require 'rubygems/version_option'

class Gem::Commands::OutdatedCommand < Gem::Command

  include Gem::LocalRemoteOptions
  include Gem::VersionOption

  def initialize
    super 'outdated', 'Display all gems that need updates'

    add_local_remote_options
    add_platform_option
  end

  def execute
    Gem::Specification.outdated.sort.each do |name|
      local   = Gem::Specification.find_all_by_name(name).max
      dep     = Gem::Dependency.new local.name, ">= #{local.version}"
      remotes = Gem::SpecFetcher.fetcher.fetch dep

      next if remotes.empty?

      remote = remotes.last.first
      say "#{local.name} (#{local.version} < #{remote.version})"
    end
  end
end
