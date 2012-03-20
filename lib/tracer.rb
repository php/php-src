##
# = Tracer
#
# Tracer outputs a source level execution trace of a Ruby program. It does
# this by registering an event handler with <code>Kernel#set_trace_func</code>
# for processing incoming events.  It also provides methods for filtering
# unwanted trace output (see Tracer.add_filter, Tracer.on, and Tracer.off).
#
# == Example
#
# Consider the following ruby script
#
#   class A
#     def square(a)
#       return a*a
#     end
#   end
#
#   a = A.new
#   a.square(5)
#
# Running the above script using <code>ruby -r tracer example.rb</code> will
# output the following trace to STDOUT (Note you can also explicitly
# <code>require 'tracer'</code>)
#
#   #0:<internal:lib/rubygems/custom_require>:38:Kernel:<: -
#   #0:example.rb:3::-: class A
#   #0:example.rb:3::C: class A
#   #0:example.rb:4::-:   def square(a)
#   #0:example.rb:7::E: end
#   #0:example.rb:9::-: a = A.new
#   #0:example.rb:10::-: a.square(5)
#   #0:example.rb:4:A:>:   def square(a)
#   #0:example.rb:5:A:-:     return a*a
#   #0:example.rb:6:A:<:   end
#    |  |         | |  |
#    |  |         | |   ---------------------+ event
#    |  |         |  ------------------------+ class
#    |  |          --------------------------+ line
#    |   ------------------------------------+ filename
#     ---------------------------------------+ thread
#
# Symbol table used for displaying incoming events:
#
# <tt>}</tt>:: call a C-language routine
# <tt>{</tt>:: return from a C-language routine
# <tt>></tt>:: call a Ruby method
# <tt>C</tt>:: start a class or module definition
# <tt>E</tt>:: finish a class or module definition
# <tt>-</tt>:: execute code on a new line
# <tt>^</tt>:: raise an exception
# <tt><</tt>:: return from a Ruby method
#
# == Copyright
#
# by Keiju ISHITSUKA(keiju@ishitsuka.com)
#
#--
# $Release Version: 0.3$
# $Revision: 1.12 $
require "thread"

#
# tracer main class
#
class Tracer
  class << self
    # display additional debug information (defaults to false)
    attr_accessor :verbose
    alias verbose? verbose

    # output stream used to output trace (defaults to STDOUT)
    attr_accessor :stdout

    # mutex lock used by tracer for displaying trace output
    attr_reader :stdout_mutex

    # display process id in trace output (defaults to false)
    attr_accessor :display_process_id
    alias display_process_id? display_process_id

    # display thread id in trace output (defaults to true)
    attr_accessor :display_thread_id
    alias display_thread_id? display_thread_id

    # display C-routine calls in trace output (defaults to false)
    attr_accessor :display_c_call
    alias display_c_call? display_c_call
  end

  Tracer::stdout = STDOUT
  Tracer::verbose = false
  Tracer::display_process_id = false
  Tracer::display_thread_id = true
  Tracer::display_c_call = false

  @stdout_mutex = Mutex.new

  # Symbol table used for displaying trace information
  EVENT_SYMBOL = {
    "line" => "-",
    "call" => ">",
    "return" => "<",
    "class" => "C",
    "end" => "E",
    "raise" => "^",
    "c-call" => "}",
    "c-return" => "{",
    "unknown" => "?"
  }

  def initialize # :nodoc:
    @threads = Hash.new
    if defined? Thread.main
      @threads[Thread.main.object_id] = 0
    else
      @threads[Thread.current.object_id] = 0
    end

    @get_line_procs = {}

    @filters = []
  end

  def stdout # :nodoc:
    Tracer.stdout
  end

  def on # :nodoc:
    if block_given?
      on
      begin
        yield
      ensure
        off
      end
    else
      set_trace_func method(:trace_func).to_proc
      stdout.print "Trace on\n" if Tracer.verbose?
    end
  end

  def off # :nodoc:
    set_trace_func nil
    stdout.print "Trace off\n" if Tracer.verbose?
  end

  def add_filter(p = proc) # :nodoc:
    @filters.push p
  end

  def set_get_line_procs(file, p = proc) # :nodoc:
    @get_line_procs[file] = p
  end

  def get_line(file, line) # :nodoc:
    if p = @get_line_procs[file]
      return p.call(line)
    end

    unless list = SCRIPT_LINES__[file]
      begin
        f = File::open(file)
        begin
          SCRIPT_LINES__[file] = list = f.readlines
        ensure
          f.close
        end
      rescue
        SCRIPT_LINES__[file] = list = []
      end
    end

    if l = list[line - 1]
      l
    else
      "-\n"
    end
  end

  def get_thread_no # :nodoc:
    if no = @threads[Thread.current.object_id]
      no
    else
      @threads[Thread.current.object_id] = @threads.size
    end
  end

  def trace_func(event, file, line, id, binding, klass, *) # :nodoc:
    return if file == __FILE__

    for p in @filters
      return unless p.call event, file, line, id, binding, klass
    end

    return unless Tracer::display_c_call? or
      event != "c-call" && event != "c-return"

    Tracer::stdout_mutex.synchronize do
      if EVENT_SYMBOL[event]
        stdout.printf("<%d>", $$) if Tracer::display_process_id?
        stdout.printf("#%d:", get_thread_no) if Tracer::display_thread_id?
        if line == 0
          source = "?\n"
        else
          source = get_line(file, line)
        end
        printf("%s:%d:%s:%s: %s",
               file,
               line,
               klass || '',
               EVENT_SYMBOL[event],
               source)
      end
    end

  end

  # Reference to singleton instance of Tracer
  Single = new

  ##
  # Start tracing
  #
  # === Example
  #
  #   Tracer.on
  #   # code to trace here
  #   Tracer.off
  #
  # You can also pass a block:
  #
  #   Tracer.on {
  #     # trace everything in this block
  #   }

  def Tracer.on
    if block_given?
      Single.on{yield}
    else
      Single.on
    end
  end

  ##
  # Disable tracing

  def Tracer.off
    Single.off
  end

  ##
  # Register an event handler <code>p</code> which is called everytime a line
  # in +file_name+ is executed.
  #
  # Example:
  #
  #   Tracer.set_get_line_procs("example.rb", lambda { |line|
  #     puts "line number executed is #{line}"
  #   })

  def Tracer.set_get_line_procs(file_name, p = proc)
    Single.set_get_line_procs(file_name, p)
  end

  ##
  # Used to filter unwanted trace output
  #
  # Example which only outputs lines of code executed within the Kernel class:
  #
  #   Tracer.add_filter do |event, file, line, id, binding, klass, *rest|
  #     "Kernel" == klass.to_s
  #   end

  def Tracer.add_filter(p = proc)
    Single.add_filter(p)
  end
end

# :stopdoc:
SCRIPT_LINES__ = {} unless defined? SCRIPT_LINES__

if $0 == __FILE__
  # direct call

  $0 = ARGV[0]
  ARGV.shift
  Tracer.on
  require $0
else
  # call Tracer.on only if required by -r command-line option
  count = caller.count {|bt| /\/rubygems\/custom_require.rb:/ !~ bt}
  if (defined?(Gem) and count == 0) or
     (!defined?(Gem) and count <= 1)
    Tracer.on
  end
end
# :startdoc:
