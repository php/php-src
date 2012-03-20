 -- shell.rb
				$Release Version: 0.6.0 $
			   	$Revision$
			   	by Keiju ISHITSUKA(keiju@ishitsuka.com)

=begin

= What's shell.rb?

It realizes a wish to do execution of commands with filters and pipes
like sh/csh by using just native facilities of ruby.

= Main classes

== Shell

Every shell object has its own current working directory, and executes
each command as if it stands in the directory.

--- Shell#cwd
--- Shell#dir
--- Shell#getwd
--- Shell#pwd

      Returns the current directory

--- Shell#system_path

      Returns the command search path in an array

--- Shell#umask

      Returns the umask

== Filter

Any result of command exection is a Filter.  Filter include
Enumerable, therefore a Filter object can use all Enumerable
facilities.

= Main methods

== Command definitions

In order to execute a command on your OS, you need to define it as a
Shell method.

Alternatively, you can execute any command via Shell#system even if it
is not defined.

--- Shell.def_system_command(command, path = command)

      Defines a command.  Registers <path> as a Shell method
      <command>.

      ex)
      Shell.def_system_command "ls"
        Defines ls.

      Shell.def_system_command "sys_sort", "sort"
        Defines sys_sort as sort.

--- Shell.undef_system_command(command)

      Undefines a commmand

--- Shell.alias_command(ali, command, *opts) {...}

      Aliases a command.

      ex)
        Shell.alias_command "lsC", "ls", "-CBF", "--show-control-chars"
        Shell.alias_command("lsC", "ls"){|*opts| ["-CBF", "--show-control-chars", *opts]}

--- Shell.unalias_command(ali)

      Unaliases a command.

--- Shell.install_system_commands(pre = "sys_")

      Defines all commands in the default_system_path as Shell method,
      all with <pre> prefixed to their names.

== Creation

--- Shell.new

      Creates a Shell object which current directory is set to the
      process current directory.

--- Shell.cd(path)

      Creates a Shell object which current directory is set to
      <path>.

== Process management

--- Shell#jobs

      Returns a list of scheduled jobs.

--- Shell#kill sig, job

      Sends a signal <sig> to <job>.

== Current directory operations

--- Shell#cd(path, &block)
--- Shell#chdir

      Changes the current directory to <path>.  If a block is given,
      it restores the current directory when the block ends.

--- Shell#pushd(path = nil, &block)
--- Shell#pushdir

      Pushes the current directory to the directory stack, changing
      the current directory to <path>.  If <path> is omitted, it
      exchanges its current directory and the top of its directory
      stack.  If a block is given, it restores the current directory
      when the block ends.

--- Shell#popd
--- Shell#popdir

      Pops a directory from the directory stack, and sets the current
      directory to it.

== File and directory operations

--- Shell#foreach(path = nil, &block)

      Same as:
        File#foreach (when path is a file)
        Dir#foreach (when path is a directory)

--- Shell#open(path, mode)

      Same as:
        File#open (when path is a file)
        Dir#open (when path is a directory)

--- Shell#unlink(path)

      Same as:
        Dir#open (when path is a file)
        Dir#unlink (when path is a directory)

--- Shell#test(command, file1, file2)
--- Shell#[command, file1, file2]

      Same as test().
      ex)
          sh[?e, "foo"]
          sh[:e, "foo"]
          sh["e", "foo"]
          sh[:exists?, "foo"]
          sh["exists?", "foo"]

--- Shell#mkdir(*path)

      Same as Dir.mkdir (with multiple directories allowed)

--- Shell#rmdir(*path)

      Same as Dir.rmdir (with multiple directories allowed)

== Command execution

--- System#system(command, *opts)

      Executes <command> with <opts>.

      ex)
        print sh.system("ls", "-l")
        sh.system("ls", "-l") | sh.head > STDOUT

--- System#rehash

      Does rehash.

--- Shell#transact &block

      Executes a block as self.
      ex)
        sh.transact{system("ls", "-l") | head > STDOUT}

--- Shell#out(dev = STDOUT, &block)

      Does transact, with redirecting the result output to <dev>.

== Internal commands

--- Shell#echo(*strings)
--- Shell#cat(*files)
--- Shell#glob(patten)
--- Shell#tee(file)

      Return Filter objects, which are results of their execution.

--- Filter#each &block

      Iterates a block for each line of it.

--- Filter#<(src)

      Inputs from <src>, which is either a string of a file name or an
      IO.

--- Filter#>(to)

      Outputs to <to>, which is either a string of a file name or an
      IO.

--- Filter#>>(to)

      Appends the ouput to <to>, which is either a string of a file
      name or an IO.

--- Filter#|(filter)

      Processes a pipeline.

--- Filter#+(filter)

      (filter1 + filter2) outputs filter1, and then outputs filter2.

--- Filter#to_a
--- Filter#to_s

== Built-in commands

--- Shell#atime(file)
--- Shell#basename(file, *opt)
--- Shell#chmod(mode, *files)
--- Shell#chown(owner, group, *file)
--- Shell#ctime(file)
--- Shell#delete(*file)
--- Shell#dirname(file)
--- Shell#ftype(file)
--- Shell#join(*file)
--- Shell#link(file_from, file_to)
--- Shell#lstat(file)
--- Shell#mtime(file)
--- Shell#readlink(file)
--- Shell#rename(file_from, file_to)
--- Shell#split(file)
--- Shell#stat(file)
--- Shell#symlink(file_from, file_to)
--- Shell#truncate(file, length)
--- Shell#utime(atime, mtime, *file)

      Equivalent to the class methods of File with the same names.

--- Shell#blockdev?(file)
--- Shell#chardev?(file)
--- Shell#directory?(file)
--- Shell#executable?(file)
--- Shell#executable_real?(file)
--- Shell#exist?(file)/Shell#exists?(file)
--- Shell#file?(file)
--- Shell#grpowned?(file)
--- Shell#owned?(file)
--- Shell#pipe?(file)
--- Shell#readable?(file)
--- Shell#readable_real?(file)
--- Shell#setgid?(file)
--- Shell#setuid?(file)
--- Shell#size(file)/Shell#size?(file)
--- Shell#socket?(file)
--- Shell#sticky?(file)
--- Shell#symlink?(file)
--- Shell#writable?(file)
--- Shell#writable_real?(file)
--- Shell#zero?(file)

      Equivalent to the class methods of FileTest with the same names.

--- Shell#syscopy(filename_from, filename_to)
--- Shell#copy(filename_from, filename_to)
--- Shell#move(filename_from, filename_to)
--- Shell#compare(filename_from, filename_to)
--- Shell#safe_unlink(*filenames)
--- Shell#makedirs(*filenames)
--- Shell#install(filename_from, filename_to, mode)

      Equivalent to the class methods of FileTools with the same
      names.

      And also, there are some aliases for convenience:

--- Shell#cmp	<- Shell#compare
--- Shell#mv	<- Shell#move
--- Shell#cp	<- Shell#copy
--- Shell#rm_f	<- Shell#safe_unlink
--- Shell#mkpath	<- Shell#makedirs

= Samples

== ex1

  sh = Shell.cd("/tmp")
  sh.mkdir "shell-test-1" unless sh.exists?("shell-test-1")
  sh.cd("shell-test-1")
  for dir in ["dir1", "dir3", "dir5"]
    if !sh.exists?(dir)
      sh.mkdir dir
      sh.cd(dir) do
	f = sh.open("tmpFile", "w")
	f.print "TEST\n"
	f.close
      end
      print sh.pwd
    end
  end

== ex2

  sh = Shell.cd("/tmp")
  sh.transact do
    mkdir "shell-test-1" unless exists?("shell-test-1")
    cd("shell-test-1")
    for dir in ["dir1", "dir3", "dir5"]
      if !exists?(dir)
	mkdir dir
	cd(dir) do
	  f = open("tmpFile", "w")
	  f.print "TEST\n"
	  f.close
	end
	print pwd
      end
    end
  end

== ex3

  sh.cat("/etc/printcap") | sh.tee("tee1") > "tee2"
  (sh.cat < "/etc/printcap") | sh.tee("tee11") > "tee12"
  sh.cat("/etc/printcap") | sh.tee("tee1") >> "tee2"
  (sh.cat < "/etc/printcap") | sh.tee("tee11") >> "tee12"

== ex4

  print sh.cat("/etc/passwd").head.collect{|l| l =~ /keiju/}

=end
