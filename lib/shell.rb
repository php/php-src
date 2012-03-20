#
#   shell.rb -
#       $Release Version: 0.7 $
#       $Revision: 1.9 $
#       by Keiju ISHITSUKA(keiju@ruby-lang.org)
#
# --
#
#
#

require "e2mmap"

require "thread" unless defined?(Mutex)

require "forwardable"

require "shell/error"
require "shell/command-processor"
require "shell/process-controller"

# Shell implements an idiomatic Ruby interface for common UNIX shell commands.
class Shell
  @RCS_ID='-$Id: shell.rb,v 1.9 2002/03/04 12:01:10 keiju Exp keiju $-'

  include Error
  extend Exception2MessageMapper

#  @cascade = true
  # debug: true -> normal debug
  # debug: 1    -> eval definition debug
  # debug: 2    -> detail inspect debug
  @debug = false
  @verbose = true

  @debug_display_process_id = false
  @debug_display_thread_id = true
  @debug_output_mutex = Mutex.new

  class << Shell
    extend Forwardable

    attr_accessor :cascade, :debug, :verbose

#    alias cascade? cascade
    alias debug? debug
    alias verbose? verbose
    @verbose = true

    def debug=(val)
      @debug = val
      @verbose = val if val
    end

    # Creates a new Shell instance with the current working directory
    # set to +path+.
    def cd(path)
      new(path)
    end

    # Returns the directories in the current shell's PATH environment variable
    # as an array of directory names. This sets the system_path for all
    # instances of Shell.
    #
    # Example: If in your current shell, you did:
    #
    #   $ echo $PATH
    #   /usr/bin:/bin:/usr/local/bin
    #
    # Running this method in the above shell would then return:
    #
    #   ["/usr/bin", "/bin", "/usr/local/bin"]
    #
    def default_system_path
      if @default_system_path
        @default_system_path
      else
        ENV["PATH"].split(":")
      end
    end

    # Sets the system_path that new instances of Shell should have as their
    # initial system_path.
    #
    # +path+ should be an array of directory name strings.
    def default_system_path=(path)
      @default_system_path = path
    end

    def default_record_separator
      if @default_record_separator
        @default_record_separator
      else
        $/
      end
    end

    def default_record_separator=(rs)
      @default_record_separator = rs
    end

    # os resource mutex
    mutex_methods = ["unlock", "lock", "locked?", "synchronize", "try_lock", "exclusive_unlock"]
    for m in mutex_methods
      def_delegator("@debug_output_mutex", m, "debug_output_"+m.to_s)
    end

  end

  def initialize(pwd = Dir.pwd, umask = nil)
    @cwd = File.expand_path(pwd)
    @dir_stack = []
    @umask = umask

    @system_path = Shell.default_system_path
    @record_separator = Shell.default_record_separator

    @command_processor = CommandProcessor.new(self)
    @process_controller = ProcessController.new(self)

    @verbose = Shell.verbose
    @debug = Shell.debug
  end

  attr_reader :system_path

  # Sets the system path (the Shell instance's PATH environment variable).
  #
  # +path+ should be an array of directory name strings.
  def system_path=(path)
    @system_path = path
    rehash
  end

  attr_accessor :umask, :record_separator
  attr_accessor :verbose, :debug

  def debug=(val)
    @debug = val
    @verbose = val if val
  end

  alias verbose? verbose
  alias debug? debug

  attr_reader :command_processor
  attr_reader :process_controller

  def expand_path(path)
    File.expand_path(path, @cwd)
  end

  # Most Shell commands are defined via CommandProcessor

  #
  # Dir related methods
  #
  # Shell#cwd/dir/getwd/pwd
  # Shell#chdir/cd
  # Shell#pushdir/pushd
  # Shell#popdir/popd
  # Shell#mkdir
  # Shell#rmdir

  # Returns the current working directory.
  attr_reader :cwd
  alias dir cwd
  alias getwd cwd
  alias pwd cwd

  attr_reader :dir_stack
  alias dirs dir_stack

  # If called as iterator, it restores the current directory when the
  # block ends.
  def chdir(path = nil, verbose = @verbose)
    check_point

    if iterator?
      notify("chdir(with block) #{path}") if verbose
      cwd_old = @cwd
      begin
        chdir(path, nil)
        yield
      ensure
        chdir(cwd_old, nil)
      end
    else
      notify("chdir #{path}") if verbose
      path = "~" unless path
      @cwd = expand_path(path)
      notify "current dir: #{@cwd}"
      rehash
      Void.new(self)
    end
  end
  alias cd chdir

  def pushdir(path = nil, verbose = @verbose)
    check_point

    if iterator?
      notify("pushdir(with block) #{path}") if verbose
      pushdir(path, nil)
      begin
        yield
      ensure
        popdir
      end
    elsif path
      notify("pushdir #{path}") if verbose
      @dir_stack.push @cwd
      chdir(path, nil)
      notify "dir stack: [#{@dir_stack.join ', '}]"
      self
    else
      notify("pushdir") if verbose
      if pop = @dir_stack.pop
        @dir_stack.push @cwd
        chdir pop
        notify "dir stack: [#{@dir_stack.join ', '}]"
        self
      else
        Shell.Fail DirStackEmpty
      end
    end
    Void.new(self)
  end
  alias pushd pushdir

  def popdir
    check_point

    notify("popdir")
    if pop = @dir_stack.pop
      chdir pop
      notify "dir stack: [#{@dir_stack.join ', '}]"
      self
    else
      Shell.Fail DirStackEmpty
    end
    Void.new(self)
  end
  alias popd popdir

  #
  # process management
  #
  def jobs
    @process_controller.jobs
  end

  def kill(sig, command)
    @process_controller.kill_job(sig, command)
  end

  #
  # command definitions
  #
  def Shell.def_system_command(command, path = command)
    CommandProcessor.def_system_command(command, path)
  end

  def Shell.undef_system_command(command)
    CommandProcessor.undef_system_command(command)
  end

  def Shell.alias_command(ali, command, *opts, &block)
    CommandProcessor.alias_command(ali, command, *opts, &block)
  end

  def Shell.unalias_command(ali)
    CommandProcessor.unalias_command(ali)
  end

  def Shell.install_system_commands(pre = "sys_")
    CommandProcessor.install_system_commands(pre)
  end

  #
  def inspect
    if debug.kind_of?(Integer) && debug > 2
      super
    else
      to_s
    end
  end

  def self.notify(*opts)
    Shell::debug_output_synchronize do
      if opts[-1].kind_of?(String)
        yorn = verbose?
      else
        yorn = opts.pop
      end
      return unless yorn

      if @debug_display_thread_id
        if @debug_display_process_id
          prefix = "shell(##{Process.pid}:#{Thread.current.to_s.sub("Thread", "Th")}): "
        else
          prefix = "shell(#{Thread.current.to_s.sub("Thread", "Th")}): "
        end
      else
        prefix = "shell: "
      end
      _head = true
      STDERR.print opts.collect{|mes|
        mes = mes.dup
        yield mes if iterator?
        if _head
          _head = false
#         "shell" " + mes
          prefix + mes
        else
          " "* prefix.size + mes
        end
      }.join("\n")+"\n"
    end
  end

  CommandProcessor.initialize
  CommandProcessor.run_config
end
