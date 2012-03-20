require 'rbconfig'
require 'fileutils'

#--
# This a FileUtils extension that defines several additional commands to be
# added to the FileUtils utility functions.
module FileUtils
  # Path to the currently running Ruby program
  RUBY = File.join(
    RbConfig::CONFIG['bindir'],
    RbConfig::CONFIG['ruby_install_name'] + RbConfig::CONFIG['EXEEXT']).
    sub(/.*\s.*/m, '"\&"')

  OPT_TABLE['sh']  = %w(noop verbose)
  OPT_TABLE['ruby'] = %w(noop verbose)

  # Run the system command +cmd+. If multiple arguments are given the command
  # is not run with the shell (same semantics as Kernel::exec and
  # Kernel::system).
  #
  # Example:
  #   sh %{ls -ltr}
  #
  #   sh 'ls', 'file with spaces'
  #
  #   # check exit status after command runs
  #   sh %{grep pattern file} do |ok, res|
  #     if ! ok
  #       puts "pattern not found (status = #{res.exitstatus})"
  #     end
  #   end
  #
  def sh(*cmd, &block)
    options = (Hash === cmd.last) ? cmd.pop : {}
    shell_runner = block_given? ? block : create_shell_runner(cmd)
    set_verbose_option(options)
    options[:noop] ||= Rake::FileUtilsExt.nowrite_flag
    Rake.rake_check_options options, :noop, :verbose
    Rake.rake_output_message cmd.join(" ") if options[:verbose]

    unless options[:noop]
      res = rake_system(*cmd)
      status = $?
      status = PseudoStatus.new(1) if !res && status.nil?
      shell_runner.call(res, status)
    end
  end

  def create_shell_runner(cmd) # :nodoc:
    show_command = cmd.join(" ")
    show_command = show_command[0,42] + "..." unless $trace
    lambda { |ok, status|
      ok or fail "Command failed with status (#{status.exitstatus}): [#{show_command}]"
    }
  end
  private :create_shell_runner

  def set_verbose_option(options) # :nodoc:
    unless options.key? :verbose
      options[:verbose] =
        Rake::FileUtilsExt.verbose_flag == Rake::FileUtilsExt::DEFAULT ||
        Rake::FileUtilsExt.verbose_flag
    end
  end
  private :set_verbose_option

  def rake_system(*cmd) # :nodoc:
    Rake::AltSystem.system(*cmd)
  end
  private :rake_system

  # Run a Ruby interpreter with the given arguments.
  #
  # Example:
  #   ruby %{-pe '$_.upcase!' <README}
  #
  def ruby(*args,&block)
    options = (Hash === args.last) ? args.pop : {}
    if args.length > 1 then
      sh(*([RUBY] + args + [options]), &block)
    else
      sh("#{RUBY} #{args.first}", options, &block)
    end
  end

  LN_SUPPORTED = [true]

  #  Attempt to do a normal file link, but fall back to a copy if the link
  #  fails.
  def safe_ln(*args)
    unless LN_SUPPORTED[0]
      cp(*args)
    else
      begin
        ln(*args)
      rescue StandardError, NotImplementedError
        LN_SUPPORTED[0] = false
        cp(*args)
      end
    end
  end

  # Split a file path into individual directory names.
  #
  # Example:
  #   split_all("a/b/c") =>  ['a', 'b', 'c']
  #
  def split_all(path)
    head, tail = File.split(path)
    return [tail] if head == '.' || tail == '/'
    return [head, tail] if head == '/'
    return split_all(head) + [tail]
  end
end
