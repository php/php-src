require 'rubygems/command'
require 'rubygems/local_remote_options'
require 'rubygems/version_option'

class Gem::Commands::FetchCommand < Gem::Command

  include Gem::LocalRemoteOptions
  include Gem::VersionOption

  def initialize
    super 'fetch', 'Download a gem and place it in the current directory'

    add_bulk_threshold_option
    add_proxy_option
    add_source_option

    add_version_option
    add_platform_option
    add_prerelease_option
  end

  def arguments # :nodoc:
    'GEMNAME       name of gem to download'
  end

  def defaults_str # :nodoc:
    "--version '#{Gem::Requirement.default}'"
  end

  def usage # :nodoc:
    "#{program_name} GEMNAME [GEMNAME ...]"
  end

  def execute
    version = options[:version] || Gem::Requirement.default
    all = Gem::Requirement.default != version

    platform  = Gem.platforms.last
    gem_names = get_all_gem_names

    gem_names.each do |gem_name|
      dep = Gem::Dependency.new gem_name, version
      dep.prerelease = options[:prerelease]

      specs_and_sources, errors =
        Gem::SpecFetcher.fetcher.fetch_with_errors(dep, all, true,
                                                   dep.prerelease?)

      if platform then
        filtered = specs_and_sources.select { |s,| s.platform == platform }
        specs_and_sources = filtered unless filtered.empty?
      end

      spec, source_uri = specs_and_sources.sort_by { |s,| s.version }.last

      if spec.nil? then
        show_lookup_failure gem_name, version, errors, options[:domain]
        next
      end

      path = Gem::RemoteFetcher.fetcher.download spec, source_uri
      FileUtils.mv path, File.basename(spec.cache_file)

      say "Downloaded #{spec.full_name}"
    end
  end

end

