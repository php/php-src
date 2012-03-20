require 'abbrev'
require 'optparse'

begin
  require 'readline'
rescue LoadError
end

begin
  require 'win32console'
rescue LoadError
end

require 'rdoc/ri'
require 'rdoc/ri/paths'
require 'rdoc/markup'
require 'rdoc/markup/formatter'
require 'rdoc/text'

##
# For RubyGems backwards compatibility

require 'rdoc/ri/formatter'

##
# The RI driver implements the command-line ri tool.
#
# The driver supports:
# * loading RI data from:
#   * Ruby's standard library
#   * RubyGems
#   * ~/.rdoc
#   * A user-supplied directory
# * Paging output (uses RI_PAGER environment variable, PAGER environment
#   variable or the less, more and pager programs)
# * Interactive mode with tab-completion
# * Abbreviated names (ri Zl shows Zlib documentation)
# * Colorized output
# * Merging output from multiple RI data sources

class RDoc::RI::Driver

  ##
  # Base Driver error class

  class Error < RDoc::RI::Error; end

  ##
  # Raised when a name isn't found in the ri data stores

  class NotFoundError < Error

    ##
    # Name that wasn't found

    alias name message

    def message # :nodoc:
      "Nothing known about #{super}"
    end
  end

  ##
  # An RDoc::RI::Store for each entry in the RI path

  attr_accessor :stores

  ##
  # Controls the user of the pager vs $stdout

  attr_accessor :use_stdout

  ##
  # Default options for ri

  def self.default_options
    options = {}
    options[:use_stdout] = !$stdout.tty?
    options[:width] = 72
    options[:interactive] = false
    options[:use_cache] = true
    options[:profile] = false

    # By default all standard paths are used.
    options[:use_system] = true
    options[:use_site] = true
    options[:use_home] = true
    options[:use_gems] = true
    options[:extra_doc_dirs] = []

    return options
  end

  ##
  # Dump +data_path+ using pp

  def self.dump data_path
    require 'pp'

    open data_path, 'rb' do |io|
      pp Marshal.load(io.read)
    end
  end

  ##
  # Parses +argv+ and returns a Hash of options

  def self.process_args argv
    options = default_options

    opts = OptionParser.new do |opt|
      opt.accept File do |file,|
        File.readable?(file) and not File.directory?(file) and file
      end

      opt.program_name = File.basename $0
      opt.version = RDoc::VERSION
      opt.release = nil
      opt.summary_indent = ' ' * 4

      opt.banner = <<-EOT
Usage: #{opt.program_name} [options] [names...]

Where name can be:

  Class | Class::method | Class#method | Class.method | method

All class names may be abbreviated to their minimum unambiguous form. If a name
is ambiguous, all valid options will be listed.

A '.' matches either class or instance methods, while #method
matches only instance and ::method matches only class methods.

For example:

    #{opt.program_name} Fil
    #{opt.program_name} File
    #{opt.program_name} File.new
    #{opt.program_name} zip

Note that shell quoting or escaping may be required for method names containing
punctuation:

    #{opt.program_name} 'Array.[]'
    #{opt.program_name} compact\\!

To see the default directories ri will search, run:

    #{opt.program_name} --list-doc-dirs

Specifying the --system, --site, --home, --gems or --doc-dir options will
limit ri to searching only the specified directories.

Options may also be set in the 'RI' environment variable.
      EOT

      opt.separator nil
      opt.separator "Options:"

      opt.separator nil

      formatters = RDoc::Markup.constants.grep(/^To[A-Z][a-z]+$/).sort
      formatters = formatters.sort.map do |formatter|
        formatter.to_s.sub('To', '').downcase
      end

      opt.on("--format=NAME", "-f",
             "Uses the selected formatter. The default",
             "formatter is bs for paged output and ansi",
             "otherwise. Valid formatters are:",
             formatters.join(' '), formatters) do |value|
        options[:formatter] = RDoc::Markup.const_get "To#{value.capitalize}"
      end

      opt.separator nil

      opt.on("--no-pager", "-T",
             "Send output directly to stdout,",
             "rather than to a pager.") do
        options[:use_stdout] = true
      end

      opt.separator nil

      opt.on("--width=WIDTH", "-w", OptionParser::DecimalInteger,
             "Set the width of the output.") do |value|
        options[:width] = value
      end

      opt.separator nil

      opt.on("--interactive", "-i",
             "In interactive mode you can repeatedly",
             "look up methods with autocomplete.") do
        options[:interactive] = true
      end

      opt.separator nil

      opt.on("--list", "-l",
             "List classes ri knows about.") do
        options[:list] = true
      end

      opt.separator nil

      opt.on("--[no-]profile",
             "Run with the ruby profiler") do |value|
        options[:profile] = value
      end

      opt.separator nil
      opt.separator "Data source options:"
      opt.separator nil

      opt.on("--list-doc-dirs",
             "List the directories from which ri will",
             "source documentation on stdout and exit.") do
        options[:list_doc_dirs] = true
      end

      opt.separator nil

      opt.on("--doc-dir=DIRNAME", "-d", Array,
             "List of directories from which to source",
             "documentation in addition to the standard",
             "directories.  May be repeated.") do |value|
        value.each do |dir|
          unless File.directory? dir then
            raise OptionParser::InvalidArgument, "#{dir} is not a directory"
          end

          options[:extra_doc_dirs] << File.expand_path(dir)
        end
      end

      opt.separator nil

      opt.on("--no-standard-docs",
             "Do not include documentation from",
             "the Ruby standard library, site_lib,",
             "installed gems, or ~/.rdoc.",
             "Use with --doc-dir") do
        options[:use_system] = false
        options[:use_site] = false
        options[:use_gems] = false
        options[:use_home] = false
      end

      opt.separator nil

      opt.on("--[no-]system",
             "Include documentation from Ruby's standard",
             "library.  Defaults to true.") do |value|
        options[:use_system] = value
      end

      opt.separator nil

      opt.on("--[no-]site",
             "Include documentation from libraries",
             "installed in site_lib.",
             "Defaults to true.") do |value|
        options[:use_site] = value
      end

      opt.separator nil

      opt.on("--[no-]gems",
             "Include documentation from RubyGems.",
             "Defaults to true.") do |value|
        options[:use_gems] = value
      end

      opt.separator nil

      opt.on("--[no-]home",
             "Include documentation stored in ~/.rdoc.",
             "Defaults to true.") do |value|
        options[:use_home] = value
      end

      opt.separator nil
      opt.separator "Debug options:"
      opt.separator nil

      opt.on("--dump=CACHE", File,
             "Dumps data from an ri cache or data file") do |value|
        options[:dump_path] = value
      end
    end

    argv = ENV['RI'].to_s.split.concat argv

    opts.parse! argv

    options[:names] = argv

    options[:use_stdout] ||= !$stdout.tty?
    options[:use_stdout] ||= options[:interactive]
    options[:width] ||= 72

    options

  rescue OptionParser::InvalidArgument, OptionParser::InvalidOption => e
    puts opts
    puts
    puts e
    exit 1
  end

  ##
  # Runs the ri command line executable using +argv+

  def self.run argv = ARGV
    options = process_args argv

    if options[:dump_path] then
      dump options[:dump_path]
      return
    end

    ri = new options
    ri.run
  end

  ##
  # Creates a new driver using +initial_options+ from ::process_args

  def initialize initial_options = {}
    @paging = false
    @classes = nil

    options = self.class.default_options.update(initial_options)

    @formatter_klass = options[:formatter]

    require 'profile' if options[:profile]

    @names = options[:names]
    @list = options[:list]

    @doc_dirs = []
    @stores   = []

    RDoc::RI::Paths.each(options[:use_system], options[:use_site],
                         options[:use_home], options[:use_gems],
                         *options[:extra_doc_dirs]) do |path, type|
      @doc_dirs << path

      store = RDoc::RI::Store.new path, type
      store.load_cache
      @stores << store
    end

    @list_doc_dirs = options[:list_doc_dirs]

    @interactive = options[:interactive]
    @use_stdout  = options[:use_stdout]
  end

  ##
  # Adds paths for undocumented classes +also_in+ to +out+

  def add_also_in out, also_in
    return if also_in.empty?

    out << RDoc::Markup::Rule.new(1)
    out << RDoc::Markup::Paragraph.new("Also found in:")

    paths = RDoc::Markup::Verbatim.new
    also_in.each do |store|
      paths.parts.push store.friendly_path, "\n"
    end
    out << paths
  end

  ##
  # Adds a class header to +out+ for class +name+ which is described in
  # +classes+.

  def add_class out, name, classes
    heading = if classes.all? { |klass| klass.module? } then
                name
              else
                superclass = classes.map do |klass|
                  klass.superclass unless klass.module?
                end.compact.shift || 'Object'

                superclass = superclass.full_name unless String === superclass

                "#{name} < #{superclass}"
              end

    out << RDoc::Markup::Heading.new(1, heading)
    out << RDoc::Markup::BlankLine.new
  end

  ##
  # Adds "(from ...)" to +out+ for +store+

  def add_from out, store
    out << RDoc::Markup::Paragraph.new("(from #{store.friendly_path})")
  end

  ##
  # Adds +includes+ to +out+

  def add_includes out, includes
    return if includes.empty?

    out << RDoc::Markup::Rule.new(1)
    out << RDoc::Markup::Heading.new(1, "Includes:")

    includes.each do |modules, store|
      if modules.length == 1 then
        include = modules.first
        name = include.name
        path = store.friendly_path
        out << RDoc::Markup::Paragraph.new("#{name} (from #{path})")

        if include.comment then
          out << RDoc::Markup::BlankLine.new
          out << include.comment
        end
      else
        out << RDoc::Markup::Paragraph.new("(from #{store.friendly_path})")

        wout, with = modules.partition { |incl| incl.comment.empty? }

        out << RDoc::Markup::BlankLine.new unless with.empty?

        with.each do |incl|
          out << RDoc::Markup::Paragraph.new(incl.name)
          out << RDoc::Markup::BlankLine.new
          out << incl.comment
        end

        unless wout.empty? then
          verb = RDoc::Markup::Verbatim.new

          wout.each do |incl|
            verb.push incl.name, "\n"
          end

          out << verb
        end
      end
    end
  end

  ##
  # Adds a list of +methods+ to +out+ with a heading of +name+

  def add_method_list out, methods, name
    return if methods.empty?

    out << RDoc::Markup::Heading.new(1, "#{name}:")
    out << RDoc::Markup::BlankLine.new

    if @use_stdout and !@interactive
      out.push(*methods.map do |method|
        RDoc::Markup::Verbatim.new method
      end)
    else
      out << RDoc::Markup::IndentedParagraph.new(2, methods.join(', '))
    end

    out << RDoc::Markup::BlankLine.new
  end

  ##
  # Returns ancestor classes of +klass+

  def ancestors_of klass
    ancestors = []

    unexamined = [klass]
    seen = []

    loop do
      break if unexamined.empty?
      current = unexamined.shift
      seen << current

      stores = classes[current]

      break unless stores and not stores.empty?

      klasses = stores.map do |store|
        store.ancestors[current]
      end.flatten.uniq

      klasses = klasses - seen

      ancestors.push(*klasses)
      unexamined.push(*klasses)
    end

    ancestors.reverse
  end

  ##
  # For RubyGems backwards compatibility

  def class_cache # :nodoc:
  end

  ##
  # Builds a RDoc::Markup::Document from +found+, +klasess+ and +includes+

  def class_document name, found, klasses, includes
    also_in = []

    out = RDoc::Markup::Document.new

    add_class out, name, klasses

    add_includes out, includes

    found.each do |store, klass|
      comment = klass.comment
      # TODO the store's cache should always return an empty Array
      class_methods    = store.class_methods[klass.full_name]    || []
      instance_methods = store.instance_methods[klass.full_name] || []
      attributes       = store.attributes[klass.full_name]       || []

      if comment.empty? and
         instance_methods.empty? and class_methods.empty? then
        also_in << store
        next
      end

      add_from out, store

      unless comment.empty? then
        out << RDoc::Markup::Rule.new(1)

        if comment.merged? then
          parts = comment.parts
          parts = parts.zip [RDoc::Markup::BlankLine.new] * parts.length
          parts.flatten!
          parts.pop

          out.push(*parts)
        else
          out << comment
        end
      end

      if class_methods or instance_methods or not klass.constants.empty? then
        out << RDoc::Markup::Rule.new(1)
      end

      unless klass.constants.empty? then
        out << RDoc::Markup::Heading.new(1, "Constants:")
        out << RDoc::Markup::BlankLine.new
        list = RDoc::Markup::List.new :NOTE

        constants = klass.constants.sort_by { |constant| constant.name }

        list.push(*constants.map do |constant|
          parts = constant.comment.parts if constant.comment
          parts << RDoc::Markup::Paragraph.new('[not documented]') if
            parts.empty?

          RDoc::Markup::ListItem.new(constant.name, *parts)
        end)

        out << list
        out << RDoc::Markup::BlankLine.new
      end

      add_method_list out, class_methods,    'Class methods'
      add_method_list out, instance_methods, 'Instance methods'
      add_method_list out, attributes,       'Attributes'
    end

    add_also_in out, also_in

    out
  end

  ##
  # Hash mapping a known class or module to the stores it can be loaded from

  def classes
    return @classes if @classes

    @classes = {}

    @stores.each do |store|
      store.cache[:modules].each do |mod|
        # using default block causes searched-for modules to be added
        @classes[mod] ||= []
        @classes[mod] << store
      end
    end

    @classes
  end

  ##
  # Returns the stores wherin +name+ is found along with the classes and
  # includes that match it

  def classes_and_includes_for name
    klasses = []
    includes = []

    found = @stores.map do |store|
      begin
        klass = store.load_class name
        klasses  << klass
        includes << [klass.includes, store] if klass.includes
        [store, klass]
      rescue Errno::ENOENT
      end
    end.compact

    includes.reject! do |modules,| modules.empty? end

    [found, klasses, includes]
  end

  ##
  # Completes +name+ based on the caches.  For Readline

  def complete name
    klasses = classes.keys
    completions = []

    klass, selector, method = parse_name name

    # may need to include Foo when given Foo::
    klass_name = method ? name : klass

    if name !~ /#|\./ then
      completions = klasses.grep(/^#{Regexp.escape klass_name}[^:]*$/)
      completions.concat klasses.grep(/^#{Regexp.escape name}[^:]*$/) if
        name =~ /::$/

      completions << klass if classes.key? klass # to complete a method name
    elsif selector then
      completions << klass if classes.key? klass
    elsif classes.key? klass_name then
      completions << klass_name
    end

    if completions.include? klass and name =~ /#|\.|::/ then
      methods = list_methods_matching name

      if not methods.empty? then
        # remove Foo if given Foo:: and a method was found
        completions.delete klass
      elsif selector then
        # replace Foo with Foo:: as given
        completions.delete klass
        completions << "#{klass}#{selector}"
      end

      completions.push(*methods)
    end

    completions.sort.uniq
  end

  ##
  # Converts +document+ to text and writes it to the pager

  def display document
    page do |io|
      text = document.accept formatter(io)

      io.write text
    end
  end

  ##
  # Outputs formatted RI data for class +name+.  Groups undocumented classes

  def display_class name
    return if name =~ /#|\./

    found, klasses, includes = classes_and_includes_for name

    return if found.empty?

    out = class_document name, found, klasses, includes

    display out
  end

  ##
  # Outputs formatted RI data for method +name+

  def display_method name
    found = load_methods_matching name

    raise NotFoundError, name if found.empty?

    filtered = filter_methods found, name

    out = method_document name, filtered

    display out
  end

  ##
  # Outputs formatted RI data for the class or method +name+.
  #
  # Returns true if +name+ was found, false if it was not an alternative could
  # be guessed, raises an error if +name+ couldn't be guessed.

  def display_name name
    return true if display_class name

    display_method name if name =~ /::|#|\./

    true
  rescue NotFoundError
    matches = list_methods_matching name if name =~ /::|#|\./
    matches = classes.keys.grep(/^#{name}/) if matches.empty?

    raise if matches.empty?

    page do |io|
      io.puts "#{name} not found, maybe you meant:"
      io.puts
      io.puts matches.join("\n")
    end

    false
  end

  ##
  # Displays each name in +name+

  def display_names names
    names.each do |name|
      name = expand_name name

      display_name name
    end
  end

  ##
  # Expands abbreviated klass +klass+ into a fully-qualified class.  "Zl::Da"
  # will be expanded to Zlib::DataError.

  def expand_class klass
    klass.split('::').inject '' do |expanded, klass_part|
      expanded << '::' unless expanded.empty?
      short = expanded << klass_part

      subset = classes.keys.select do |klass_name|
        klass_name =~ /^#{expanded}[^:]*$/
      end

      abbrevs = Abbrev.abbrev subset

      expanded = abbrevs[short]

      raise NotFoundError, short unless expanded

      expanded.dup
    end
  end

  ##
  # Expands the class portion of +name+ into a fully-qualified class.  See
  # #expand_class.

  def expand_name name
    klass, selector, method = parse_name name

    return [selector, method].join if klass.empty?

    "#{expand_class klass}#{selector}#{method}"
  end

  ##
  # Filters the methods in +found+ trying to find a match for +name+.

  def filter_methods found, name
    regexp = name_regexp name

    filtered = found.find_all do |store, methods|
      methods.any? { |method| method.full_name =~ regexp }
    end

    return filtered unless filtered.empty?

    found
  end

  ##
  # Yields items matching +name+ including the store they were found in, the
  # class being searched for, the class they were found in (an ancestor) the
  # types of methods to look up (from #method_type), and the method name being
  # searched for

  def find_methods name
    klass, selector, method = parse_name name

    types = method_type selector

    klasses = nil
    ambiguous = klass.empty?

    if ambiguous then
      klasses = classes.keys
    else
      klasses = ancestors_of klass
      klasses.unshift klass
    end

    methods = []

    klasses.each do |ancestor|
      ancestors = classes[ancestor]

      next unless ancestors

      klass = ancestor if ambiguous

      ancestors.each do |store|
        methods << [store, klass, ancestor, types, method]
      end
    end

    methods = methods.sort_by do |_, k, a, _, m|
      [k, a, m].compact
    end

    methods.each do |item|
      yield(*item) # :yields: store, klass, ancestor, types, method
    end

    self
  end

  ##
  # Creates a new RDoc::Markup::Formatter.  If a formatter is given with -f,
  # use it.  If we're outputting to a pager, use bs, otherwise ansi.

  def formatter(io)
    if @formatter_klass then
      @formatter_klass.new
    elsif paging? or !io.tty? then
      RDoc::Markup::ToBs.new
    else
      RDoc::Markup::ToAnsi.new
    end
  end

  ##
  # Runs ri interactively using Readline if it is available.

  def interactive
    puts "\nEnter the method name you want to look up."

    if defined? Readline then
      Readline.completion_proc = method :complete
      puts "You can use tab to autocomplete."
    end

    puts "Enter a blank line to exit.\n\n"

    loop do
      name = if defined? Readline then
               Readline.readline ">> "
             else
               print ">> "
               $stdin.gets
             end

      return if name.nil? or name.empty?

      name = expand_name name.strip

      begin
        display_name name
      rescue NotFoundError => e
        puts e.message
      end
    end

  rescue Interrupt
    exit
  end

  ##
  # Is +file+ in ENV['PATH']?

  def in_path? file
    return true if file =~ %r%\A/% and File.exist? file

    ENV['PATH'].split(File::PATH_SEPARATOR).any? do |path|
      File.exist? File.join(path, file)
    end
  end

  ##
  # Lists classes known to ri starting with +names+.  If +names+ is empty all
  # known classes are shown.

  def list_known_classes names = []
    classes = []

    stores.each do |store|
      classes << store.modules
    end

    classes = classes.flatten.uniq.sort

    unless names.empty? then
      filter = Regexp.union names.map { |name| /^#{name}/ }

      classes = classes.grep filter
    end

    page do |io|
      if paging? or io.tty? then
        if names.empty? then
          io.puts "Classes and Modules known to ri:"
        else
          io.puts "Classes and Modules starting with #{names.join ', '}:"
        end
        io.puts
      end

      io.puts classes.join("\n")
    end
  end

  ##
  # Returns an Array of methods matching +name+

  def list_methods_matching name
    found = []

    find_methods name do |store, klass, ancestor, types, method|
      if types == :instance or types == :both then
        methods = store.instance_methods[ancestor]

        if methods then
          matches = methods.grep(/^#{Regexp.escape method.to_s}/)

          matches = matches.map do |match|
            "#{klass}##{match}"
          end

          found.push(*matches)
        end
      end

      if types == :class or types == :both then
        methods = store.class_methods[ancestor]

        next unless methods
        matches = methods.grep(/^#{Regexp.escape method.to_s}/)

        matches = matches.map do |match|
          "#{klass}::#{match}"
        end

        found.push(*matches)
      end
    end

    found.uniq
  end

  ##
  # Loads RI data for method +name+ on +klass+ from +store+.  +type+ and
  # +cache+ indicate if it is a class or instance method.

  def load_method store, cache, klass, type, name
    methods = store.send(cache)[klass]

    return unless methods

    method = methods.find do |method_name|
      method_name == name
    end

    return unless method

    store.load_method klass, "#{type}#{method}"
  end

  ##
  # Returns an Array of RI data for methods matching +name+

  def load_methods_matching name
    found = []

    find_methods name do |store, klass, ancestor, types, method|
      methods = []

      methods << load_method(store, :class_methods, ancestor, '::',  method) if
        [:class, :both].include? types

      methods << load_method(store, :instance_methods, ancestor, '#',  method) if
        [:instance, :both].include? types

      found << [store, methods.compact]
    end

    found.reject do |path, methods| methods.empty? end
  end

  ##
  # Builds a RDoc::Markup::Document from +found+, +klasess+ and +includes+

  def method_document name, filtered
    out = RDoc::Markup::Document.new

    out << RDoc::Markup::Heading.new(1, name)
    out << RDoc::Markup::BlankLine.new

    filtered.each do |store, methods|
      methods.each do |method|
        out << RDoc::Markup::Paragraph.new("(from #{store.friendly_path})")

        unless name =~ /^#{Regexp.escape method.parent_name}/ then
          out << RDoc::Markup::Heading.new(3, "Implementation from #{method.parent_name}")
        end
        out << RDoc::Markup::Rule.new(1)

        if method.arglists then
          arglists = method.arglists.chomp.split "\n"
          arglists = arglists.map { |line| line + "\n" }
          out << RDoc::Markup::Verbatim.new(*arglists)
          out << RDoc::Markup::Rule.new(1)
        end

        out << RDoc::Markup::BlankLine.new
        out << method.comment
        out << RDoc::Markup::BlankLine.new
      end
    end

    out
  end

  ##
  # Returns the type of method (:both, :instance, :class) for +selector+

  def method_type selector
    case selector
    when '.', nil then :both
    when '#'      then :instance
    else               :class
    end
  end

  ##
  # Returns a regular expression for +name+ that will match an
  # RDoc::AnyMethod's name.

  def name_regexp name
    klass, type, name = parse_name name

    case type
    when '#', '::' then
      /^#{klass}#{type}#{Regexp.escape name}$/
    else
      /^#{klass}(#|::)#{Regexp.escape name}$/
    end
  end

  ##
  # Paginates output through a pager program.

  def page
    if pager = setup_pager then
      begin
        yield pager
      ensure
        pager.close
      end
    else
      yield $stdout
    end
  rescue Errno::EPIPE
  ensure
    @paging = false
  end

  ##
  # Are we using a pager?

  def paging?
    @paging
  end

  ##
  # Extracts the class, selector and method name parts from +name+ like
  # Foo::Bar#baz.
  #
  # NOTE: Given Foo::Bar, Bar is considered a class even though it may be a
  #       method

  def parse_name name
    parts = name.split(/(::|#|\.)/)

    if parts.length == 1 then
      if parts.first =~ /^[a-z]|^([%&*+\/<>^`|~-]|\+@|-@|<<|<=>?|===?|=>|=~|>>|\[\]=?|~@)$/ then
        type = '.'
        meth = parts.pop
      else
        type = nil
        meth = nil
      end
    elsif parts.length == 2 or parts.last =~ /::|#|\./ then
      type = parts.pop
      meth = nil
    elsif parts[-2] != '::' or parts.last !~ /^[A-Z]/ then
      meth = parts.pop
      type = parts.pop
    end

    klass = parts.join

    [klass, type, meth]
  end

  ##
  # Looks up and displays ri data according to the options given.

  def run
    if @list_doc_dirs then
      puts @doc_dirs
    elsif @list then
      list_known_classes @names
    elsif @interactive or @names.empty? then
      interactive
    else
      display_names @names
    end
  rescue NotFoundError => e
    abort e.message
  end

  ##
  # Sets up a pager program to pass output through.  Tries the RI_PAGER and
  # PAGER environment variables followed by pager, less then more.

  def setup_pager
    return if @use_stdout

    pagers = [ENV['RI_PAGER'], ENV['PAGER'], 'pager', 'less', 'more']

    pagers.compact.uniq.each do |pager|
      next unless pager

      pager_cmd = pager.split.first

      next unless in_path? pager_cmd

      io = IO.popen(pager, 'w') rescue next

      next if $? and $?.exited? # pager didn't work

      @paging = true

      return io
    end

    @use_stdout = true

    nil
  end

end

