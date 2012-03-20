require 'rubygems/command'
require 'rubygems/indexer'

##
# Generates a index files for use as a gem server.
#
# See `gem help generate_index`

class Gem::Commands::GenerateIndexCommand < Gem::Command

  def initialize
    super 'generate_index',
          'Generates the index files for a gem server directory',
          :directory => '.', :build_legacy => true, :build_modern => true

    add_option '-d', '--directory=DIRNAME',
               'repository base dir containing gems subdir' do |dir, options|
      options[:directory] = File.expand_path dir
    end

    add_option '--[no-]legacy',
               'Generate Marshal.4.8' do |value, options|
      unless options[:build_modern] or value then
        raise OptionParser::InvalidOption, 'no indicies will be built'
      end

      options[:build_legacy] = value
    end

    add_option '--[no-]modern',
               'Generate indexes for RubyGems newer',
               'than 1.2.0' do |value, options|
      unless options[:build_legacy] or value then
        raise OptionParser::InvalidOption, 'no indicies will be built'
      end

      options[:build_modern] = value
    end

    add_option '--update',
               'Update modern indexes with gems added',
               'since the last update' do |value, options|
      options[:update] = value
    end

    add_option :RSS, '--rss-gems-host=GEM_HOST',
               'Host name where gems are served from,',
               'used for GUID and enclosure values' do |value, options|
      options[:rss_gems_host] = value
    end

    add_option :RSS, '--rss-host=HOST',
               'Host name for more gems information,',
               'used for RSS feed link' do |value, options|
      options[:rss_host] = value
    end

    add_option :RSS, '--rss-title=TITLE',
               'Set title for RSS feed' do |value, options|
      options[:rss_title] = value
    end
  end

  def defaults_str # :nodoc:
    "--directory . --legacy --modern"
  end

  def description # :nodoc:
    <<-EOF
The generate_index command creates a set of indexes for serving gems
statically.  The command expects a 'gems' directory under the path given to
the --directory option.  The given directory will be the directory you serve
as the gem repository.

For `gem generate_index --directory /path/to/repo`, expose /path/to/repo via
your HTTP server configuration (not /path/to/repo/gems).

When done, it will generate a set of files like this:

  gems/*.gem                                   # .gem files you want to
                                               # index

  specs.<version>.gz                           # specs index
  latest_specs.<version>.gz                    # latest specs index
  prerelease_specs.<version>.gz                # prerelease specs index
  quick/Marshal.<version>/<gemname>.gemspec.rz # Marshal quick index file

  # these files support legacy RubyGems
  Marshal.<version>
  Marshal.<version>.Z                          # Marshal full index

The .Z and .rz extension files are compressed with the inflate algorithm.
The Marshal version number comes from ruby's Marshal::MAJOR_VERSION and
Marshal::MINOR_VERSION constants.  It is used to ensure compatibility.

If --rss-host and --rss-gem-host are given an RSS feed will be generated at
index.rss containing gems released in the last two days.
    EOF
  end

  def execute
    if options[:update] and
       (options[:rss_host] or options[:rss_gems_host]) then
      alert_error '--update not compatible with RSS generation'
      terminate_interaction 1
    end

    if not File.exist?(options[:directory]) or
       not File.directory?(options[:directory]) then
      alert_error "unknown directory name #{directory}."
      terminate_interaction 1
    else
      indexer = Gem::Indexer.new options.delete(:directory), options

      if options[:update] then
        indexer.update_index
      else
        indexer.generate_index
      end
    end
  end

end

