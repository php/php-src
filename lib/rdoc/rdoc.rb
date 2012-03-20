require 'rdoc'

require 'rdoc/encoding'
require 'rdoc/parser'

# Simple must come first
require 'rdoc/parser/simple'
require 'rdoc/parser/ruby'
require 'rdoc/parser/c'

require 'rdoc/stats'
require 'rdoc/options'

require 'find'
require 'fileutils'
require 'time'

##
# Encapsulate the production of rdoc documentation. Basically you can use this
# as you would invoke rdoc from the command line:
#
#   rdoc = RDoc::RDoc.new
#   rdoc.document(args)
#
# Where +args+ is an array of strings, each corresponding to an argument you'd
# give rdoc on the command line.  See <tt>rdoc --help<tt> for details.
#
# = Plugins
#
# When you <tt>require 'rdoc/rdoc'</tt> RDoc looks for 'rdoc/discover' files
# in your installed gems.  This can be used to load alternate generators or
# add additional preprocessor directives.
#
# You will want to wrap your plugin loading in an RDoc version check.
# Something like:
#
#   begin
#     gem 'rdoc', '~> 3'
#     require 'path/to/my/awesome/rdoc/plugin'
#   rescue Gem::LoadError
#   end
#
# The most obvious plugin type is a new output generator.  See RDoc::Generator
# for details.
#
# You can also hook into RDoc::Markup to add new directives (:nodoc: is a
# directive).  See RDoc::Markup::PreProcess::register for details.

class RDoc::RDoc

  ##
  # File pattern to exclude

  attr_accessor :exclude

  ##
  # Generator instance used for creating output

  attr_accessor :generator

  ##
  # Hash of files and their last modified times.

  attr_reader :last_modified

  ##
  # RDoc options

  attr_accessor :options

  ##
  # Accessor for statistics.  Available after each call to parse_files

  attr_reader :stats

  ##
  # This is the list of supported output generators

  GENERATORS = {}

  ##
  # Add +klass+ that can generate output after parsing

  def self.add_generator(klass)
    name = klass.name.sub(/^RDoc::Generator::/, '').downcase
    GENERATORS[name] = klass
  end

  ##
  # Active RDoc::RDoc instance

  def self.current
    @current
  end

  ##
  # Sets the active RDoc::RDoc instance

  def self.current=(rdoc)
    @current = rdoc
  end

  ##
  # Resets all internal state

  def self.reset
    RDoc::TopLevel.reset
    RDoc::Parser::C.reset
  end

  ##
  # Creates a new RDoc::RDoc instance.  Call #document to parse files and
  # generate documentation.

  def initialize
    @current       = nil
    @exclude       = nil
    @generator     = nil
    @last_modified = {}
    @old_siginfo   = nil
    @options       = nil
    @stats         = nil
  end

  ##
  # Report an error message and exit

  def error(msg)
    raise RDoc::Error, msg
  end

  ##
  # Gathers a set of parseable files from the files and directories listed in
  # +files+.

  def gather_files files
    files = ["."] if files.empty?

    file_list = normalized_file_list files, true, @exclude

    file_list = file_list.uniq

    file_list = remove_unparseable file_list
  end

  ##
  # Turns RDoc from stdin into HTML

  def handle_pipe
    @html = RDoc::Markup::ToHtml.new

    out = @html.convert $stdin.read

    $stdout.write out
  end

  ##
  # Installs a siginfo handler that prints the current filename.

  def install_siginfo_handler
    return unless Signal.list.include? 'INFO'

    @old_siginfo = trap 'INFO' do
      puts @current if @current
    end
  end

  ##
  # Create an output dir if it doesn't exist. If it does exist, but doesn't
  # contain the flag file <tt>created.rid</tt> then we refuse to use it, as
  # we may clobber some manually generated documentation

  def setup_output_dir(dir, force)
    flag_file = output_flag_file dir

    last = {}

    if @options.dry_run then
      # do nothing
    elsif File.exist? dir then
      error "#{dir} exists and is not a directory" unless File.directory? dir

      begin
        open flag_file do |io|
          unless force then
            Time.parse io.gets

            io.each do |line|
              file, time = line.split "\t", 2
              time = Time.parse(time) rescue next
              last[file] = time
            end
          end
        end
      rescue SystemCallError, TypeError
        error <<-ERROR

Directory #{dir} already exists, but it looks like it isn't an RDoc directory.

Because RDoc doesn't want to risk destroying any of your existing files,
you'll need to specify a different output directory name (using the --op <dir>
option)

        ERROR
      end unless @options.force_output
    else
      FileUtils.mkdir_p dir
      FileUtils.touch output_flag_file dir
    end

    last
  end

  ##
  # Update the flag file in an output directory.

  def update_output_dir(op_dir, time, last = {})
    return if @options.dry_run or not @options.update_output_dir

    open output_flag_file(op_dir), "w" do |f|
      f.puts time.rfc2822
      last.each do |n, t|
        f.puts "#{n}\t#{t.rfc2822}"
      end
    end
  end

  ##
  # Return the path name of the flag file in an output directory.

  def output_flag_file(op_dir)
    File.join op_dir, "created.rid"
  end

  ##
  # The .document file contains a list of file and directory name patterns,
  # representing candidates for documentation. It may also contain comments
  # (starting with '#')

  def parse_dot_doc_file in_dir, filename
    # read and strip comments
    patterns = File.read(filename).gsub(/#.*/, '')

    result = []

    patterns.split.each do |patt|
      candidates = Dir.glob(File.join(in_dir, patt))
      result.concat normalized_file_list(candidates)
    end

    result
  end

  ##
  # Given a list of files and directories, create a list of all the Ruby
  # files they contain.
  #
  # If +force_doc+ is true we always add the given files, if false, only
  # add files that we guarantee we can parse.  It is true when looking at
  # files given on the command line, false when recursing through
  # subdirectories.
  #
  # The effect of this is that if you want a file with a non-standard
  # extension parsed, you must name it explicitly.

  def normalized_file_list(relative_files, force_doc = false,
                           exclude_pattern = nil)
    file_list = []

    relative_files.each do |rel_file_name|
      next if exclude_pattern && exclude_pattern =~ rel_file_name
      stat = File.stat rel_file_name rescue next

      case type = stat.ftype
      when "file" then
        next if last_modified = @last_modified[rel_file_name] and
                stat.mtime.to_i <= last_modified.to_i

        if force_doc or RDoc::Parser.can_parse(rel_file_name) then
          file_list << rel_file_name.sub(/^\.\//, '')
          @last_modified[rel_file_name] = stat.mtime
        end
      when "directory" then
        next if rel_file_name == "CVS" || rel_file_name == ".svn"

        dot_doc = File.join rel_file_name, RDoc::DOT_DOC_FILENAME

        if File.file? dot_doc then
          file_list << parse_dot_doc_file(rel_file_name, dot_doc)
        else
          file_list << list_files_in_directory(rel_file_name)
        end
      else
        raise RDoc::Error, "I can't deal with a #{type} #{rel_file_name}"
      end
    end

    file_list.flatten
  end

  ##
  # Return a list of the files to be processed in a directory. We know that
  # this directory doesn't have a .document file, so we're looking for real
  # files. However we may well contain subdirectories which must be tested
  # for .document files.

  def list_files_in_directory dir
    files = Dir.glob File.join(dir, "*")

    normalized_file_list files, false, @options.exclude
  end

  ##
  # Parses +filename+ and returns an RDoc::TopLevel

  def parse_file filename
    if defined?(Encoding) then
      encoding = @options.encoding
      filename = filename.encode encoding
    end

    @stats.add_file filename

    content = RDoc::Encoding.read_file filename, encoding

    return unless content

    top_level = RDoc::TopLevel.new filename

    parser = RDoc::Parser.for top_level, filename, content, @options, @stats

    return unless parser

    parser.scan

    # restart documentation for the classes & modules found
    top_level.classes_or_modules.each do |cm|
      cm.done_documenting = false
    end

    top_level

  rescue => e
    $stderr.puts <<-EOF
Before reporting this, could you check that the file you're documenting
has proper syntax:

  #{Gem.ruby} -c #{filename}

RDoc is not a full Ruby parser and will fail when fed invalid ruby programs.

The internal error was:

\t(#{e.class}) #{e.message}

    EOF

    $stderr.puts e.backtrace.join("\n\t") if $DEBUG_RDOC

    raise e
    nil
  end

  ##
  # Parse each file on the command line, recursively entering directories.

  def parse_files files
    file_list = gather_files files
    @stats = RDoc::Stats.new file_list.size, @options.verbosity

    return [] if file_list.empty?

    file_info = []

    @stats.begin_adding

    file_info = file_list.map do |filename|
      @current = filename
      parse_file filename
    end.compact

    @stats.done_adding

    file_info
  end

  ##
  # Removes file extensions known to be unparseable from +files+

  def remove_unparseable files
    files.reject do |file|
      file =~ /\.(?:class|eps|erb|scpt\.txt|ttf|yml)$/i
    end
  end

  ##
  # Generates documentation or a coverage report depending upon the settings
  # in +options+.
  #
  # +options+ can be either an RDoc::Options instance or an array of strings
  # equivalent to the strings that would be passed on the command line like
  # <tt>%w[-q -o doc -t My\ Doc\ Title]</tt>.  #document will automatically
  # call RDoc::Options#finish if an options instance was given.
  #
  # For a list of options, see either RDoc::Options or <tt>rdoc --help</tt>.
  #
  # By default, output will be stored in a directory called "doc" below the
  # current directory, so make sure you're somewhere writable before invoking.

  def document options
    RDoc::RDoc.reset

    if RDoc::Options === options then
      @options = options
      @options.finish
    else
      @options = RDoc::Options.new
      @options.parse options
    end

    if @options.pipe then
      handle_pipe
      exit
    end

    @exclude = @options.exclude

    unless @options.coverage_report then
      @last_modified = setup_output_dir @options.op_dir, @options.force_update
    end

    @start_time = Time.now

    file_info = parse_files @options.files

    @options.default_title = "RDoc Documentation"

    RDoc::TopLevel.complete @options.visibility

    @stats.coverage_level = @options.coverage_report

    if @options.coverage_report then
      puts

      puts @stats.report
    elsif file_info.empty? then
      $stderr.puts "\nNo newer files." unless @options.quiet
    else
      gen_klass = @options.generator

      @generator = gen_klass.new @options

      generate file_info
    end

    if @stats and (@options.coverage_report or not @options.quiet) then
      puts
      puts @stats.summary
    end

    exit @stats.fully_documented? if @options.coverage_report
  end

  ##
  # Generates documentation for +file_info+ (from #parse_files) into the
  # output dir using the generator selected
  # by the RDoc options

  def generate file_info
    Dir.chdir @options.op_dir do
      begin
        self.class.current = self

        unless @options.quiet then
          $stderr.puts "\nGenerating #{@generator.class.name.sub(/^.*::/, '')} format into #{Dir.pwd}..."
        end

        @generator.generate file_info
        update_output_dir '.', @start_time, @last_modified
      ensure
        self.class.current = nil
      end
    end
  end

  ##
  # Removes a siginfo handler and replaces the previous

  def remove_siginfo_handler
    return unless Signal.list.key? 'INFO'

    handler = @old_siginfo || 'DEFAULT'

    trap 'INFO', handler
  end

end

begin
  require 'rubygems'

  if Gem.respond_to? :find_files then
    rdoc_extensions = Gem.find_files 'rdoc/discover'

    rdoc_extensions.each do |extension|
      begin
        load extension
      rescue => e
        warn "error loading #{extension.inspect}: #{e.message} (#{e.class})"
        warn "\t#{e.backtrace.join "\n\t"}" if $DEBUG
      end
    end
  end
rescue LoadError
end

# require built-in generators after discovery in case they've been replaced
require 'rdoc/generator/darkfish'
require 'rdoc/generator/ri'

