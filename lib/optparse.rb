#
# optparse.rb - command-line option analysis with the OptionParser class.
#
# Author:: Nobu Nakada
# Documentation:: Nobu Nakada and Gavin Sinclair.
#
# See OptionParser for documentation.
#


# == Developer Documentation (not for RDoc output)
#
# === Class tree
#
# - OptionParser:: front end
# - OptionParser::Switch:: each switches
# - OptionParser::List:: options list
# - OptionParser::ParseError:: errors on parsing
#   - OptionParser::AmbiguousOption
#   - OptionParser::NeedlessArgument
#   - OptionParser::MissingArgument
#   - OptionParser::InvalidOption
#   - OptionParser::InvalidArgument
#     - OptionParser::AmbiguousArgument
#
# === Object relationship diagram
#
#   +--------------+
#   | OptionParser |<>-----+
#   +--------------+       |                      +--------+
#                          |                    ,-| Switch |
#        on_head -------->+---------------+    /  +--------+
#        accept/reject -->| List          |<|>-
#                         |               |<|>-  +----------+
#        on ------------->+---------------+    `-| argument |
#                           :           :        |  class   |
#                         +---------------+      |==========|
#        on_tail -------->|               |      |pattern   |
#                         +---------------+      |----------|
#   OptionParser.accept ->| DefaultList   |      |converter |
#                reject   |(shared between|      +----------+
#                         | all instances)|
#                         +---------------+
#
# == OptionParser
#
# === Introduction
#
# OptionParser is a class for command-line option analysis.  It is much more
# advanced, yet also easier to use, than GetoptLong, and is a more Ruby-oriented
# solution.
#
# === Features
#
# 1. The argument specification and the code to handle it are written in the
#    same place.
# 2. It can output an option summary; you don't need to maintain this string
#    separately.
# 3. Optional and mandatory arguments are specified very gracefully.
# 4. Arguments can be automatically converted to a specified class.
# 5. Arguments can be restricted to a certain set.
#
# All of these features are demonstrated in the examples below.  See
# #make_switch for full documentation.
#
# === Minimal example
#
#   require 'optparse'
#
#   options = {}
#   OptionParser.new do |opts|
#     opts.banner = "Usage: example.rb [options]"
#
#     opts.on("-v", "--[no-]verbose", "Run verbosely") do |v|
#       options[:verbose] = v
#     end
#   end.parse!
#
#   p options
#   p ARGV
#
# === Complete example
#
# The following example is a complete Ruby program.  You can run it and see the
# effect of specifying various options.  This is probably the best way to learn
# the features of +optparse+.
#
#   require 'optparse'
#   require 'optparse/time'
#   require 'ostruct'
#   require 'pp'
#
#   class OptparseExample
#
#     CODES = %w[iso-2022-jp shift_jis euc-jp utf8 binary]
#     CODE_ALIASES = { "jis" => "iso-2022-jp", "sjis" => "shift_jis" }
#
#     #
#     # Return a structure describing the options.
#     #
#     def self.parse(args)
#       # The options specified on the command line will be collected in *options*.
#       # We set default values here.
#       options = OpenStruct.new
#       options.library = []
#       options.inplace = false
#       options.encoding = "utf8"
#       options.transfer_type = :auto
#       options.verbose = false
#
#       opts = OptionParser.new do |opts|
#         opts.banner = "Usage: example.rb [options]"
#
#         opts.separator ""
#         opts.separator "Specific options:"
#
#         # Mandatory argument.
#         opts.on("-r", "--require LIBRARY",
#                 "Require the LIBRARY before executing your script") do |lib|
#           options.library << lib
#         end
#
#         # Optional argument; multi-line description.
#         opts.on("-i", "--inplace [EXTENSION]",
#                 "Edit ARGV files in place",
#                 "  (make backup if EXTENSION supplied)") do |ext|
#           options.inplace = true
#           options.extension = ext || ''
#           options.extension.sub!(/\A\.?(?=.)/, ".")  # Ensure extension begins with dot.
#         end
#
#         # Cast 'delay' argument to a Float.
#         opts.on("--delay N", Float, "Delay N seconds before executing") do |n|
#           options.delay = n
#         end
#
#         # Cast 'time' argument to a Time object.
#         opts.on("-t", "--time [TIME]", Time, "Begin execution at given time") do |time|
#           options.time = time
#         end
#
#         # Cast to octal integer.
#         opts.on("-F", "--irs [OCTAL]", OptionParser::OctalInteger,
#                 "Specify record separator (default \\0)") do |rs|
#           options.record_separator = rs
#         end
#
#         # List of arguments.
#         opts.on("--list x,y,z", Array, "Example 'list' of arguments") do |list|
#           options.list = list
#         end
#
#         # Keyword completion.  We are specifying a specific set of arguments (CODES
#         # and CODE_ALIASES - notice the latter is a Hash), and the user may provide
#         # the shortest unambiguous text.
#         code_list = (CODE_ALIASES.keys + CODES).join(',')
#         opts.on("--code CODE", CODES, CODE_ALIASES, "Select encoding",
#                 "  (#{code_list})") do |encoding|
#           options.encoding = encoding
#         end
#
#         # Optional argument with keyword completion.
#         opts.on("--type [TYPE]", [:text, :binary, :auto],
#                 "Select transfer type (text, binary, auto)") do |t|
#           options.transfer_type = t
#         end
#
#         # Boolean switch.
#         opts.on("-v", "--[no-]verbose", "Run verbosely") do |v|
#           options.verbose = v
#         end
#
#         opts.separator ""
#         opts.separator "Common options:"
#
#         # No argument, shows at tail.  This will print an options summary.
#         # Try it and see!
#         opts.on_tail("-h", "--help", "Show this message") do
#           puts opts
#           exit
#         end
#
#         # Another typical switch to print the version.
#         opts.on_tail("--version", "Show version") do
#           puts OptionParser::Version.join('.')
#           exit
#         end
#       end
#
#       opts.parse!(args)
#       options
#     end  # parse()
#
#   end  # class OptparseExample
#
#   options = OptparseExample.parse(ARGV)
#   pp options
#
# === Shell Completion
#
# For modern shells (e.g. bash, zsh, etc.), you can use shell
# completion for command line options.
#
# === Further documentation
#
# The above examples should be enough to learn how to use this class.  If you
# have any questions, email me (gsinclair@soyabean.com.au) and I will update
# this document.
#
class OptionParser
  # :stopdoc:
  RCSID = %w$Id$[1..-1].each {|s| s.freeze}.freeze
  Version = (RCSID[1].split('.').collect {|s| s.to_i}.extend(Comparable).freeze if RCSID[1])
  LastModified = (Time.gm(*RCSID[2, 2].join('-').scan(/\d+/).collect {|s| s.to_i}) if RCSID[2])
  Release = RCSID[2]

  NoArgument = [NO_ARGUMENT = :NONE, nil].freeze
  RequiredArgument = [REQUIRED_ARGUMENT = :REQUIRED, true].freeze
  OptionalArgument = [OPTIONAL_ARGUMENT = :OPTIONAL, false].freeze
  # :startdoc:

  #
  # Keyword completion module.  This allows partial arguments to be specified
  # and resolved against a list of acceptable values.
  #
  module Completion
    def self.regexp(key, icase)
      Regexp.new('\A' + Regexp.quote(key).gsub(/\w+\b/, '\&\w*'), icase)
    end

    def self.candidate(key, icase = false, pat = nil, &block)
      pat ||= Completion.regexp(key, icase)
      candidates = []
      block.call do |k, *v|
        (if Regexp === k
           kn = nil
           k === key
         else
           kn = defined?(k.id2name) ? k.id2name : k
           pat === kn
         end) or next
        v << k if v.empty?
        candidates << [k, v, kn]
      end
      candidates
    end

    def candidate(key, icase = false, pat = nil)
      Completion.candidate(key, icase, pat, &method(:each))
    end

    public
    def complete(key, icase = false, pat = nil)
      candidates = candidate(key, icase, pat, &method(:each)).sort_by {|k, v, kn| kn.size}
      if candidates.size == 1
        canon, sw, * = candidates[0]
      elsif candidates.size > 1
        canon, sw, cn = candidates.shift
        candidates.each do |k, v, kn|
          next if sw == v
          if String === cn and String === kn
            if cn.rindex(kn, 0)
              canon, sw, cn = k, v, kn
              next
            elsif kn.rindex(cn, 0)
              next
            end
          end
          throw :ambiguous, key
        end
      end
      if canon
        block_given? or return key, *sw
        yield(key, *sw)
      end
    end

    def convert(opt = nil, val = nil, *)
      val
    end
  end


  #
  # Map from option/keyword string to object with completion.
  #
  class OptionMap < Hash
    include Completion
  end


  #
  # Individual switch class.  Not important to the user.
  #
  # Defined within Switch are several Switch-derived classes: NoArgument,
  # RequiredArgument, etc.
  #
  class Switch
    attr_reader :pattern, :conv, :short, :long, :arg, :desc, :block

    #
    # Guesses argument style from +arg+.  Returns corresponding
    # OptionParser::Switch class (OptionalArgument, etc.).
    #
    def self.guess(arg)
      case arg
      when ""
        t = self
      when /\A=?\[/
        t = Switch::OptionalArgument
      when /\A\s+\[/
        t = Switch::PlacedArgument
      else
        t = Switch::RequiredArgument
      end
      self >= t or incompatible_argument_styles(arg, t)
      t
    end

    def self.incompatible_argument_styles(arg, t)
      raise(ArgumentError, "#{arg}: incompatible argument styles\n  #{self}, #{t}",
            ParseError.filter_backtrace(caller(2)))
    end

    def self.pattern
      NilClass
    end

    def initialize(pattern = nil, conv = nil,
                   short = nil, long = nil, arg = nil,
                   desc = ([] if short or long), block = Proc.new)
      raise if Array === pattern
      @pattern, @conv, @short, @long, @arg, @desc, @block =
        pattern, conv, short, long, arg, desc, block
    end

    #
    # Parses +arg+ and returns rest of +arg+ and matched portion to the
    # argument pattern. Yields when the pattern doesn't match substring.
    #
    def parse_arg(arg)
      pattern or return nil, [arg]
      unless m = pattern.match(arg)
        yield(InvalidArgument, arg)
        return arg, []
      end
      if String === m
        m = [s = m]
      else
        m = m.to_a
        s = m[0]
        return nil, m unless String === s
      end
      raise InvalidArgument, arg unless arg.rindex(s, 0)
      return nil, m if s.length == arg.length
      yield(InvalidArgument, arg) # didn't match whole arg
      return arg[s.length..-1], m
    end
    private :parse_arg

    #
    # Parses argument, converts and returns +arg+, +block+ and result of
    # conversion. Yields at semi-error condition instead of raising an
    # exception.
    #
    def conv_arg(arg, val = [])
      if conv
        val = conv.call(*val)
      else
        val = proc {|v| v}.call(*val)
      end
      return arg, block, val
    end
    private :conv_arg

    #
    # Produces the summary text. Each line of the summary is yielded to the
    # block (without newline).
    #
    # +sdone+::  Already summarized short style options keyed hash.
    # +ldone+::  Already summarized long style options keyed hash.
    # +width+::  Width of left side (option part). In other words, the right
    #            side (description part) starts after +width+ columns.
    # +max+::    Maximum width of left side -> the options are filled within
    #            +max+ columns.
    # +indent+:: Prefix string indents all summarized lines.
    #
    def summarize(sdone = [], ldone = [], width = 1, max = width - 1, indent = "")
      sopts, lopts = [], [], nil
      @short.each {|s| sdone.fetch(s) {sopts << s}; sdone[s] = true} if @short
      @long.each {|s| ldone.fetch(s) {lopts << s}; ldone[s] = true} if @long
      return if sopts.empty? and lopts.empty? # completely hidden

      left = [sopts.join(', ')]
      right = desc.dup

      while s = lopts.shift
        l = left[-1].length + s.length
        l += arg.length if left.size == 1 && arg
        l < max or sopts.empty? or left << ''
        left[-1] << if left[-1].empty? then ' ' * 4 else ', ' end << s
      end

      if arg
        left[0] << (left[1] ? arg.sub(/\A(\[?)=/, '\1') + ',' : arg)
      end
      mlen = left.collect {|ss| ss.length}.max.to_i
      while mlen > width and l = left.shift
        mlen = left.collect {|ss| ss.length}.max.to_i if l.length == mlen
        if l.length < width and (r = right[0]) and !r.empty?
          l = l.to_s.ljust(width) + ' ' + r
          right.shift
        end
        yield(indent + l)
      end

      while begin l = left.shift; r = right.shift; l or r end
        l = l.to_s.ljust(width) + ' ' + r if r and !r.empty?
        yield(indent + l)
      end

      self
    end

    def add_banner(to)  # :nodoc:
      unless @short or @long
        s = desc.join
        to << " [" + s + "]..." unless s.empty?
      end
      to
    end

    def match_nonswitch?(str)  # :nodoc:
      @pattern =~ str unless @short or @long
    end

    #
    # Main name of the switch.
    #
    def switch_name
      (long.first || short.first).sub(/\A-+(?:\[no-\])?/, '')
    end

    def compsys(sdone, ldone)   # :nodoc:
      sopts, lopts = [], []
      @short.each {|s| sdone.fetch(s) {sopts << s}; sdone[s] = true} if @short
      @long.each {|s| ldone.fetch(s) {lopts << s}; ldone[s] = true} if @long
      return if sopts.empty? and lopts.empty? # completely hidden

      (sopts+lopts).each do |opt|
        # "(-x -c -r)-l[left justify]" \
        if opt =~ /^--\[no-\](.+)$/
          o = $1
          yield("--#{o}", desc.join(""))
          yield("--no-#{o}", desc.join(""))
        else
          yield("#{opt}", desc.join(""))
        end
      end
    end

    #
    # Switch that takes no arguments.
    #
    class NoArgument < self

      #
      # Raises an exception if any arguments given.
      #
      def parse(arg, argv)
        yield(NeedlessArgument, arg) if arg
        conv_arg(arg)
      end

      def self.incompatible_argument_styles(*)
      end

      def self.pattern
        Object
      end
    end

    #
    # Switch that takes an argument.
    #
    class RequiredArgument < self

      #
      # Raises an exception if argument is not present.
      #
      def parse(arg, argv)
        unless arg
          raise MissingArgument if argv.empty?
          arg = argv.shift
        end
        conv_arg(*parse_arg(arg, &method(:raise)))
      end
    end

    #
    # Switch that can omit argument.
    #
    class OptionalArgument < self

      #
      # Parses argument if given, or uses default value.
      #
      def parse(arg, argv, &error)
        if arg
          conv_arg(*parse_arg(arg, &error))
        else
          conv_arg(arg)
        end
      end
    end

    #
    # Switch that takes an argument, which does not begin with '-'.
    #
    class PlacedArgument < self

      #
      # Returns nil if argument is not present or begins with '-'.
      #
      def parse(arg, argv, &error)
        if !(val = arg) and (argv.empty? or /\A-/ =~ (val = argv[0]))
          return nil, block, nil
        end
        opt = (val = parse_arg(val, &error))[1]
        val = conv_arg(*val)
        if opt and !arg
          argv.shift
        else
          val[0] = nil
        end
        val
      end
    end
  end

  #
  # Simple option list providing mapping from short and/or long option
  # string to OptionParser::Switch and mapping from acceptable argument to
  # matching pattern and converter pair. Also provides summary feature.
  #
  class List
    # Map from acceptable argument types to pattern and converter pairs.
    attr_reader :atype

    # Map from short style option switches to actual switch objects.
    attr_reader :short

    # Map from long style option switches to actual switch objects.
    attr_reader :long

    # List of all switches and summary string.
    attr_reader :list

    #
    # Just initializes all instance variables.
    #
    def initialize
      @atype = {}
      @short = OptionMap.new
      @long = OptionMap.new
      @list = []
    end

    #
    # See OptionParser.accept.
    #
    def accept(t, pat = /.*/m, &block)
      if pat
        pat.respond_to?(:match) or
          raise TypeError, "has no `match'", ParseError.filter_backtrace(caller(2))
      else
        pat = t if t.respond_to?(:match)
      end
      unless block
        block = pat.method(:convert).to_proc if pat.respond_to?(:convert)
      end
      @atype[t] = [pat, block]
    end

    #
    # See OptionParser.reject.
    #
    def reject(t)
      @atype.delete(t)
    end

    #
    # Adds +sw+ according to +sopts+, +lopts+ and +nlopts+.
    #
    # +sw+::     OptionParser::Switch instance to be added.
    # +sopts+::  Short style option list.
    # +lopts+::  Long style option list.
    # +nlopts+:: Negated long style options list.
    #
    def update(sw, sopts, lopts, nsw = nil, nlopts = nil)
      sopts.each {|o| @short[o] = sw} if sopts
      lopts.each {|o| @long[o] = sw} if lopts
      nlopts.each {|o| @long[o] = nsw} if nsw and nlopts
      used = @short.invert.update(@long.invert)
      @list.delete_if {|o| Switch === o and !used[o]}
    end
    private :update

    #
    # Inserts +switch+ at the head of the list, and associates short, long
    # and negated long options. Arguments are:
    #
    # +switch+::      OptionParser::Switch instance to be inserted.
    # +short_opts+::  List of short style options.
    # +long_opts+::   List of long style options.
    # +nolong_opts+:: List of long style options with "no-" prefix.
    #
    #   prepend(switch, short_opts, long_opts, nolong_opts)
    #
    def prepend(*args)
      update(*args)
      @list.unshift(args[0])
    end

    #
    # Appends +switch+ at the tail of the list, and associates short, long
    # and negated long options. Arguments are:
    #
    # +switch+::      OptionParser::Switch instance to be inserted.
    # +short_opts+::  List of short style options.
    # +long_opts+::   List of long style options.
    # +nolong_opts+:: List of long style options with "no-" prefix.
    #
    #   append(switch, short_opts, long_opts, nolong_opts)
    #
    def append(*args)
      update(*args)
      @list.push(args[0])
    end

    #
    # Searches +key+ in +id+ list. The result is returned or yielded if a
    # block is given. If it isn't found, nil is returned.
    #
    def search(id, key)
      if list = __send__(id)
        val = list.fetch(key) {return nil}
        block_given? ? yield(val) : val
      end
    end

    #
    # Searches list +id+ for +opt+ and the optional patterns for completion
    # +pat+. If +icase+ is true, the search is case insensitive. The result
    # is returned or yielded if a block is given. If it isn't found, nil is
    # returned.
    #
    def complete(id, opt, icase = false, *pat, &block)
      __send__(id).complete(opt, icase, *pat, &block)
    end

    #
    # Iterates over each option, passing the option to the +block+.
    #
    def each_option(&block)
      list.each(&block)
    end

    #
    # Creates the summary table, passing each line to the +block+ (without
    # newline). The arguments +args+ are passed along to the summarize
    # method which is called on every option.
    #
    def summarize(*args, &block)
      sum = []
      list.reverse_each do |opt|
        if opt.respond_to?(:summarize) # perhaps OptionParser::Switch
          s = []
          opt.summarize(*args) {|l| s << l}
          sum.concat(s.reverse)
        elsif !opt or opt.empty?
          sum << ""
        elsif opt.respond_to?(:each_line)
          sum.concat([*opt.each_line].reverse)
        else
          sum.concat([*opt.each].reverse)
        end
      end
      sum.reverse_each(&block)
    end

    def add_banner(to)  # :nodoc:
      list.each do |opt|
        if opt.respond_to?(:add_banner)
          opt.add_banner(to)
        end
      end
      to
    end

    def compsys(*args, &block)  # :nodoc:
      list.each do |opt|
        if opt.respond_to?(:compsys)
          opt.compsys(*args, &block)
        end
      end
    end
  end

  #
  # Hash with completion search feature. See OptionParser::Completion.
  #
  class CompletingHash < Hash
    include Completion

    #
    # Completion for hash key.
    #
    def match(key)
      *values = fetch(key) {
        raise AmbiguousArgument, catch(:ambiguous) {return complete(key)}
      }
      return key, *values
    end
  end

  # :stopdoc:

  #
  # Enumeration of acceptable argument styles. Possible values are:
  #
  # NO_ARGUMENT::       The switch takes no arguments. (:NONE)
  # REQUIRED_ARGUMENT:: The switch requires an argument. (:REQUIRED)
  # OPTIONAL_ARGUMENT:: The switch requires an optional argument. (:OPTIONAL)
  #
  # Use like --switch=argument (long style) or -Xargument (short style). For
  # short style, only portion matched to argument pattern is dealed as
  # argument.
  #
  ArgumentStyle = {}
  NoArgument.each {|el| ArgumentStyle[el] = Switch::NoArgument}
  RequiredArgument.each {|el| ArgumentStyle[el] = Switch::RequiredArgument}
  OptionalArgument.each {|el| ArgumentStyle[el] = Switch::OptionalArgument}
  ArgumentStyle.freeze

  #
  # Switches common used such as '--', and also provides default
  # argument classes
  #
  DefaultList = List.new
  DefaultList.short['-'] = Switch::NoArgument.new {}
  DefaultList.long[''] = Switch::NoArgument.new {throw :terminate}


  COMPSYS_HEADER = <<'XXX'      # :nodoc:

typeset -A opt_args
local context state line

_arguments -s -S \
XXX

  def compsys(to, name = File.basename($0)) # :nodoc:
    to << "#compdef #{name}\n"
    to << COMPSYS_HEADER
    visit(:compsys, {}, {}) {|o, d|
      to << %Q[  "#{o}[#{d.gsub(/[\"\[\]]/, '\\\\\&')}]" \\\n]
    }
    to << "  '*:file:_files' && return 0\n"
  end

  #
  # Default options for ARGV, which never appear in option summary.
  #
  Officious = {}

  #
  # --help
  # Shows option summary.
  #
  Officious['help'] = proc do |parser|
    Switch::NoArgument.new do |arg|
      puts parser.help
      exit
    end
  end

  #
  # --*-completion-bash=WORD
  # Shows candidates for command line completion.
  #
  Officious['*-completion-bash'] = proc do |parser|
    Switch::RequiredArgument.new do |arg|
      puts parser.candidate(arg)
      exit
    end
  end

  #
  # --*-completion-zsh[=NAME:FILE]
  # Creates zsh completion file.
  #
  Officious['*-completion-zsh'] = proc do |parser|
    Switch::OptionalArgument.new do |arg|
      parser.compsys(STDOUT, arg)
      exit
    end
  end

  #
  # --version
  # Shows version string if Version is defined.
  #
  Officious['version'] = proc do |parser|
    Switch::OptionalArgument.new do |pkg|
      if pkg
        begin
          require 'optparse/version'
        rescue LoadError
        else
          show_version(*pkg.split(/,/)) or
            abort("#{parser.program_name}: no version found in package #{pkg}")
          exit
        end
      end
      v = parser.ver or abort("#{parser.program_name}: version unknown")
      puts v
      exit
    end
  end

  # :startdoc:

  #
  # Class methods
  #

  #
  # Initializes a new instance and evaluates the optional block in context
  # of the instance. Arguments +args+ are passed to #new, see there for
  # description of parameters.
  #
  # This method is *deprecated*, its behavior corresponds to the older #new
  # method.
  #
  def self.with(*args, &block)
    opts = new(*args)
    opts.instance_eval(&block)
    opts
  end

  #
  # Returns an incremented value of +default+ according to +arg+.
  #
  def self.inc(arg, default = nil)
    case arg
    when Integer
      arg.nonzero?
    when nil
      default.to_i + 1
    end
  end
  def inc(*args)
    self.class.inc(*args)
  end

  #
  # Initializes the instance and yields itself if called with a block.
  #
  # +banner+:: Banner message.
  # +width+::  Summary width.
  # +indent+:: Summary indent.
  #
  def initialize(banner = nil, width = 32, indent = ' ' * 4)
    @stack = [DefaultList, List.new, List.new]
    @program_name = nil
    @banner = banner
    @summary_width = width
    @summary_indent = indent
    @default_argv = ARGV
    add_officious
    yield self if block_given?
  end

  def add_officious  # :nodoc:
    list = base()
    Officious.each do |opt, block|
      list.long[opt] ||= block.call(self)
    end
  end

  #
  # Terminates option parsing. Optional parameter +arg+ is a string pushed
  # back to be the first non-option argument.
  #
  def terminate(arg = nil)
    self.class.terminate(arg)
  end
  def self.terminate(arg = nil)
    throw :terminate, arg
  end

  @stack = [DefaultList]
  def self.top() DefaultList end

  #
  # Directs to accept specified class +t+. The argument string is passed to
  # the block in which it should be converted to the desired class.
  #
  # +t+::   Argument class specifier, any object including Class.
  # +pat+:: Pattern for argument, defaults to +t+ if it responds to match.
  #
  #   accept(t, pat, &block)
  #
  def accept(*args, &blk) top.accept(*args, &blk) end
  #
  # See #accept.
  #
  def self.accept(*args, &blk) top.accept(*args, &blk) end

  #
  # Directs to reject specified class argument.
  #
  # +t+:: Argument class specifier, any object including Class.
  #
  #   reject(t)
  #
  def reject(*args, &blk) top.reject(*args, &blk) end
  #
  # See #reject.
  #
  def self.reject(*args, &blk) top.reject(*args, &blk) end

  #
  # Instance methods
  #

  # Heading banner preceding summary.
  attr_writer :banner

  # Program name to be emitted in error message and default banner,
  # defaults to $0.
  attr_writer :program_name

  # Width for option list portion of summary. Must be Numeric.
  attr_accessor :summary_width

  # Indentation for summary. Must be String (or have + String method).
  attr_accessor :summary_indent

  # Strings to be parsed in default.
  attr_accessor :default_argv

  #
  # Heading banner preceding summary.
  #
  def banner
    unless @banner
      @banner = "Usage: #{program_name} [options]"
      visit(:add_banner, @banner)
    end
    @banner
  end

  #
  # Program name to be emitted in error message and default banner, defaults
  # to $0.
  #
  def program_name
    @program_name || File.basename($0, '.*')
  end

  # for experimental cascading :-)
  alias set_banner banner=
  alias set_program_name program_name=
  alias set_summary_width summary_width=
  alias set_summary_indent summary_indent=

  # Version
  attr_writer :version
  # Release code
  attr_writer :release

  #
  # Version
  #
  def version
    @version || (defined?(::Version) && ::Version)
  end

  #
  # Release code
  #
  def release
    @release || (defined?(::Release) && ::Release) || (defined?(::RELEASE) && ::RELEASE)
  end

  #
  # Returns version string from program_name, version and release.
  #
  def ver
    if v = version
      str = "#{program_name} #{[v].join('.')}"
      str << " (#{v})" if v = release
      str
    end
  end

  def warn(mesg = $!)
    super("#{program_name}: #{mesg}")
  end

  def abort(mesg = $!)
    super("#{program_name}: #{mesg}")
  end

  #
  # Subject of #on / #on_head, #accept / #reject
  #
  def top
    @stack[-1]
  end

  #
  # Subject of #on_tail.
  #
  def base
    @stack[1]
  end

  #
  # Pushes a new List.
  #
  def new
    @stack.push(List.new)
    if block_given?
      yield self
    else
      self
    end
  end

  #
  # Removes the last List.
  #
  def remove
    @stack.pop
  end

  #
  # Puts option summary into +to+ and returns +to+. Yields each line if
  # a block is given.
  #
  # +to+:: Output destination, which must have method <<. Defaults to [].
  # +width+:: Width of left side, defaults to @summary_width.
  # +max+:: Maximum length allowed for left side, defaults to +width+ - 1.
  # +indent+:: Indentation, defaults to @summary_indent.
  #
  def summarize(to = [], width = @summary_width, max = width - 1, indent = @summary_indent, &blk)
    blk ||= proc {|l| to << (l.index($/, -1) ? l : l + $/)}
    visit(:summarize, {}, {}, width, max, indent, &blk)
    to
  end

  #
  # Returns option summary string.
  #
  def help; summarize(banner.to_s.sub(/\n?\z/, "\n")) end
  alias to_s help

  #
  # Returns option summary list.
  #
  def to_a; summarize(banner.to_a.dup) end

  #
  # Checks if an argument is given twice, in which case an ArgumentError is
  # raised. Called from OptionParser#switch only.
  #
  # +obj+:: New argument.
  # +prv+:: Previously specified argument.
  # +msg+:: Exception message.
  #
  def notwice(obj, prv, msg)
    unless !prv or prv == obj
      raise(ArgumentError, "argument #{msg} given twice: #{obj}",
            ParseError.filter_backtrace(caller(2)))
    end
    obj
  end
  private :notwice

  SPLAT_PROC = proc {|*a| a.length <= 1 ? a.first : a}
  #
  # Creates an OptionParser::Switch from the parameters. The parsed argument
  # value is passed to the given block, where it can be processed.
  #
  # See at the beginning of OptionParser for some full examples.
  #
  # +opts+ can include the following elements:
  #
  # [Argument style:]
  #   One of the following:
  #     :NONE, :REQUIRED, :OPTIONAL
  #
  # [Argument pattern:]
  #   Acceptable option argument format, must be pre-defined with
  #   OptionParser.accept or OptionParser#accept, or Regexp. This can appear
  #   once or assigned as String if not present, otherwise causes an
  #   ArgumentError. Examples:
  #     Float, Time, Array
  #
  # [Possible argument values:]
  #   Hash or Array.
  #     [:text, :binary, :auto]
  #     %w[iso-2022-jp shift_jis euc-jp utf8 binary]
  #     { "jis" => "iso-2022-jp", "sjis" => "shift_jis" }
  #
  # [Long style switch:]
  #   Specifies a long style switch which takes a mandatory, optional or no
  #   argument. It's a string of the following form:
  #     "--switch=MANDATORY" or "--switch MANDATORY"
  #     "--switch[=OPTIONAL]"
  #     "--switch"
  #
  # [Short style switch:]
  #   Specifies short style switch which takes a mandatory, optional or no
  #   argument. It's a string of the following form:
  #     "-xMANDATORY"
  #     "-x[OPTIONAL]"
  #     "-x"
  #   There is also a special form which matches character range (not full
  #   set of regular expression):
  #     "-[a-z]MANDATORY"
  #     "-[a-z][OPTIONAL]"
  #     "-[a-z]"
  #
  # [Argument style and description:]
  #   Instead of specifying mandatory or optional arguments directly in the
  #   switch parameter, this separate parameter can be used.
  #     "=MANDATORY"
  #     "=[OPTIONAL]"
  #
  # [Description:]
  #   Description string for the option.
  #     "Run verbosely"
  #
  # [Handler:]
  #   Handler for the parsed argument value. Either give a block or pass a
  #   Proc or Method as an argument.
  #
  def make_switch(opts, block = nil)
    short, long, nolong, style, pattern, conv, not_pattern, not_conv, not_style = [], [], []
    ldesc, sdesc, desc, arg = [], [], []
    default_style = Switch::NoArgument
    default_pattern = nil
    klass = nil
    q, a = nil

    opts.each do |o|
      # argument class
      next if search(:atype, o) do |pat, c|
        klass = notwice(o, klass, 'type')
        if not_style and not_style != Switch::NoArgument
          not_pattern, not_conv = pat, c
        else
          default_pattern, conv = pat, c
        end
      end

      # directly specified pattern(any object possible to match)
      if (!(String === o || Symbol === o)) and o.respond_to?(:match)
        pattern = notwice(o, pattern, 'pattern')
        if pattern.respond_to?(:convert)
          conv = pattern.method(:convert).to_proc
        else
          conv = SPLAT_PROC
        end
        next
      end

      # anything others
      case o
      when Proc, Method
        block = notwice(o, block, 'block')
      when Array, Hash
        case pattern
        when CompletingHash
        when nil
          pattern = CompletingHash.new
          conv = pattern.method(:convert).to_proc if pattern.respond_to?(:convert)
        else
          raise ArgumentError, "argument pattern given twice"
        end
        o.each {|pat, *v| pattern[pat] = v.fetch(0) {pat}}
      when Module
        raise ArgumentError, "unsupported argument type: #{o}", ParseError.filter_backtrace(caller(4))
      when *ArgumentStyle.keys
        style = notwice(ArgumentStyle[o], style, 'style')
      when /^--no-([^\[\]=\s]*)(.+)?/
        q, a = $1, $2
        o = notwice(a ? Object : TrueClass, klass, 'type')
        not_pattern, not_conv = search(:atype, o) unless not_style
        not_style = (not_style || default_style).guess(arg = a) if a
        default_style = Switch::NoArgument
        default_pattern, conv = search(:atype, FalseClass) unless default_pattern
        ldesc << "--no-#{q}"
        long << 'no-' + (q = q.downcase)
        nolong << q
      when /^--\[no-\]([^\[\]=\s]*)(.+)?/
        q, a = $1, $2
        o = notwice(a ? Object : TrueClass, klass, 'type')
        if a
          default_style = default_style.guess(arg = a)
          default_pattern, conv = search(:atype, o) unless default_pattern
        end
        ldesc << "--[no-]#{q}"
        long << (o = q.downcase)
        not_pattern, not_conv = search(:atype, FalseClass) unless not_style
        not_style = Switch::NoArgument
        nolong << 'no-' + o
      when /^--([^\[\]=\s]*)(.+)?/
        q, a = $1, $2
        if a
          o = notwice(NilClass, klass, 'type')
          default_style = default_style.guess(arg = a)
          default_pattern, conv = search(:atype, o) unless default_pattern
        end
        ldesc << "--#{q}"
        long << (o = q.downcase)
      when /^-(\[\^?\]?(?:[^\\\]]|\\.)*\])(.+)?/
        q, a = $1, $2
        o = notwice(Object, klass, 'type')
        if a
          default_style = default_style.guess(arg = a)
          default_pattern, conv = search(:atype, o) unless default_pattern
        end
        sdesc << "-#{q}"
        short << Regexp.new(q)
      when /^-(.)(.+)?/
        q, a = $1, $2
        if a
          o = notwice(NilClass, klass, 'type')
          default_style = default_style.guess(arg = a)
          default_pattern, conv = search(:atype, o) unless default_pattern
        end
        sdesc << "-#{q}"
        short << q
      when /^=/
        style = notwice(default_style.guess(arg = o), style, 'style')
        default_pattern, conv = search(:atype, Object) unless default_pattern
      else
        desc.push(o)
      end
    end

    default_pattern, conv = search(:atype, default_style.pattern) unless default_pattern
    if !(short.empty? and long.empty?)
      s = (style || default_style).new(pattern || default_pattern,
                                       conv, sdesc, ldesc, arg, desc, block)
    elsif !block
      if style or pattern
        raise ArgumentError, "no switch given", ParseError.filter_backtrace(caller)
      end
      s = desc
    else
      short << pattern
      s = (style || default_style).new(pattern,
                                       conv, nil, nil, arg, desc, block)
    end
    return s, short, long,
      (not_style.new(not_pattern, not_conv, sdesc, ldesc, nil, desc, block) if not_style),
      nolong
  end

  def define(*opts, &block)
    top.append(*(sw = make_switch(opts, block)))
    sw[0]
  end

  #
  # Add option switch and handler. See #make_switch for an explanation of
  # parameters.
  #
  def on(*opts, &block)
    define(*opts, &block)
    self
  end
  alias def_option define

  def define_head(*opts, &block)
    top.prepend(*(sw = make_switch(opts, block)))
    sw[0]
  end

  #
  # Add option switch like with #on, but at head of summary.
  #
  def on_head(*opts, &block)
    define_head(*opts, &block)
    self
  end
  alias def_head_option define_head

  def define_tail(*opts, &block)
    base.append(*(sw = make_switch(opts, block)))
    sw[0]
  end

  #
  # Add option switch like with #on, but at tail of summary.
  #
  def on_tail(*opts, &block)
    define_tail(*opts, &block)
    self
  end
  alias def_tail_option define_tail

  #
  # Add separator in summary.
  #
  def separator(string)
    top.append(string, nil, nil)
  end

  #
  # Parses command line arguments +argv+ in order. When a block is given,
  # each non-option argument is yielded.
  #
  # Returns the rest of +argv+ left unparsed.
  #
  def order(*argv, &block)
    argv = argv[0].dup if argv.size == 1 and Array === argv[0]
    order!(argv, &block)
  end

  #
  # Same as #order, but removes switches destructively.
  #
  def order!(argv = default_argv, &nonopt)
    parse_in_order(argv, &nonopt)
  end

  def parse_in_order(argv = default_argv, setter = nil, &nonopt)  # :nodoc:
    opt, arg, val, rest = nil
    nonopt ||= proc {|a| throw :terminate, a}
    argv.unshift(arg) if arg = catch(:terminate) {
      while arg = argv.shift
        case arg
        # long option
        when /\A--([^=]*)(?:=(.*))?/m
          opt, rest = $1, $2
          begin
            sw, = complete(:long, opt, true)
          rescue ParseError
            raise $!.set_option(arg, true)
          end
          begin
            opt, cb, val = sw.parse(rest, argv) {|*exc| raise(*exc)}
            val = cb.call(val) if cb
            setter.call(sw.switch_name, val) if setter
          rescue ParseError
            raise $!.set_option(arg, rest)
          end

        # short option
        when /\A-(.)((=).*|.+)?/m
          opt, has_arg, eq, val, rest = $1, $3, $3, $2, $2
          begin
            sw, = search(:short, opt)
            unless sw
              begin
                sw, = complete(:short, opt)
                # short option matched.
                val = arg.sub(/\A-/, '')
                has_arg = true
              rescue InvalidOption
                # if no short options match, try completion with long
                # options.
                sw, = complete(:long, opt)
                eq ||= !rest
              end
            end
          rescue ParseError
            raise $!.set_option(arg, true)
          end
          begin
            opt, cb, val = sw.parse(val, argv) {|*exc| raise(*exc) if eq}
            raise InvalidOption, arg if has_arg and !eq and arg == "-#{opt}"
            argv.unshift(opt) if opt and (!rest or (opt = opt.sub(/\A-*/, '-')) != '-')
            val = cb.call(val) if cb
            setter.call(sw.switch_name, val) if setter
          rescue ParseError
            raise $!.set_option(arg, arg.length > 2)
          end

        # non-option argument
        else
          catch(:prune) do
            visit(:each_option) do |sw0|
              sw = sw0
              sw.block.call(arg) if Switch === sw and sw.match_nonswitch?(arg)
            end
            nonopt.call(arg)
          end
        end
      end

      nil
    }

    visit(:search, :short, nil) {|sw| sw.block.call(*argv) if !sw.pattern}

    argv
  end
  private :parse_in_order

  #
  # Parses command line arguments +argv+ in permutation mode and returns
  # list of non-option arguments.
  #
  def permute(*argv)
    argv = argv[0].dup if argv.size == 1 and Array === argv[0]
    permute!(argv)
  end

  #
  # Same as #permute, but removes switches destructively.
  #
  def permute!(argv = default_argv)
    nonopts = []
    order!(argv, &nonopts.method(:<<))
    argv[0, 0] = nonopts
    argv
  end

  #
  # Parses command line arguments +argv+ in order when environment variable
  # POSIXLY_CORRECT is set, and in permutation mode otherwise.
  #
  def parse(*argv)
    argv = argv[0].dup if argv.size == 1 and Array === argv[0]
    parse!(argv)
  end

  #
  # Same as #parse, but removes switches destructively.
  #
  def parse!(argv = default_argv)
    if ENV.include?('POSIXLY_CORRECT')
      order!(argv)
    else
      permute!(argv)
    end
  end

  #
  # Wrapper method for getopts.rb.
  #
  #   params = ARGV.getopts("ab:", "foo", "bar:")
  #   # params[:a] = true   # -a
  #   # params[:b] = "1"    # -b1
  #   # params[:foo] = "1"  # --foo
  #   # params[:bar] = "x"  # --bar x
  #
  def getopts(*args)
    argv = Array === args.first ? args.shift : default_argv
    single_options, *long_options = *args

    result = {}

    single_options.scan(/(.)(:)?/) do |opt, val|
      if val
        result[opt] = nil
        define("-#{opt} VAL")
      else
        result[opt] = false
        define("-#{opt}")
      end
    end if single_options

    long_options.each do |arg|
      opt, val = arg.split(':', 2)
      if val
        result[opt] = val.empty? ? nil : val
        define("--#{opt} VAL")
      else
        result[opt] = false
        define("--#{opt}")
      end
    end

    parse_in_order(argv, result.method(:[]=))
    result
  end

  #
  # See #getopts.
  #
  def self.getopts(*args)
    new.getopts(*args)
  end

  #
  # Traverses @stack, sending each element method +id+ with +args+ and
  # +block+.
  #
  def visit(id, *args, &block)
    @stack.reverse_each do |el|
      el.send(id, *args, &block)
    end
    nil
  end
  private :visit

  #
  # Searches +key+ in @stack for +id+ hash and returns or yields the result.
  #
  def search(id, key)
    block_given = block_given?
    visit(:search, id, key) do |k|
      return block_given ? yield(k) : k
    end
  end
  private :search

  #
  # Completes shortened long style option switch and returns pair of
  # canonical switch and switch descriptor OptionParser::Switch.
  #
  # +id+::    Searching table.
  # +opt+::   Searching key.
  # +icase+:: Search case insensitive if true.
  # +pat+::   Optional pattern for completion.
  #
  def complete(typ, opt, icase = false, *pat)
    if pat.empty?
      search(typ, opt) {|sw| return [sw, opt]} # exact match or...
    end
    raise AmbiguousOption, catch(:ambiguous) {
      visit(:complete, typ, opt, icase, *pat) {|o, *sw| return sw}
      raise InvalidOption, opt
    }
  end
  private :complete

  def candidate(word)
    list = []
    case word
    when /\A--/
      word, arg = word.split(/=/, 2)
      argpat = Completion.regexp(arg, false) if arg and !arg.empty?
      long = true
    when /\A-(!-)/
      short = true
    when /\A-/
      long = short = true
    end
    pat = Completion.regexp(word, true)
    visit(:each_option) do |opt|
      next unless Switch === opt
      opts = (long ? opt.long : []) + (short ? opt.short : [])
      opts = Completion.candidate(word, true, pat, &opts.method(:each)).map(&:first) if pat
      if /\A=/ =~ opt.arg
        opts.map! {|sw| sw + "="}
        if arg and CompletingHash === opt.pattern
          if opts = opt.pattern.candidate(arg, false, argpat)
            opts.map!(&:last)
          end
        end
      end
      list.concat(opts)
    end
    list
  end

  #
  # Loads options from file names as +filename+. Does nothing when the file
  # is not present. Returns whether successfully loaded.
  #
  # +filename+ defaults to basename of the program without suffix in a
  # directory ~/.options.
  #
  def load(filename = nil)
    begin
      filename ||= File.expand_path(File.basename($0, '.*'), '~/.options')
    rescue
      return false
    end
    begin
      parse(*IO.readlines(filename).each {|s| s.chomp!})
      true
    rescue Errno::ENOENT, Errno::ENOTDIR
      false
    end
  end

  #
  # Parses environment variable +env+ or its uppercase with splitting like a
  # shell.
  #
  # +env+ defaults to the basename of the program.
  #
  def environment(env = File.basename($0, '.*'))
    env = ENV[env] || ENV[env.upcase] or return
    require 'shellwords'
    parse(*Shellwords.shellwords(env))
  end

  #
  # Acceptable argument classes
  #

  #
  # Any string and no conversion. This is fall-back.
  #
  accept(Object) {|s,|s or s.nil?}

  accept(NilClass) {|s,|s}

  #
  # Any non-empty string, and no conversion.
  #
  accept(String, /.+/m) {|s,*|s}

  #
  # Ruby/C-like integer, octal for 0-7 sequence, binary for 0b, hexadecimal
  # for 0x, and decimal for others; with optional sign prefix. Converts to
  # Integer.
  #
  decimal = '\d+(?:_\d+)*'
  binary = 'b[01]+(?:_[01]+)*'
  hex = 'x[\da-f]+(?:_[\da-f]+)*'
  octal = "0(?:[0-7]*(?:_[0-7]+)*|#{binary}|#{hex})"
  integer = "#{octal}|#{decimal}"
  accept(Integer, %r"\A[-+]?(?:#{integer})"io) {|s,| Integer(s) if s}

  #
  # Float number format, and converts to Float.
  #
  float = "(?:#{decimal}(?:\\.(?:#{decimal})?)?|\\.#{decimal})(?:E[-+]?#{decimal})?"
  floatpat = %r"\A[-+]?#{float}"io
  accept(Float, floatpat) {|s,| s.to_f if s}

  #
  # Generic numeric format, converts to Integer for integer format, Float
  # for float format, and Rational for rational format.
  #
  real = "[-+]?(?:#{octal}|#{float})"
  accept(Numeric, /\A(#{real})(?:\/(#{real}))?/io) {|s, d, n|
    if n
      Rational(d, n)
    elsif s
      eval(s)
    end
  }

  #
  # Decimal integer format, to be converted to Integer.
  #
  DecimalInteger = /\A[-+]?#{decimal}/io
  accept(DecimalInteger) {|s,| s.to_i if s}

  #
  # Ruby/C like octal/hexadecimal/binary integer format, to be converted to
  # Integer.
  #
  OctalInteger = /\A[-+]?(?:[0-7]+(?:_[0-7]+)*|0(?:#{binary}|#{hex}))/io
  accept(OctalInteger) {|s,| s.oct if s}

  #
  # Decimal integer/float number format, to be converted to Integer for
  # integer format, Float for float format.
  #
  DecimalNumeric = floatpat     # decimal integer is allowed as float also.
  accept(DecimalNumeric) {|s,| eval(s) if s}

  #
  # Boolean switch, which means whether it is present or not, whether it is
  # absent or not with prefix no-, or it takes an argument
  # yes/no/true/false/+/-.
  #
  yesno = CompletingHash.new
  %w[- no false].each {|el| yesno[el] = false}
  %w[+ yes true].each {|el| yesno[el] = true}
  yesno['nil'] = false          # should be nil?
  accept(TrueClass, yesno) {|arg, val| val == nil or val}
  #
  # Similar to TrueClass, but defaults to false.
  #
  accept(FalseClass, yesno) {|arg, val| val != nil and val}

  #
  # List of strings separated by ",".
  #
  accept(Array) do |s,|
    if s
      s = s.split(',').collect {|ss| ss unless ss.empty?}
    end
    s
  end

  #
  # Regular expression with options.
  #
  accept(Regexp, %r"\A/((?:\\.|[^\\])*)/([[:alpha:]]+)?\z|.*") do |all, s, o|
    f = 0
    if o
      f |= Regexp::IGNORECASE if /i/ =~ o
      f |= Regexp::MULTILINE if /m/ =~ o
      f |= Regexp::EXTENDED if /x/ =~ o
      k = o.delete("imx")
      k = nil if k.empty?
    end
    Regexp.new(s || all, f, k)
  end

  #
  # Exceptions
  #

  #
  # Base class of exceptions from OptionParser.
  #
  class ParseError < RuntimeError
    # Reason which caused the error.
    Reason = 'parse error'.freeze

    def initialize(*args)
      @args = args
      @reason = nil
    end

    attr_reader :args
    attr_writer :reason

    #
    # Pushes back erred argument(s) to +argv+.
    #
    def recover(argv)
      argv[0, 0] = @args
      argv
    end

    def self.filter_backtrace(array)
      unless $DEBUG
        array.delete_if(&%r"\A#{Regexp.quote(__FILE__)}:"o.method(:=~))
      end
      array
    end

    def set_backtrace(array)
      super(self.class.filter_backtrace(array))
    end

    def set_option(opt, eq)
      if eq
        @args[0] = opt
      else
        @args.unshift(opt)
      end
      self
    end

    #
    # Returns error reason. Override this for I18N.
    #
    def reason
      @reason || self.class::Reason
    end

    def inspect
      "#<#{self.class.to_s}: #{args.join(' ')}>"
    end

    #
    # Default stringizing method to emit standard error message.
    #
    def message
      reason + ': ' + args.join(' ')
    end

    alias to_s message
  end

  #
  # Raises when ambiguously completable string is encountered.
  #
  class AmbiguousOption < ParseError
    const_set(:Reason, 'ambiguous option'.freeze)
  end

  #
  # Raises when there is an argument for a switch which takes no argument.
  #
  class NeedlessArgument < ParseError
    const_set(:Reason, 'needless argument'.freeze)
  end

  #
  # Raises when a switch with mandatory argument has no argument.
  #
  class MissingArgument < ParseError
    const_set(:Reason, 'missing argument'.freeze)
  end

  #
  # Raises when switch is undefined.
  #
  class InvalidOption < ParseError
    const_set(:Reason, 'invalid option'.freeze)
  end

  #
  # Raises when the given argument does not match required format.
  #
  class InvalidArgument < ParseError
    const_set(:Reason, 'invalid argument'.freeze)
  end

  #
  # Raises when the given argument word can't be completed uniquely.
  #
  class AmbiguousArgument < InvalidArgument
    const_set(:Reason, 'ambiguous argument'.freeze)
  end

  #
  # Miscellaneous
  #

  #
  # Extends command line arguments array (ARGV) to parse itself.
  #
  module Arguable

    #
    # Sets OptionParser object, when +opt+ is +false+ or +nil+, methods
    # OptionParser::Arguable#options and OptionParser::Arguable#options= are
    # undefined. Thus, there is no ways to access the OptionParser object
    # via the receiver object.
    #
    def options=(opt)
      unless @optparse = opt
        class << self
          undef_method(:options)
          undef_method(:options=)
        end
      end
    end

    #
    # Actual OptionParser object, automatically created if nonexistent.
    #
    # If called with a block, yields the OptionParser object and returns the
    # result of the block. If an OptionParser::ParseError exception occurs
    # in the block, it is rescued, a error message printed to STDERR and
    # +nil+ returned.
    #
    def options
      @optparse ||= OptionParser.new
      @optparse.default_argv = self
      block_given? or return @optparse
      begin
        yield @optparse
      rescue ParseError
        @optparse.warn $!
        nil
      end
    end

    #
    # Parses +self+ destructively in order and returns +self+ containing the
    # rest arguments left unparsed.
    #
    def order!(&blk) options.order!(self, &blk) end

    #
    # Parses +self+ destructively in permutation mode and returns +self+
    # containing the rest arguments left unparsed.
    #
    def permute!() options.permute!(self) end

    #
    # Parses +self+ destructively and returns +self+ containing the
    # rest arguments left unparsed.
    #
    def parse!() options.parse!(self) end

    #
    # Substitution of getopts is possible as follows. Also see
    # OptionParser#getopts.
    #
    #   def getopts(*args)
    #     ($OPT = ARGV.getopts(*args)).each do |opt, val|
    #       eval "$OPT_#{opt.gsub(/[^A-Za-z0-9_]/, '_')} = val"
    #     end
    #   rescue OptionParser::ParseError
    #   end
    #
    def getopts(*args)
      options.getopts(self, *args)
    end

    #
    # Initializes instance variable.
    #
    def self.extend_object(obj)
      super
      obj.instance_eval {@optparse = nil}
    end
    def initialize(*args)
      super
      @optparse = nil
    end
  end

  #
  # Acceptable argument classes. Now contains DecimalInteger, OctalInteger
  # and DecimalNumeric. See Acceptable argument classes (in source code).
  #
  module Acceptables
    const_set(:DecimalInteger, OptionParser::DecimalInteger)
    const_set(:OctalInteger, OptionParser::OctalInteger)
    const_set(:DecimalNumeric, OptionParser::DecimalNumeric)
  end
end

# ARGV is arguable by OptionParser
ARGV.extend(OptionParser::Arguable)

if $0 == __FILE__
  Version = OptionParser::Version
  ARGV.options {|q|
    q.parse!.empty? or print "what's #{ARGV.join(' ')}?\n"
  } or abort(ARGV.options.to_s)
end
