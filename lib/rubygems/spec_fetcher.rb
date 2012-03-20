require 'rubygems/remote_fetcher'
require 'rubygems/user_interaction'
require 'rubygems/errors'
require 'rubygems/text'

##
# SpecFetcher handles metadata updates from remote gem repositories.

class Gem::SpecFetcher

  include Gem::UserInteraction
  include Gem::Text

  FILES = {
    :all        => 'specs',
    :latest     => 'latest_specs',
    :prerelease => 'prerelease_specs',
  }

  ##
  # The SpecFetcher cache dir.

  attr_reader :dir # :nodoc:

  ##
  # Cache of latest specs

  attr_reader :latest_specs # :nodoc:

  ##
  # Cache of all released specs

  attr_reader :specs # :nodoc:

  ##
  # Cache of prerelease specs

  attr_reader :prerelease_specs # :nodoc:

  @fetcher = nil

  def self.fetcher
    @fetcher ||= new
  end

  def self.fetcher=(fetcher) # :nodoc:
    @fetcher = fetcher
  end

  def initialize
    require 'fileutils'

    @dir = File.join Gem.user_home, '.gem', 'specs'
    @update_cache = File.stat(Gem.user_home).uid == Process.uid

    @specs = {}
    @latest_specs = {}
    @prerelease_specs = {}

    @caches = {
      :latest => @latest_specs,
      :prerelease => @prerelease_specs,
      :all => @specs
    }

    @fetcher = Gem::RemoteFetcher.fetcher
  end

  ##
  # Returns the local directory to write +uri+ to.

  def cache_dir(uri)
    # Correct for windows paths
    escaped_path = uri.path.sub(/^\/([a-z]):\//i, '/\\1-/')
    File.join @dir, "#{uri.host}%#{uri.port}", File.dirname(escaped_path)
  end

  ##
  # Fetch specs matching +dependency+.  If +all+ is true, all matching
  # (released) versions are returned.  If +matching_platform+ is
  # false, all platforms are returned. If +prerelease+ is true,
  # prerelease versions are included.

  def fetch_with_errors(dependency,
                        all               = false,
                        matching_platform = true,
                        prerelease        = false)

    specs_and_sources, errors = find_matching_with_errors(dependency,
                                                          all,
                                                          matching_platform,
                                                          prerelease)

    ss = specs_and_sources.map do |spec_tuple, source_uri|
      [fetch_spec(spec_tuple, URI.parse(source_uri)), source_uri]
    end

    return [ss, errors]
  end

  def fetch(*args)
    fetch_with_errors(*args).first
  end

  def fetch_spec(spec, source_uri)
    source_uri = URI.parse source_uri if String === source_uri
    spec = spec - [nil, 'ruby', '']
    spec_file_name = "#{spec.join '-'}.gemspec"

    uri = source_uri + "#{Gem::MARSHAL_SPEC_DIR}#{spec_file_name}"

    cache_dir = cache_dir uri

    local_spec = File.join cache_dir, spec_file_name

    if File.exist? local_spec then
      spec = Gem.read_binary local_spec
    else
      uri.path << '.rz'

      spec = @fetcher.fetch_path uri
      spec = Gem.inflate spec

      if @update_cache then
        FileUtils.mkdir_p cache_dir

        open local_spec, 'wb' do |io|
          io.write spec
        end
      end
    end

    # TODO: Investigate setting Gem::Specification#loaded_from to a URI
    Marshal.load spec
  end

  ##
  # Find spec names that match +dependency+.  If +all+ is true, all
  # matching released versions are returned.  If +matching_platform+
  # is false, gems for all platforms are returned.

  def find_matching_with_errors(dependency,
                                all               = false,
                                matching_platform = true,
                                prerelease        = false)
    found = {}

    rejected_specs = {}

    list(all, prerelease).each do |source_uri, specs|
      found[source_uri] = specs.select do |spec_name, version, spec_platform|
        if dependency.match?(spec_name, version)
          if matching_platform and !Gem::Platform.match(spec_platform)
            pm = (rejected_specs[dependency] ||= Gem::PlatformMismatch.new(spec_name, version))
            pm.add_platform spec_platform
            false
          else
            true
          end
        end
      end
    end

    errors = rejected_specs.values

    specs_and_sources = []

    found.each do |source_uri, specs|
      uri_str = source_uri.to_s
      specs_and_sources.concat(specs.map { |spec| [spec, uri_str] })
    end

    [specs_and_sources, errors]
  end

  def find_matching(*args)
    find_matching_with_errors(*args).first
  end

  ##
  # Suggests a gem based on the supplied +gem_name+. Returns a string
  # of the gem name if an approximate match can be found or nil
  # otherwise. NOTE: for performance reasons only gems which exactly
  # match the first character of +gem_name+ are considered.

  def suggest_gems_from_name gem_name
    gem_name        = gem_name.downcase
    max             = gem_name.size / 2
    specs           = list.values.flatten 1

    matches = specs.map { |name, version, platform|
      next unless Gem::Platform.match platform

      distance = levenshtein_distance gem_name, name.downcase

      next if distance >= max

      return [name] if distance == 0

      [name, distance]
    }.compact

    matches = matches.uniq.sort_by { |name, dist| dist }

    matches.first(5).map { |name, dist| name }
  end

  ##
  # Returns a list of gems available for each source in Gem::sources.  If
  # +all+ is true, all released versions are returned instead of only latest
  # versions. If +prerelease+ is true, include prerelease versions.

  def list(all = false, prerelease = false)
    # TODO: make type the only argument
    type = if all
             :all
           elsif prerelease
             :prerelease
           else
             :latest
           end

    list  = {}
    file  = FILES[type]
    cache = @caches[type]

    Gem.sources.each do |source_uri|
      source_uri = URI.parse source_uri

      unless cache.include? source_uri
        cache[source_uri] = load_specs source_uri, file
      end

      list[source_uri] = cache[source_uri]
    end

    if type == :all
      list.values.map do |gems|
        gems.reject! { |g| !g[1] || g[1].prerelease? }
      end
    end

    list
  end

  ##
  # Loads specs in +file+, fetching from +source_uri+ if the on-disk cache is
  # out of date.

  def load_specs(source_uri, file)
    file_name  = "#{file}.#{Gem.marshal_version}"
    spec_path  = source_uri + "#{file_name}.gz"
    cache_dir  = cache_dir spec_path
    local_file = File.join(cache_dir, file_name)
    loaded     = false

    if File.exist? local_file then
      spec_dump =
        @fetcher.fetch_path(spec_path, File.mtime(local_file)) rescue nil

      loaded = true if spec_dump

      spec_dump ||= Gem.read_binary local_file
    else
      spec_dump = @fetcher.fetch_path spec_path
      loaded = true
    end

    specs = begin
              Marshal.load spec_dump
            rescue ArgumentError
              spec_dump = @fetcher.fetch_path spec_path
              loaded = true

              Marshal.load spec_dump
            end

    if loaded and @update_cache then
      begin
        FileUtils.mkdir_p cache_dir

        open local_file, 'wb' do |io|
          io << spec_dump
        end
      rescue
      end
    end

    specs
  end

end

