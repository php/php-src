#
#   shell/command-controller.rb -
#       $Release Version: 0.7 $
#       $Revision$
#       by Keiju ISHITSUKA(keiju@ruby-lang.org)
#
# --
#
#
#

require "e2mmap"
require "thread"

require "shell/error"
require "shell/filter"
require "shell/system-command"
require "shell/builtin-command"

class Shell
  class CommandProcessor
#    include Error

    #
    # initialize of Shell and related classes.
    #
    m = [:initialize, :expand_path]
    if Object.methods.first.kind_of?(String)
      NoDelegateMethods = m.collect{|x| x.id2name}
    else
      NoDelegateMethods = m
    end

    def self.initialize

      install_builtin_commands

      # define CommandProcessor#methods to Shell#methods and Filter#methods
      for m in CommandProcessor.instance_methods(false) - NoDelegateMethods
        add_delegate_command_to_shell(m)
      end

      def self.method_added(id)
        add_delegate_command_to_shell(id)
      end
    end

    #
    # include run file.
    #
    def self.run_config
      begin
        load File.expand_path("~/.rb_shell") if ENV.key?("HOME")
      rescue LoadError, Errno::ENOENT
      rescue
        print "load error: #{rc}\n"
        print $!.class, ": ", $!, "\n"
        for err in $@[0, $@.size - 2]
          print "\t", err, "\n"
        end
      end
    end

    def initialize(shell)
      @shell = shell
      @system_commands = {}
    end

    #
    # CommandProcessor#expand_path(path)
    #     path:   String
    #     return: String
    #   returns the absolute path for <path>
    #
    def expand_path(path)
      @shell.expand_path(path)
    end

    #
    # File related commands
    # Shell#foreach
    # Shell#open
    # Shell#unlink
    # Shell#test
    #
    # -
    #
    # CommandProcessor#foreach(path, rs)
    #     path: String
    #     rs:   String - record separator
    #     iterator
    #   Same as:
    #     File#foreach (when path is file)
    #     Dir#foreach (when path is directory)
    #   path is relative to pwd
    #
    def foreach(path = nil, *rs)
      path = "." unless path
      path = expand_path(path)

      if File.directory?(path)
        Dir.foreach(path){|fn| yield fn}
      else
        IO.foreach(path, *rs){|l| yield l}
      end
    end

    #
    # CommandProcessor#open(path, mode)
    #     path:   String
    #     mode:   String
    #     return: File or Dir
    #   Same as:
    #     File#open (when path is file)
    #     Dir#open  (when path is directory)
    #   mode has an effect only when path is a file
    #
    def open(path, mode = nil, perm = 0666, &b)
      path = expand_path(path)
      if File.directory?(path)
        Dir.open(path, &b)
      else
        if @shell.umask
          f = File.open(path, mode, perm)
          File.chmod(perm & ~@shell.umask, path)
          if block_given?
            f.each(&b)
          end
          f
        else
          f = File.open(path, mode, perm, &b)
        end
      end
    end
    #  public :open

    #
    # CommandProcessor#unlink(path)
    #   same as:
    #     Dir#unlink  (when path is directory)
    #     File#unlink (when path is file)
    #
    def unlink(path)
      @shell.check_point

      path = expand_path(path)
      if File.directory?(path)
        Dir.unlink(path)
      else
        IO.unlink(path)
      end
      Void.new(@shell)
    end

    #
    # CommandProcessor#test(command, file1, file2)
    # CommandProcessor#[command, file1, file2]
    #     command: char or String or Symbol
    #     file1:   String
    #     file2:   String(optional)
    #     return: Boolean
    #   same as:
    #     test()           (when command is char or length 1 string or symbol)
    #     FileTest.command (others)
    #   example:
    #     sh[?e, "foo"]
    #     sh[:e, "foo"]
    #     sh["e", "foo"]
    #     sh[:exists?, "foo"]
    #     sh["exists?", "foo"]
    #
    alias top_level_test test
    def test(command, file1, file2=nil)
      file1 = expand_path(file1)
      file2 = expand_path(file2) if file2
      command = command.id2name if command.kind_of?(Symbol)

      case command
      when Integer
        if file2
          top_level_test(command, file1, file2)
        else
          top_level_test(command, file1)
        end
      when String
        if command.size == 1
          if file2
            top_level_test(command, file1, file2)
          else
            top_level_test(command, file1)
          end
        else
          if file2
            FileTest.send(command, file1, file2)
          else
            FileTest.send(command, file1)
          end
        end
      end
    end
    alias [] test

    #
    # Dir related methods
    #
    # Shell#mkdir
    # Shell#rmdir
    #
    #--
    #
    # CommandProcessor#mkdir(*path)
    #     path: String
    #   same as Dir.mkdir()
    #
    def mkdir(*path)
      @shell.check_point
      notify("mkdir #{path.join(' ')}")

      perm = nil
      if path.last.kind_of?(Integer)
        perm = path.pop
      end
      for dir in path
        d = expand_path(dir)
        if perm
          Dir.mkdir(d, perm)
        else
          Dir.mkdir(d)
        end
        File.chmod(d, 0666 & ~@shell.umask) if @shell.umask
      end
      Void.new(@shell)
    end

    #
    # CommandProcessor#rmdir(*path)
    #     path: String
    #   same as Dir.rmdir()
    #
    def rmdir(*path)
      @shell.check_point
      notify("rmdir #{path.join(' ')}")

      for dir in path
        Dir.rmdir(expand_path(dir))
      end
      Void.new(@shell)
    end

    #
    # CommandProcessor#system(command, *opts)
    #     command: String
    #     opts:    String
    #     return:  SystemCommand
    #   Same as system() function
    #   example:
    #     print sh.system("ls", "-l")
    #     sh.system("ls", "-l") | sh.head > STDOUT
    #
    def system(command, *opts)
      if opts.empty?
        if command =~ /\*|\?|\{|\}|\[|\]|<|>|\(|\)|~|&|\||\\|\$|;|'|`|"|\n/
          return SystemCommand.new(@shell, find_system_command("sh"), "-c", command)
        else
          command, *opts = command.split(/\s+/)
        end
      end
      SystemCommand.new(@shell, find_system_command(command), *opts)
    end

    #
    # ProcessCommand#rehash
    #   clear command hash table.
    #
    def rehash
      @system_commands = {}
    end

    #
    # ProcessCommand#transact
    #
    def check_point
      @shell.process_controller.wait_all_jobs_execution
    end
    alias finish_all_jobs check_point

    def transact(&block)
      begin
        @shell.instance_eval(&block)
      ensure
        check_point
      end
    end

    #
    # internal commands
    #
    def out(dev = STDOUT, &block)
      dev.print transact(&block)
    end

    def echo(*strings)
      Echo.new(@shell, *strings)
    end

    def cat(*filenames)
      Cat.new(@shell, *filenames)
    end

    #   def sort(*filenames)
    #     Sort.new(self, *filenames)
    #   end

    def glob(pattern)
      Glob.new(@shell, pattern)
    end

    def append(to, filter)
      case to
      when String
        AppendFile.new(@shell, to, filter)
      when IO
        AppendIO.new(@shell, to, filter)
      else
        Shell.Fail Error::CantApplyMethod, "append", to.class
      end
    end

    def tee(file)
      Tee.new(@shell, file)
    end

    def concat(*jobs)
      Concat.new(@shell, *jobs)
    end

    # %pwd, %cwd -> @pwd
    def notify(*opts)
      Shell.notify(*opts) {|mes|
        yield mes if iterator?

        mes.gsub!("%pwd", "#{@cwd}")
        mes.gsub!("%cwd", "#{@cwd}")
      }
    end

    #
    # private functions
    #
    def find_system_command(command)
      return command if /^\// =~ command
      case path = @system_commands[command]
      when String
        if exists?(path)
          return path
        else
          Shell.Fail Error::CommandNotFound, command
        end
      when false
        Shell.Fail Error::CommandNotFound, command
      end

      for p in @shell.system_path
        path = join(p, command)
        if FileTest.exist?(path)
          @system_commands[command] = path
          return path
        end
      end
      @system_commands[command] = false
      Shell.Fail Error::CommandNotFound, command
    end

    #
    # CommandProcessor.def_system_command(command, path)
    #     command:  String
    #     path:     String
    #   define 'command()' method as method.
    #
    def self.def_system_command(command, path = command)
      begin
        eval((d = %Q[def #{command}(*opts)
                  SystemCommand.new(@shell, '#{path}', *opts)
               end]), nil, __FILE__, __LINE__ - 1)
      rescue SyntaxError
        Shell.notify "warn: Can't define #{command} path: #{path}."
      end
      Shell.notify "Define #{command} path: #{path}.", Shell.debug?
      Shell.notify("Definition of #{command}: ", d,
             Shell.debug.kind_of?(Integer) && Shell.debug > 1)
    end

    def self.undef_system_command(command)
      command = command.id2name if command.kind_of?(Symbol)
      remove_method(command)
      Shell.module_eval{remove_method(command)}
      Filter.module_eval{remove_method(command)}
      self
    end

    # define command alias
    # ex)
    # def_alias_command("ls_c", "ls", "-C", "-F")
    # def_alias_command("ls_c", "ls"){|*opts| ["-C", "-F", *opts]}
    #
    @alias_map = {}
    def self.alias_map
      @alias_map
    end
    def self.alias_command(ali, command, *opts)
      ali = ali.id2name if ali.kind_of?(Symbol)
      command = command.id2name if command.kind_of?(Symbol)
      begin
        if iterator?
          @alias_map[ali.intern] = proc

          eval((d = %Q[def #{ali}(*opts)
                          @shell.__send__(:#{command},
                                          *(CommandProcessor.alias_map[:#{ali}].call *opts))
                        end]), nil, __FILE__, __LINE__ - 1)

        else
           args = opts.collect{|opt| '"' + opt + '"'}.join(",")
           eval((d = %Q[def #{ali}(*opts)
                          @shell.__send__(:#{command}, #{args}, *opts)
                        end]), nil, __FILE__, __LINE__ - 1)
        end
      rescue SyntaxError
        Shell.notify "warn: Can't alias #{ali} command: #{command}."
        Shell.notify("Definition of #{ali}: ", d)
        raise
      end
      Shell.notify "Define #{ali} command: #{command}.", Shell.debug?
      Shell.notify("Definition of #{ali}: ", d,
             Shell.debug.kind_of?(Integer) && Shell.debug > 1)
      self
    end

    def self.unalias_command(ali)
      ali = ali.id2name if ali.kind_of?(Symbol)
      @alias_map.delete ali.intern
      undef_system_command(ali)
    end

    #
    # CommandProcessor.def_builtin_commands(delegation_class, command_specs)
    #     delegation_class: Class or Module
    #     command_specs: [[command_name, [argument,...]],...]
    #        command_name: String
    #        arguments:    String
    #           FILENAME?? -> expand_path(filename??)
    #           *FILENAME?? -> filename??.collect{|f|expand_path(f)}.join(", ")
    #   define command_name(argument,...) as
    #       delegation_class.command_name(argument,...)
    #
    def self.def_builtin_commands(delegation_class, command_specs)
      for meth, args in command_specs
        arg_str = args.collect{|arg| arg.downcase}.join(", ")
        call_arg_str = args.collect{
          |arg|
          case arg
          when /^(FILENAME.*)$/
            format("expand_path(%s)", $1.downcase)
          when /^(\*FILENAME.*)$/
            # \*FILENAME* -> filenames.collect{|fn| expand_path(fn)}.join(", ")
            $1.downcase + '.collect{|fn| expand_path(fn)}'
          else
            arg
          end
        }.join(", ")
        d = %Q[def #{meth}(#{arg_str})
                    #{delegation_class}.#{meth}(#{call_arg_str})
                 end]
        Shell.notify "Define #{meth}(#{arg_str})", Shell.debug?
        Shell.notify("Definition of #{meth}: ", d,
                     Shell.debug.kind_of?(Integer) && Shell.debug > 1)
        eval d
      end
    end

    #
    # CommandProcessor.install_system_commands(pre)
    #       pre: String - command name prefix
    # defines every command which belongs in default_system_path via
    # CommandProcessor.command().  It doesn't define already defined
    # methods twice.  By default, "pre_" is prefixes to each method
    # name.  Characters that may not be used in a method name are
    # all converted to '_'.  Definition errors are just ignored.
    #
    def self.install_system_commands(pre = "sys_")
      defined_meth = {}
      for m in Shell.methods
        defined_meth[m] = true
      end
      sh = Shell.new
      for path in Shell.default_system_path
        next unless sh.directory? path
        sh.cd path
        sh.foreach do
          |cn|
          if !defined_meth[pre + cn] && sh.file?(cn) && sh.executable?(cn)
            command = (pre + cn).gsub(/\W/, "_").sub(/^([0-9])/, '_\1')
            begin
              def_system_command(command, sh.expand_path(cn))
            rescue
              Shell.notify "warn: Can't define #{command} path: #{cn}"
            end
            defined_meth[command] = command
          end
        end
      end
    end

    #----------------------------------------------------------------------
    #
    #  class initializing methods  -
    #
    #----------------------------------------------------------------------
    def self.add_delegate_command_to_shell(id)
      id = id.intern if id.kind_of?(String)
      name = id.id2name
      if Shell.method_defined?(id)
        Shell.notify "warn: override definition of Shell##{name}."
        Shell.notify "warn: alias Shell##{name} to Shell##{name}_org.\n"
        Shell.module_eval "alias #{name}_org #{name}"
      end
      Shell.notify "method added: Shell##{name}.", Shell.debug?
      Shell.module_eval(%Q[def #{name}(*args, &block)
                            begin
                              @command_processor.__send__(:#{name}, *args, &block)
                            rescue Exception
                              $@.delete_if{|s| /:in `__getobj__'$/ =~ s} #`
                              $@.delete_if{|s| /^\\(eval\\):/ =~ s}
                            raise
                            end
                          end], __FILE__, __LINE__)

      if Shell::Filter.method_defined?(id)
        Shell.notify "warn: override definition of Shell::Filter##{name}."
        Shell.notify "warn: alias Shell##{name} to Shell::Filter##{name}_org."
        Filter.module_eval "alias #{name}_org #{name}"
      end
      Shell.notify "method added: Shell::Filter##{name}.", Shell.debug?
      Filter.module_eval(%Q[def #{name}(*args, &block)
                            begin
                              self | @shell.__send__(:#{name}, *args, &block)
                            rescue Exception
                              $@.delete_if{|s| /:in `__getobj__'$/ =~ s} #`
                              $@.delete_if{|s| /^\\(eval\\):/ =~ s}
                            raise
                            end
                          end], __FILE__, __LINE__)
    end

    #
    # define default builtin commands
    #
    def self.install_builtin_commands
      # method related File.
      # (exclude open/foreach/unlink)
      normal_delegation_file_methods = [
        ["atime", ["FILENAME"]],
        ["basename", ["fn", "*opts"]],
        ["chmod", ["mode", "*FILENAMES"]],
        ["chown", ["owner", "group", "*FILENAME"]],
        ["ctime", ["FILENAMES"]],
        ["delete", ["*FILENAMES"]],
        ["dirname", ["FILENAME"]],
        ["ftype", ["FILENAME"]],
        ["join", ["*items"]],
        ["link", ["FILENAME_O", "FILENAME_N"]],
        ["lstat", ["FILENAME"]],
        ["mtime", ["FILENAME"]],
        ["readlink", ["FILENAME"]],
        ["rename", ["FILENAME_FROM", "FILENAME_TO"]],
        #      ["size", ["FILENAME"]],
        ["split", ["pathname"]],
        ["stat", ["FILENAME"]],
        ["symlink", ["FILENAME_O", "FILENAME_N"]],
        ["truncate", ["FILENAME", "length"]],
        ["utime", ["atime", "mtime", "*FILENAMES"]]]

      def_builtin_commands(File, normal_delegation_file_methods)
      alias_method :rm, :delete

      # method related FileTest
      def_builtin_commands(FileTest,
                   FileTest.singleton_methods(false).collect{|m| [m, ["FILENAME"]]})

    end

  end
end
