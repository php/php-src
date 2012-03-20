require 'rdoc/ri'

##
# The directories where ri data lives.

module RDoc::RI::Paths

  #:stopdoc:
  require 'rbconfig'

  version = RbConfig::CONFIG['ruby_version']

  base    = if RbConfig::CONFIG.key? 'ridir' then
              File.join RbConfig::CONFIG['ridir'], version
            else
              File.join RbConfig::CONFIG['datadir'], 'ri', version
            end

  SYSDIR  = File.join base, "system"
  SITEDIR = File.join base, "site"

  homedir = begin
              File.expand_path('~')
            rescue ArgumentError
            end

  homedir ||= ENV['HOME'] ||
              ENV['USERPROFILE'] || ENV['HOMEPATH'] # for 1.8 compatibility

  HOMEDIR = if homedir then
              File.join homedir, ".rdoc"
            end
  #:startdoc:

  @gemdirs = nil

  ##
  # Iterates over each selected path yielding the directory and type.
  #
  # Yielded types:
  # :system:: Where Ruby's ri data is stored.  Yielded when +system+ is
  #           true
  # :site:: Where ri for installed libraries are stored.  Yielded when
  #         +site+ is true.  Normally no ri data is stored here.
  # :home:: ~/.rdoc.  Yielded when +home+ is true.
  # :gem:: ri data for an installed gem.  Yielded when +gems+ is true.
  # :extra:: ri data directory from the command line.  Yielded for each
  #          entry in +extra_dirs+

  def self.each system, site, home, gems, *extra_dirs # :yields: directory, type
    extra_dirs.each do |dir|
      yield dir, :extra
    end

    yield SYSDIR,  :system if system
    yield SITEDIR, :site   if site
    yield HOMEDIR, :home   if home and HOMEDIR

    gemdirs.each do |dir|
      yield dir, :gem
    end if gems

    nil
  end

  ##
  # The latest installed gems' ri directories

  def self.gemdirs
    return @gemdirs if @gemdirs

    require 'rubygems' unless defined?(Gem)

    # HACK dup'd from Gem.latest_partials and friends
    all_paths = []

    all_paths = Gem.path.map do |dir|
      Dir[File.join(dir, 'doc', '*', 'ri')]
    end.flatten

    ri_paths = {}

    all_paths.each do |dir|
      base = File.basename File.dirname(dir)
      if base =~ /(.*)-((\d+\.)*\d+)/ then
        name, version = $1, $2
        ver = Gem::Version.new version
        if ri_paths[name].nil? or ver > ri_paths[name][0] then
          ri_paths[name] = [ver, dir]
        end
      end
    end

    @gemdirs = ri_paths.map { |k,v| v.last }.sort
  rescue LoadError
    @gemdirs = []
  end

  ##
  # Returns existing directories from the selected documentation directories
  # as an Array.
  #
  # See also ::each

  def self.path(system, site, home, gems, *extra_dirs)
    path = raw_path system, site, home, gems, *extra_dirs

    path.select { |directory| File.directory? directory }
  end

  ##
  # Returns selected documentation directories including nonexistent
  # directories.
  #
  # See also ::each

  def self.raw_path(system, site, home, gems, *extra_dirs)
    path = []

    each(system, site, home, gems, *extra_dirs) do |dir, type|
      path << dir
    end

    path.compact
  end

end

