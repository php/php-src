#
# = fileutils.rb
#
# Copyright (c) 2000-2007 Minero Aoki
#
# This program is free software.
# You can distribute/modify this program under the same terms of ruby.
#
# == module FileUtils
#
# Namespace for several file utility methods for copying, moving, removing, etc.
#
# === Module Functions
#
#   cd(dir, options)
#   cd(dir, options) {|dir| .... }
#   pwd()
#   mkdir(dir, options)
#   mkdir(list, options)
#   mkdir_p(dir, options)
#   mkdir_p(list, options)
#   rmdir(dir, options)
#   rmdir(list, options)
#   ln(old, new, options)
#   ln(list, destdir, options)
#   ln_s(old, new, options)
#   ln_s(list, destdir, options)
#   ln_sf(src, dest, options)
#   cp(src, dest, options)
#   cp(list, dir, options)
#   cp_r(src, dest, options)
#   cp_r(list, dir, options)
#   mv(src, dest, options)
#   mv(list, dir, options)
#   rm(list, options)
#   rm_r(list, options)
#   rm_rf(list, options)
#   install(src, dest, mode = <src's>, options)
#   chmod(mode, list, options)
#   chmod_R(mode, list, options)
#   chown(user, group, list, options)
#   chown_R(user, group, list, options)
#   touch(list, options)
#
# The <tt>options</tt> parameter is a hash of options, taken from the list
# <tt>:force</tt>, <tt>:noop</tt>, <tt>:preserve</tt>, and <tt>:verbose</tt>.
# <tt>:noop</tt> means that no changes are made.  The other two are obvious.
# Each method documents the options that it honours.
#
# All methods that have the concept of a "source" file or directory can take
# either one file or a list of files in that argument.  See the method
# documentation for examples.
#
# There are some `low level' methods, which do not accept any option:
#
#   copy_entry(src, dest, preserve = false, dereference = false)
#   copy_file(src, dest, preserve = false, dereference = true)
#   copy_stream(srcstream, deststream)
#   remove_entry(path, force = false)
#   remove_entry_secure(path, force = false)
#   remove_file(path, force = false)
#   compare_file(path_a, path_b)
#   compare_stream(stream_a, stream_b)
#   uptodate?(file, cmp_list)
#
# == module FileUtils::Verbose
#
# This module has all methods of FileUtils module, but it outputs messages
# before acting.  This equates to passing the <tt>:verbose</tt> flag to methods
# in FileUtils.
#
# == module FileUtils::NoWrite
#
# This module has all methods of FileUtils module, but never changes
# files/directories.  This equates to passing the <tt>:noop</tt> flag to methods
# in FileUtils.
#
# == module FileUtils::DryRun
#
# This module has all methods of FileUtils module, but never changes
# files/directories.  This equates to passing the <tt>:noop</tt> and
# <tt>:verbose</tt> flags to methods in FileUtils.
#

module FileUtils
  @fileutils_output  = $stderr
  @fileutils_label   = ''
  extend self

  #
  # This module has all methods of FileUtils module, but it outputs messages
  # before acting.  This equates to passing the <tt>:verbose</tt> flag to
  # methods in FileUtils.
  #
  module Verbose
    include FileUtils
    @fileutils_output  = $stderr
    @fileutils_label   = ''
    extend self
  end

  #
  # This module has all methods of FileUtils module, but never changes
  # files/directories.  This equates to passing the <tt>:noop</tt> flag
  # to methods in FileUtils.
  #
  module NoWrite
    include FileUtils
    @fileutils_output  = $stderr
    @fileutils_label   = ''
    extend self
  end

  #
  # This module has all methods of FileUtils module, but never changes
  # files/directories, with printing message before acting.
  # This equates to passing the <tt>:noop</tt> and <tt>:verbose</tt> flag
  # to methods in FileUtils.
  #
  module DryRun
    include FileUtils
    @fileutils_output  = $stderr
    @fileutils_label   = ''
    extend self
  end

  # This hash table holds command options.
  OPT_TABLE = {}   #:nodoc: internal use only

  #
  def self.define_command(name, *options)
    OPT_TABLE[name.to_s] = options

    if options.include?(:verbose)
      Verbose.module_eval(<<-EOS, __FILE__, __LINE__ + 1)
        def #{name}(*args)
          super(*fu_update_option(args, :verbose => true))
        end
      EOS
    end
    if options.include?(:noop)
      NoWrite.module_eval(<<-EOS, __FILE__, __LINE__ + 1)
        def #{name}(*args)
          super(*fu_update_option(args, :noop => true))
        end
      EOS
      DryRun.module_eval(<<-EOS, __FILE__, __LINE__ + 1)
        def #{name}(*args)
          super(*fu_update_option(args, :noop => true, :verbose => true))
        end
      EOS
    else
      NoWrite.module_eval(<<-EOS, __FILE__, __LINE__ + 1)
        def #{name}(*); end
      EOS
      DryRun.module_eval(<<-EOS, __FILE__, __LINE__ + 1)
        def #{name}(*); end
      EOS
    end

    [self, Verbose, DryRun, NoWrite].each do |mod|
      mod.module_eval(<<-EOS, __FILE__, __LINE__ + 1)
        private :#{name}
        class << self; public :#{name}; end
      EOS
    end
  end

  class << self
    private :define_command
  end

public

  #
  # Options: (none)
  #
  # Returns the name of the current directory.
  #
  def pwd
    Dir.pwd
  end

  alias getwd pwd

  define_command('pwd')
  define_command('getwd')

  #
  # Options: verbose
  #
  # Changes the current directory to the directory +dir+.
  #
  # If this method is called with block, resumes to the old
  # working directory after the block execution finished.
  #
  #   FileUtils.cd('/', :verbose => true)   # chdir and report it
  #
  #   FileUtils.cd('/') do  # chdir
  #     [...]               # do something
  #   end                   # return to original directory
  #
  def cd(dir, options = {}, &block) # :yield: dir
    fu_check_options options, OPT_TABLE['cd']
    fu_output_message "cd #{dir}" if options[:verbose]
    Dir.chdir(dir, &block)
    fu_output_message 'cd -' if options[:verbose] and block
  end

  alias chdir cd

  define_command('cd', :verbose)
  define_command('chdir', :verbose)

  #
  # Options: (none)
  #
  # Returns true if +newer+ is newer than all +old_list+.
  # Non-existent files are older than any file.
  #
  #   FileUtils.uptodate?('hello.o', %w(hello.c hello.h)) or \
  #       system 'make hello.o'
  #
  def uptodate?(new, old_list, options = nil)
    raise ArgumentError, 'uptodate? does not accept any option' if options

    return false unless File.exist?(new)
    new_time = File.mtime(new)
    old_list.each do |old|
      if File.exist?(old)
        return false unless new_time > File.mtime(old)
      end
    end
    true
  end

  define_command('uptodate?')

  #
  # Options: mode noop verbose
  #
  # Creates one or more directories.
  #
  #   FileUtils.mkdir 'test'
  #   FileUtils.mkdir %w( tmp data )
  #   FileUtils.mkdir 'notexist', :noop => true  # Does not really create.
  #   FileUtils.mkdir 'tmp', :mode => 0700
  #
  def mkdir(list, options = {})
    fu_check_options options, OPT_TABLE['mkdir']
    list = fu_list(list)
    fu_output_message "mkdir #{options[:mode] ? ('-m %03o ' % options[:mode]) : ''}#{list.join ' '}" if options[:verbose]
    return if options[:noop]

    list.each do |dir|
      fu_mkdir dir, options[:mode]
    end
  end

  define_command('mkdir', :mode, :noop, :verbose)

  #
  # Options: mode noop verbose
  #
  # Creates a directory and all its parent directories.
  # For example,
  #
  #   FileUtils.mkdir_p '/usr/local/lib/ruby'
  #
  # causes to make following directories, if it does not exist.
  #     * /usr
  #     * /usr/local
  #     * /usr/local/lib
  #     * /usr/local/lib/ruby
  #
  # You can pass several directories at a time in a list.
  #
  def mkdir_p(list, options = {})
    fu_check_options options, OPT_TABLE['mkdir_p']
    list = fu_list(list)
    fu_output_message "mkdir -p #{options[:mode] ? ('-m %03o ' % options[:mode]) : ''}#{list.join ' '}" if options[:verbose]
    return *list if options[:noop]

    list.map {|path| path.chomp(?/) }.each do |path|
      # optimize for the most common case
      begin
        fu_mkdir path, options[:mode]
        next
      rescue SystemCallError
        next if File.directory?(path)
      end

      stack = []
      until path == stack.last   # dirname("/")=="/", dirname("C:/")=="C:/"
        stack.push path
        path = File.dirname(path)
      end
      stack.reverse_each do |dir|
        begin
          fu_mkdir dir, options[:mode]
        rescue SystemCallError
          raise unless File.directory?(dir)
        end
      end
    end

    return *list
  end

  alias mkpath    mkdir_p
  alias makedirs  mkdir_p

  define_command('mkdir_p', :mode, :noop, :verbose)
  define_command('mkpath', :mode, :noop, :verbose)
  define_command('makedirs', :mode, :noop, :verbose)

private

  def fu_mkdir(path, mode)   #:nodoc:
    path = path.chomp(?/)
    if mode
      Dir.mkdir path, mode
      File.chmod mode, path
    else
      Dir.mkdir path
    end
  end

public

  #
  # Options: noop, verbose
  #
  # Removes one or more directories.
  #
  #   FileUtils.rmdir 'somedir'
  #   FileUtils.rmdir %w(somedir anydir otherdir)
  #   # Does not really remove directory; outputs message.
  #   FileUtils.rmdir 'somedir', :verbose => true, :noop => true
  #
  def rmdir(list, options = {})
    fu_check_options options, OPT_TABLE['rmdir']
    list = fu_list(list)
    parents = options[:parents]
    fu_output_message "rmdir #{parents ? '-p ' : ''}#{list.join ' '}" if options[:verbose]
    return if options[:noop]
    list.each do |dir|
      begin
        Dir.rmdir(dir = dir.chomp(?/))
        if parents
          until (parent = File.dirname(dir)) == '.' or parent == dir
            Dir.rmdir(dir)
          end
        end
      rescue Errno::ENOTEMPTY, Errno::ENOENT
      end
    end
  end

  define_command('rmdir', :parents, :noop, :verbose)

  #
  # Options: force noop verbose
  #
  # <b><tt>ln(old, new, options = {})</tt></b>
  #
  # Creates a hard link +new+ which points to +old+.
  # If +new+ already exists and it is a directory, creates a link +new/old+.
  # If +new+ already exists and it is not a directory, raises Errno::EEXIST.
  # But if :force option is set, overwrite +new+.
  #
  #   FileUtils.ln 'gcc', 'cc', :verbose => true
  #   FileUtils.ln '/usr/bin/emacs21', '/usr/bin/emacs'
  #
  # <b><tt>ln(list, destdir, options = {})</tt></b>
  #
  # Creates several hard links in a directory, with each one pointing to the
  # item in +list+.  If +destdir+ is not a directory, raises Errno::ENOTDIR.
  #
  #   include FileUtils
  #   cd '/sbin'
  #   FileUtils.ln %w(cp mv mkdir), '/bin'   # Now /sbin/cp and /bin/cp are linked.
  #
  def ln(src, dest, options = {})
    fu_check_options options, OPT_TABLE['ln']
    fu_output_message "ln#{options[:force] ? ' -f' : ''} #{[src,dest].flatten.join ' '}" if options[:verbose]
    return if options[:noop]
    fu_each_src_dest0(src, dest) do |s,d|
      remove_file d, true if options[:force]
      File.link s, d
    end
  end

  alias link ln

  define_command('ln', :force, :noop, :verbose)
  define_command('link', :force, :noop, :verbose)

  #
  # Options: force noop verbose
  #
  # <b><tt>ln_s(old, new, options = {})</tt></b>
  #
  # Creates a symbolic link +new+ which points to +old+.  If +new+ already
  # exists and it is a directory, creates a symbolic link +new/old+.  If +new+
  # already exists and it is not a directory, raises Errno::EEXIST.  But if
  # :force option is set, overwrite +new+.
  #
  #   FileUtils.ln_s '/usr/bin/ruby', '/usr/local/bin/ruby'
  #   FileUtils.ln_s 'verylongsourcefilename.c', 'c', :force => true
  #
  # <b><tt>ln_s(list, destdir, options = {})</tt></b>
  #
  # Creates several symbolic links in a directory, with each one pointing to the
  # item in +list+.  If +destdir+ is not a directory, raises Errno::ENOTDIR.
  #
  # If +destdir+ is not a directory, raises Errno::ENOTDIR.
  #
  #   FileUtils.ln_s Dir.glob('bin/*.rb'), '/home/aamine/bin'
  #
  def ln_s(src, dest, options = {})
    fu_check_options options, OPT_TABLE['ln_s']
    fu_output_message "ln -s#{options[:force] ? 'f' : ''} #{[src,dest].flatten.join ' '}" if options[:verbose]
    return if options[:noop]
    fu_each_src_dest0(src, dest) do |s,d|
      remove_file d, true if options[:force]
      File.symlink s, d
    end
  end

  alias symlink ln_s

  define_command('ln_s', :force, :noop, :verbose)
  define_command('symlink', :force, :noop, :verbose)

  #
  # Options: noop verbose
  #
  # Same as
  #   #ln_s(src, dest, :force)
  #
  def ln_sf(src, dest, options = {})
    fu_check_options options, OPT_TABLE['ln_sf']
    options = options.dup
    options[:force] = true
    ln_s src, dest, options
  end

  define_command('ln_sf', :noop, :verbose)

  #
  # Options: preserve noop verbose
  #
  # Copies a file content +src+ to +dest+.  If +dest+ is a directory,
  # copies +src+ to +dest/src+.
  #
  # If +src+ is a list of files, then +dest+ must be a directory.
  #
  #   FileUtils.cp 'eval.c', 'eval.c.org'
  #   FileUtils.cp %w(cgi.rb complex.rb date.rb), '/usr/lib/ruby/1.6'
  #   FileUtils.cp %w(cgi.rb complex.rb date.rb), '/usr/lib/ruby/1.6', :verbose => true
  #   FileUtils.cp 'symlink', 'dest'   # copy content, "dest" is not a symlink
  #
  def cp(src, dest, options = {})
    fu_check_options options, OPT_TABLE['cp']
    fu_output_message "cp#{options[:preserve] ? ' -p' : ''} #{[src,dest].flatten.join ' '}" if options[:verbose]
    return if options[:noop]
    fu_each_src_dest(src, dest) do |s, d|
      copy_file s, d, options[:preserve]
    end
  end

  alias copy cp

  define_command('cp', :preserve, :noop, :verbose)
  define_command('copy', :preserve, :noop, :verbose)

  #
  # Options: preserve noop verbose dereference_root remove_destination
  #
  # Copies +src+ to +dest+. If +src+ is a directory, this method copies
  # all its contents recursively. If +dest+ is a directory, copies
  # +src+ to +dest/src+.
  #
  # +src+ can be a list of files.
  #
  #   # Installing ruby library "mylib" under the site_ruby
  #   FileUtils.rm_r site_ruby + '/mylib', :force
  #   FileUtils.cp_r 'lib/', site_ruby + '/mylib'
  #
  #   # Examples of copying several files to target directory.
  #   FileUtils.cp_r %w(mail.rb field.rb debug/), site_ruby + '/tmail'
  #   FileUtils.cp_r Dir.glob('*.rb'), '/home/aamine/lib/ruby', :noop => true, :verbose => true
  #
  #   # If you want to copy all contents of a directory instead of the
  #   # directory itself, c.f. src/x -> dest/x, src/y -> dest/y,
  #   # use following code.
  #   FileUtils.cp_r 'src/.', 'dest'     # cp_r('src', 'dest') makes src/dest,
  #                                      # but this doesn't.
  #
  def cp_r(src, dest, options = {})
    fu_check_options options, OPT_TABLE['cp_r']
    fu_output_message "cp -r#{options[:preserve] ? 'p' : ''}#{options[:remove_destination] ? ' --remove-destination' : ''} #{[src,dest].flatten.join ' '}" if options[:verbose]
    return if options[:noop]
    options = options.dup
    options[:dereference_root] = true unless options.key?(:dereference_root)
    fu_each_src_dest(src, dest) do |s, d|
      copy_entry s, d, options[:preserve], options[:dereference_root], options[:remove_destination]
    end
  end

  define_command('cp_r', :preserve, :noop, :verbose, :dereference_root, :remove_destination)

  #
  # Copies a file system entry +src+ to +dest+.
  # If +src+ is a directory, this method copies its contents recursively.
  # This method preserves file types, c.f. symlink, directory...
  # (FIFO, device files and etc. are not supported yet)
  #
  # Both of +src+ and +dest+ must be a path name.
  # +src+ must exist, +dest+ must not exist.
  #
  # If +preserve+ is true, this method preserves owner, group, permissions
  # and modified time.
  #
  # If +dereference_root+ is true, this method dereference tree root.
  #
  # If +remove_destination+ is true, this method removes each destination file before copy.
  #
  def copy_entry(src, dest, preserve = false, dereference_root = false, remove_destination = false)
    Entry_.new(src, nil, dereference_root).traverse do |ent|
      destent = Entry_.new(dest, ent.rel, false)
      File.unlink destent.path if remove_destination && File.file?(destent.path)
      ent.copy destent.path
      ent.copy_metadata destent.path if preserve
    end
  end

  define_command(:copy_entry)

  #
  # Copies file contents of +src+ to +dest+.
  # Both of +src+ and +dest+ must be a path name.
  #
  def copy_file(src, dest, preserve = false, dereference = true)
    ent = Entry_.new(src, nil, dereference)
    ent.copy_file dest
    ent.copy_metadata dest if preserve
  end

  define_command(:copy_file)

  #
  # Copies stream +src+ to +dest+.
  # +src+ must respond to #read(n) and
  # +dest+ must respond to #write(str).
  #
  def copy_stream(src, dest)
    IO.copy_stream(src, dest)
  end

  define_command(:copy_stream)

  #
  # Options: force noop verbose
  #
  # Moves file(s) +src+ to +dest+.  If +file+ and +dest+ exist on the different
  # disk partition, the file is copied then the original file is removed.
  #
  #   FileUtils.mv 'badname.rb', 'goodname.rb'
  #   FileUtils.mv 'stuff.rb', '/notexist/lib/ruby', :force => true  # no error
  #
  #   FileUtils.mv %w(junk.txt dust.txt), '/home/aamine/.trash/'
  #   FileUtils.mv Dir.glob('test*.rb'), 'test', :noop => true, :verbose => true
  #
  def mv(src, dest, options = {})
    fu_check_options options, OPT_TABLE['mv']
    fu_output_message "mv#{options[:force] ? ' -f' : ''} #{[src,dest].flatten.join ' '}" if options[:verbose]
    return if options[:noop]
    fu_each_src_dest(src, dest) do |s, d|
      destent = Entry_.new(d, nil, true)
      begin
        if destent.exist?
          if destent.directory?
            raise Errno::EEXIST, dest
          else
            destent.remove_file if rename_cannot_overwrite_file?
          end
        end
        begin
          File.rename s, d
        rescue Errno::EXDEV
          copy_entry s, d, true
          if options[:secure]
            remove_entry_secure s, options[:force]
          else
            remove_entry s, options[:force]
          end
        end
      rescue SystemCallError
        raise unless options[:force]
      end
    end
  end

  alias move mv

  define_command('mv', :force, :noop, :verbose, :secure)
  define_command('move', :force, :noop, :verbose, :secure)

private

  def rename_cannot_overwrite_file?   #:nodoc:
    /cygwin|mswin|mingw|bccwin|emx/ =~ RUBY_PLATFORM
  end

public

  #
  # Options: force noop verbose
  #
  # Remove file(s) specified in +list+.  This method cannot remove directories.
  # All StandardErrors are ignored when the :force option is set.
  #
  #   FileUtils.rm %w( junk.txt dust.txt )
  #   FileUtils.rm Dir.glob('*.so')
  #   FileUtils.rm 'NotExistFile', :force => true   # never raises exception
  #
  def rm(list, options = {})
    fu_check_options options, OPT_TABLE['rm']
    list = fu_list(list)
    fu_output_message "rm#{options[:force] ? ' -f' : ''} #{list.join ' '}" if options[:verbose]
    return if options[:noop]

    list.each do |path|
      remove_file path, options[:force]
    end
  end

  alias remove rm

  define_command('rm', :force, :noop, :verbose)
  define_command('remove', :force, :noop, :verbose)

  #
  # Options: noop verbose
  #
  # Equivalent to
  #
  #   #rm(list, :force => true)
  #
  def rm_f(list, options = {})
    fu_check_options options, OPT_TABLE['rm_f']
    options = options.dup
    options[:force] = true
    rm list, options
  end

  alias safe_unlink rm_f

  define_command('rm_f', :noop, :verbose)
  define_command('safe_unlink', :noop, :verbose)

  #
  # Options: force noop verbose secure
  #
  # remove files +list+[0] +list+[1]... If +list+[n] is a directory,
  # removes its all contents recursively. This method ignores
  # StandardError when :force option is set.
  #
  #   FileUtils.rm_r Dir.glob('/tmp/*')
  #   FileUtils.rm_r '/', :force => true          #  :-)
  #
  # WARNING: This method causes local vulnerability
  # if one of parent directories or removing directory tree are world
  # writable (including /tmp, whose permission is 1777), and the current
  # process has strong privilege such as Unix super user (root), and the
  # system has symbolic link.  For secure removing, read the documentation
  # of #remove_entry_secure carefully, and set :secure option to true.
  # Default is :secure=>false.
  #
  # NOTE: This method calls #remove_entry_secure if :secure option is set.
  # See also #remove_entry_secure.
  #
  def rm_r(list, options = {})
    fu_check_options options, OPT_TABLE['rm_r']
    # options[:secure] = true unless options.key?(:secure)
    list = fu_list(list)
    fu_output_message "rm -r#{options[:force] ? 'f' : ''} #{list.join ' '}" if options[:verbose]
    return if options[:noop]
    list.each do |path|
      if options[:secure]
        remove_entry_secure path, options[:force]
      else
        remove_entry path, options[:force]
      end
    end
  end

  define_command('rm_r', :force, :noop, :verbose, :secure)

  #
  # Options: noop verbose secure
  #
  # Equivalent to
  #
  #   #rm_r(list, :force => true)
  #
  # WARNING: This method causes local vulnerability.
  # Read the documentation of #rm_r first.
  #
  def rm_rf(list, options = {})
    fu_check_options options, OPT_TABLE['rm_rf']
    options = options.dup
    options[:force] = true
    rm_r list, options
  end

  alias rmtree rm_rf

  define_command('rm_rf', :noop, :verbose, :secure)
  define_command('rmtree', :noop, :verbose, :secure)

  #
  # This method removes a file system entry +path+.  +path+ shall be a
  # regular file, a directory, or something.  If +path+ is a directory,
  # remove it recursively.  This method is required to avoid TOCTTOU
  # (time-of-check-to-time-of-use) local security vulnerability of #rm_r.
  # #rm_r causes security hole when:
  #
  #   * Parent directory is world writable (including /tmp).
  #   * Removing directory tree includes world writable directory.
  #   * The system has symbolic link.
  #
  # To avoid this security hole, this method applies special preprocess.
  # If +path+ is a directory, this method chown(2) and chmod(2) all
  # removing directories.  This requires the current process is the
  # owner of the removing whole directory tree, or is the super user (root).
  #
  # WARNING: You must ensure that *ALL* parent directories cannot be
  # moved by other untrusted users.  For example, parent directories
  # should not be owned by untrusted users, and should not be world
  # writable except when the sticky bit set.
  #
  # WARNING: Only the owner of the removing directory tree, or Unix super
  # user (root) should invoke this method.  Otherwise this method does not
  # work.
  #
  # For details of this security vulnerability, see Perl's case:
  #
  #   http://www.cve.mitre.org/cgi-bin/cvename.cgi?name=CAN-2005-0448
  #   http://www.cve.mitre.org/cgi-bin/cvename.cgi?name=CAN-2004-0452
  #
  # For fileutils.rb, this vulnerability is reported in [ruby-dev:26100].
  #
  def remove_entry_secure(path, force = false)
    unless fu_have_symlink?
      remove_entry path, force
      return
    end
    fullpath = File.expand_path(path)
    st = File.lstat(fullpath)
    unless st.directory?
      File.unlink fullpath
      return
    end
    # is a directory.
    parent_st = File.stat(File.dirname(fullpath))
    unless parent_st.world_writable?
      remove_entry path, force
      return
    end
    unless parent_st.sticky?
      raise ArgumentError, "parent directory is world writable, FileUtils#remove_entry_secure does not work; abort: #{path.inspect} (parent directory mode #{'%o' % parent_st.mode})"
    end
    # freeze tree root
    euid = Process.euid
    File.open(fullpath + '/.') {|f|
      unless fu_stat_identical_entry?(st, f.stat)
        # symlink (TOC-to-TOU attack?)
        File.unlink fullpath
        return
      end
      f.chown euid, -1
      f.chmod 0700
      unless fu_stat_identical_entry?(st, File.lstat(fullpath))
        # TOC-to-TOU attack?
        File.unlink fullpath
        return
      end
    }
    # ---- tree root is frozen ----
    root = Entry_.new(path)
    root.preorder_traverse do |ent|
      if ent.directory?
        ent.chown euid, -1
        ent.chmod 0700
      end
    end
    root.postorder_traverse do |ent|
      begin
        ent.remove
      rescue
        raise unless force
      end
    end
  rescue
    raise unless force
  end

  define_command(:remove_entry_secure)

private

  def fu_have_symlink?   #:nodoc:
    File.symlink nil, nil
  rescue NotImplementedError
    return false
  rescue TypeError
    return true
  end

  def fu_stat_identical_entry?(a, b)   #:nodoc:
    a.dev == b.dev and a.ino == b.ino
  end

public

  #
  # This method removes a file system entry +path+.
  # +path+ might be a regular file, a directory, or something.
  # If +path+ is a directory, remove it recursively.
  #
  # See also #remove_entry_secure.
  #
  def remove_entry(path, force = false)
    Entry_.new(path).postorder_traverse do |ent|
      begin
        ent.remove
      rescue
        raise unless force
      end
    end
  rescue
    raise unless force
  end

  define_command(:remove_entry)

  #
  # Removes a file +path+.
  # This method ignores StandardError if +force+ is true.
  #
  def remove_file(path, force = false)
    Entry_.new(path).remove_file
  rescue
    raise unless force
  end

  define_command(:remove_file)

  #
  # Removes a directory +dir+ and its contents recursively.
  # This method ignores StandardError if +force+ is true.
  #
  def remove_dir(path, force = false)
    remove_entry path, force   # FIXME?? check if it is a directory
  end

  define_command(:remove_dir)

  #
  # Returns true if the contents of a file A and a file B are identical.
  #
  #   FileUtils.compare_file('somefile', 'somefile')  #=> true
  #   FileUtils.compare_file('/bin/cp', '/bin/mv')    #=> maybe false
  #
  def compare_file(a, b)
    return false unless File.size(a) == File.size(b)
    File.open(a, 'rb') {|fa|
      File.open(b, 'rb') {|fb|
        return compare_stream(fa, fb)
      }
    }
  end

  alias identical? compare_file
  alias cmp compare_file

  define_command(:compare_file)
  define_command(:identical?)
  define_command(:cmp)

  #
  # Returns true if the contents of a stream +a+ and +b+ are identical.
  #
  def compare_stream(a, b)
    bsize = fu_stream_blksize(a, b)
    sa = sb = nil
    while sa == sb
      sa = a.read(bsize)
      sb = b.read(bsize)
      unless sa and sb
        if sa.nil? and sb.nil?
          return true
        end
      end
    end
    false
  end

  define_command(:compare_stream)

  #
  # Options: mode preserve noop verbose
  #
  # If +src+ is not same as +dest+, copies it and changes the permission
  # mode to +mode+.  If +dest+ is a directory, destination is +dest+/+src+.
  # This method removes destination before copy.
  #
  #   FileUtils.install 'ruby', '/usr/local/bin/ruby', :mode => 0755, :verbose => true
  #   FileUtils.install 'lib.rb', '/usr/local/lib/ruby/site_ruby', :verbose => true
  #
  def install(src, dest, options = {})
    fu_check_options options, OPT_TABLE['install']
    fu_output_message "install -c#{options[:preserve] && ' -p'}#{options[:mode] ? (' -m 0%o' % options[:mode]) : ''} #{[src,dest].flatten.join ' '}" if options[:verbose]
    return if options[:noop]
    fu_each_src_dest(src, dest) do |s, d, st|
      unless File.exist?(d) and compare_file(s, d)
        remove_file d, true
        copy_file s, d
        File.utime st.atime, st.mtime, d if options[:preserve]
        File.chmod options[:mode], d if options[:mode]
      end
    end
  end

  define_command('install', :mode, :preserve, :noop, :verbose)

private

  def user_mask(target)  #:nodoc:
    mask = 0
    target.each_byte do |byte_chr|
      case byte_chr.chr
        when "u"
          mask |= 04700
        when "g"
          mask |= 02070
        when "o"
          mask |= 01007
        when "a"
          mask |= 07777
      end
    end
    mask
  end

  def mode_mask(mode, path)  #:nodoc:
    mask = 0
    mode.each_byte do |byte_chr|
      case byte_chr.chr
        when "r"
          mask |= 0444
        when "w"
          mask |= 0222
        when "x"
          mask |= 0111
        when "X"
          mask |= 0111 if FileTest::directory? path
        when "s"
          mask |= 06000
        when "t"
          mask |= 01000
      end
    end
    mask
  end

  def symbolic_modes_to_i(modes, path)  #:nodoc:
    current_mode = (File.stat(path).mode & 07777)
    modes.split(/,/).inject(0) do |mode, mode_sym|
      mode_sym = "a#{mode_sym}" if mode_sym =~ %r!^[+-=]!
      target, mode = mode_sym.split %r![+-=]!
      user_mask = user_mask(target)
      mode_mask = mode_mask(mode ? mode : "", path)

      case mode_sym
        when /=/
          current_mode &= ~(user_mask)
          current_mode |= user_mask & mode_mask
        when /\+/
          current_mode |= user_mask & mode_mask
        when /-/
          current_mode &= ~(user_mask & mode_mask)
      end
    end
  end

  def fu_mode(mode, path)  #:nodoc:
    mode.is_a?(String) ? symbolic_modes_to_i(mode, path) : mode
  end

public

  #
  # Options: noop verbose
  #
  # Changes permission bits on the named files (in +list+) to the bit pattern
  # represented by +mode+.
  #
  # +mode+ is the symbolic and absolute mode can be used.
  #
  # Absolute mode is
  #   FileUtils.chmod 0755, 'somecommand'
  #   FileUtils.chmod 0644, %w(my.rb your.rb his.rb her.rb)
  #   FileUtils.chmod 0755, '/usr/bin/ruby', :verbose => true
  #
  # Symbolic mode is
  #   FileUtils.chmod "u=wrx,go=rx", 'somecommand'
  #   FileUtils.chmod "u=wr,go=rr", %w(my.rb your.rb his.rb her.rb)
  #   FileUtils.chmod "u=wrx,go=rx", '/usr/bin/ruby', :verbose => true
  #
  #   "a" is user, group, other mask.
  #   "u" is user's mask.
  #   "g" is group's mask.
  #   "o" is other's mask.
  #   "w" is write permission.
  #   "r" is read permission.
  #   "x" is execute permission.
  #   "s" is uid, gid.
  #   "t" is sticky bit.
  #   "+" is added to a class given the specified mode.
  #   "-" Is removed from a given class given mode.
  #   "=" Is the exact nature of the class will be given a specified mode.

  def chmod(mode, list, options = {})
    fu_check_options options, OPT_TABLE['chmod']
    list = fu_list(list)
    fu_output_message sprintf('chmod %o %s', mode, list.join(' ')) if options[:verbose]
    return if options[:noop]
    list.each do |path|
      Entry_.new(path).chmod(fu_mode(mode, path))
    end
  end

  define_command('chmod', :noop, :verbose)

  #
  # Options: noop verbose force
  #
  # Changes permission bits on the named files (in +list+)
  # to the bit pattern represented by +mode+.
  #
  #   FileUtils.chmod_R 0700, "/tmp/app.#{$$}"
  #   FileUtils.chmod_R "u=wrx", "/tmp/app.#{$$}"
  #
  def chmod_R(mode, list, options = {})
    fu_check_options options, OPT_TABLE['chmod_R']
    list = fu_list(list)
    fu_output_message sprintf('chmod -R%s %o %s',
                              (options[:force] ? 'f' : ''),
                              mode, list.join(' ')) if options[:verbose]
    return if options[:noop]
    list.each do |root|
      Entry_.new(root).traverse do |ent|
        begin
          ent.chmod(fu_mode(mode, ent.path))
        rescue
          raise unless options[:force]
        end
      end
    end
  end

  define_command('chmod_R', :noop, :verbose, :force)

  #
  # Options: noop verbose
  #
  # Changes owner and group on the named files (in +list+)
  # to the user +user+ and the group +group+.  +user+ and +group+
  # may be an ID (Integer/String) or a name (String).
  # If +user+ or +group+ is nil, this method does not change
  # the attribute.
  #
  #   FileUtils.chown 'root', 'staff', '/usr/local/bin/ruby'
  #   FileUtils.chown nil, 'bin', Dir.glob('/usr/bin/*'), :verbose => true
  #
  def chown(user, group, list, options = {})
    fu_check_options options, OPT_TABLE['chown']
    list = fu_list(list)
    fu_output_message sprintf('chown %s%s',
                              [user,group].compact.join(':') + ' ',
                              list.join(' ')) if options[:verbose]
    return if options[:noop]
    uid = fu_get_uid(user)
    gid = fu_get_gid(group)
    list.each do |path|
      Entry_.new(path).chown uid, gid
    end
  end

  define_command('chown', :noop, :verbose)

  #
  # Options: noop verbose force
  #
  # Changes owner and group on the named files (in +list+)
  # to the user +user+ and the group +group+ recursively.
  # +user+ and +group+ may be an ID (Integer/String) or
  # a name (String).  If +user+ or +group+ is nil, this
  # method does not change the attribute.
  #
  #   FileUtils.chown_R 'www', 'www', '/var/www/htdocs'
  #   FileUtils.chown_R 'cvs', 'cvs', '/var/cvs', :verbose => true
  #
  def chown_R(user, group, list, options = {})
    fu_check_options options, OPT_TABLE['chown_R']
    list = fu_list(list)
    fu_output_message sprintf('chown -R%s %s%s',
                              (options[:force] ? 'f' : ''),
                              [user,group].compact.join(':') + ' ',
                              list.join(' ')) if options[:verbose]
    return if options[:noop]
    uid = fu_get_uid(user)
    gid = fu_get_gid(group)
    return unless uid or gid
    list.each do |root|
      Entry_.new(root).traverse do |ent|
        begin
          ent.chown uid, gid
        rescue
          raise unless options[:force]
        end
      end
    end
  end

  define_command('chown_R', :noop, :verbose, :force)

private

  begin
    require 'etc'

    def fu_get_uid(user)   #:nodoc:
      return nil unless user
      case user
      when Integer
        user
      when /\A\d+\z/
        user.to_i
      else
        Etc.getpwnam(user).uid
      end
    end

    def fu_get_gid(group)   #:nodoc:
      return nil unless group
      case group
      when Integer
        group
      when /\A\d+\z/
        group.to_i
      else
        Etc.getgrnam(group).gid
      end
    end

  rescue LoadError
    # need Win32 support???

    def fu_get_uid(user)   #:nodoc:
      user    # FIXME
    end

    def fu_get_gid(group)   #:nodoc:
      group   # FIXME
    end
  end

public

  #
  # Options: noop verbose
  #
  # Updates modification time (mtime) and access time (atime) of file(s) in
  # +list+.  Files are created if they don't exist.
  #
  #   FileUtils.touch 'timestamp'
  #   FileUtils.touch Dir.glob('*.c');  system 'make'
  #
  def touch(list, options = {})
    fu_check_options options, OPT_TABLE['touch']
    list = fu_list(list)
    created = nocreate = options[:nocreate]
    t = options[:mtime]
    if options[:verbose]
      fu_output_message "touch #{nocreate ? '-c ' : ''}#{t ? t.strftime('-t %Y%m%d%H%M.%S ') : ''}#{list.join ' '}"
    end
    return if options[:noop]
    list.each do |path|
      created = nocreate
      begin
        File.utime(t, t, path)
      rescue Errno::ENOENT
        raise if created
        File.open(path, 'a') {
          ;
        }
        created = true
        retry if t
      end
    end
  end

  define_command('touch', :noop, :verbose, :mtime, :nocreate)

private

  module StreamUtils_
    private

    def fu_windows?
      /mswin|mingw|bccwin|emx/ =~ RUBY_PLATFORM
    end

    def fu_copy_stream0(src, dest, blksize = nil)   #:nodoc:
      IO.copy_stream(src, dest)
    end

    def fu_stream_blksize(*streams)
      streams.each do |s|
        next unless s.respond_to?(:stat)
        size = fu_blksize(s.stat)
        return size if size
      end
      fu_default_blksize()
    end

    def fu_blksize(st)
      s = st.blksize
      return nil unless s
      return nil if s == 0
      s
    end

    def fu_default_blksize
      1024
    end
  end

  include StreamUtils_
  extend StreamUtils_

  class Entry_   #:nodoc: internal use only
    include StreamUtils_

    def initialize(a, b = nil, deref = false)
      @prefix = @rel = @path = nil
      if b
        @prefix = a
        @rel = b
      else
        @path = a
      end
      @deref = deref
      @stat = nil
      @lstat = nil
    end

    def inspect
      "\#<#{self.class} #{path()}>"
    end

    def path
      if @path
        File.path(@path)
      else
        join(@prefix, @rel)
      end
    end

    def prefix
      @prefix || @path
    end

    def rel
      @rel
    end

    def dereference?
      @deref
    end

    def exist?
      lstat! ? true : false
    end

    def file?
      s = lstat!
      s and s.file?
    end

    def directory?
      s = lstat!
      s and s.directory?
    end

    def symlink?
      s = lstat!
      s and s.symlink?
    end

    def chardev?
      s = lstat!
      s and s.chardev?
    end

    def blockdev?
      s = lstat!
      s and s.blockdev?
    end

    def socket?
      s = lstat!
      s and s.socket?
    end

    def pipe?
      s = lstat!
      s and s.pipe?
    end

    S_IF_DOOR = 0xD000

    def door?
      s = lstat!
      s and (s.mode & 0xF000 == S_IF_DOOR)
    end

    def entries
      opts = {}
      opts[:encoding] = ::Encoding::UTF_8 if fu_windows?
      Dir.entries(path(), opts)\
          .reject {|n| n == '.' or n == '..' }\
          .map {|n| Entry_.new(prefix(), join(rel(), n.untaint)) }
    end

    def stat
      return @stat if @stat
      if lstat() and lstat().symlink?
        @stat = File.stat(path())
      else
        @stat = lstat()
      end
      @stat
    end

    def stat!
      return @stat if @stat
      if lstat! and lstat!.symlink?
        @stat = File.stat(path())
      else
        @stat = lstat!
      end
      @stat
    rescue SystemCallError
      nil
    end

    def lstat
      if dereference?
        @lstat ||= File.stat(path())
      else
        @lstat ||= File.lstat(path())
      end
    end

    def lstat!
      lstat()
    rescue SystemCallError
      nil
    end

    def chmod(mode)
      if symlink?
        File.lchmod mode, path() if have_lchmod?
      else
        File.chmod mode, path()
      end
    end

    def chown(uid, gid)
      if symlink?
        File.lchown uid, gid, path() if have_lchown?
      else
        File.chown uid, gid, path()
      end
    end

    def copy(dest)
      case
      when file?
        copy_file dest
      when directory?
        if !File.exist?(dest) and descendant_diretory?(dest, path)
          raise ArgumentError, "cannot copy directory %s to itself %s" % [path, dest]
        end
        begin
          Dir.mkdir dest
        rescue
          raise unless File.directory?(dest)
        end
      when symlink?
        File.symlink File.readlink(path()), dest
      when chardev?
        raise "cannot handle device file" unless File.respond_to?(:mknod)
        mknod dest, ?c, 0666, lstat().rdev
      when blockdev?
        raise "cannot handle device file" unless File.respond_to?(:mknod)
        mknod dest, ?b, 0666, lstat().rdev
      when socket?
        raise "cannot handle socket" unless File.respond_to?(:mknod)
        mknod dest, nil, lstat().mode, 0
      when pipe?
        raise "cannot handle FIFO" unless File.respond_to?(:mkfifo)
        mkfifo dest, 0666
      when door?
        raise "cannot handle door: #{path()}"
      else
        raise "unknown file type: #{path()}"
      end
    end

    def copy_file(dest)
      File.open(path()) do |s|
        File.open(dest, 'wb', s.stat.mode) do |f|
          IO.copy_stream(s, f)
        end
      end
    end

    def copy_metadata(path)
      st = lstat()
      File.utime st.atime, st.mtime, path
      begin
        File.chown st.uid, st.gid, path
      rescue Errno::EPERM
        # clear setuid/setgid
        File.chmod st.mode & 01777, path
      else
        File.chmod st.mode, path
      end
    end

    def remove
      if directory?
        remove_dir1
      else
        remove_file
      end
    end

    def remove_dir1
      platform_support {
        Dir.rmdir path().chomp(?/)
      }
    end

    def remove_file
      platform_support {
        File.unlink path
      }
    end

    def platform_support
      return yield unless fu_windows?
      first_time_p = true
      begin
        yield
      rescue Errno::ENOENT
        raise
      rescue => err
        if first_time_p
          first_time_p = false
          begin
            File.chmod 0700, path()   # Windows does not have symlink
            retry
          rescue SystemCallError
          end
        end
        raise err
      end
    end

    def preorder_traverse
      stack = [self]
      while ent = stack.pop
        yield ent
        stack.concat ent.entries.reverse if ent.directory?
      end
    end

    alias traverse preorder_traverse

    def postorder_traverse
      if directory?
        entries().each do |ent|
          ent.postorder_traverse do |e|
            yield e
          end
        end
      end
      yield self
    end

  private

    $fileutils_rb_have_lchmod = nil

    def have_lchmod?
      # This is not MT-safe, but it does not matter.
      if $fileutils_rb_have_lchmod == nil
        $fileutils_rb_have_lchmod = check_have_lchmod?
      end
      $fileutils_rb_have_lchmod
    end

    def check_have_lchmod?
      return false unless File.respond_to?(:lchmod)
      File.lchmod 0
      return true
    rescue NotImplementedError
      return false
    end

    $fileutils_rb_have_lchown = nil

    def have_lchown?
      # This is not MT-safe, but it does not matter.
      if $fileutils_rb_have_lchown == nil
        $fileutils_rb_have_lchown = check_have_lchown?
      end
      $fileutils_rb_have_lchown
    end

    def check_have_lchown?
      return false unless File.respond_to?(:lchown)
      File.lchown nil, nil
      return true
    rescue NotImplementedError
      return false
    end

    def join(dir, base)
      return File.path(dir) if not base or base == '.'
      return File.path(base) if not dir or dir == '.'
      File.join(dir, base)
    end

    if File::ALT_SEPARATOR
      DIRECTORY_TERM = "(?=[/#{Regexp.quote(File::ALT_SEPARATOR)}]|\\z)".freeze
    else
      DIRECTORY_TERM = "(?=/|\\z)".freeze
    end
    SYSCASE = File::FNM_SYSCASE.nonzero? ? "-i" : ""

    def descendant_diretory?(descendant, ascendant)
      /\A(?#{SYSCASE}:#{Regexp.quote(ascendant)})#{DIRECTORY_TERM}/ =~ File.dirname(descendant)
    end
  end   # class Entry_

private

  def fu_list(arg)   #:nodoc:
    [arg].flatten.map {|path| File.path(path) }
  end

  def fu_each_src_dest(src, dest)   #:nodoc:
    fu_each_src_dest0(src, dest) do |s, d|
      raise ArgumentError, "same file: #{s} and #{d}" if fu_same?(s, d)
      yield s, d, File.stat(s)
    end
  end

  def fu_each_src_dest0(src, dest)   #:nodoc:
    if tmp = Array.try_convert(src)
      tmp.each do |s|
        s = File.path(s)
        yield s, File.join(dest, File.basename(s))
      end
    else
      src = File.path(src)
      if File.directory?(dest)
        yield src, File.join(dest, File.basename(src))
      else
        yield src, File.path(dest)
      end
    end
  end

  def fu_same?(a, b)   #:nodoc:
    File.identical?(a, b)
  end

  def fu_check_options(options, optdecl)   #:nodoc:
    h = options.dup
    optdecl.each do |opt|
      h.delete opt
    end
    raise ArgumentError, "no such option: #{h.keys.join(' ')}" unless h.empty?
  end

  def fu_update_option(args, new)   #:nodoc:
    if tmp = Hash.try_convert(args.last)
      args[-1] = tmp.dup.update(new)
    else
      args.push new
    end
    args
  end

  def fu_output_message(msg)   #:nodoc:
    @fileutils_output ||= $stderr
    @fileutils_label  ||= ''
    @fileutils_output.puts @fileutils_label + msg
  end

  #
  # Returns an Array of method names which have any options.
  #
  #   p FileUtils.commands  #=> ["chmod", "cp", "cp_r", "install", ...]
  #
  def FileUtils.commands
    OPT_TABLE.keys
  end

  #
  # Returns an Array of option names.
  #
  #   p FileUtils.options  #=> ["noop", "force", "verbose", "preserve", "mode"]
  #
  def FileUtils.options
    OPT_TABLE.values.flatten.uniq.map {|sym| sym.to_s }
  end

  #
  # Returns true if the method +mid+ have an option +opt+.
  #
  #   p FileUtils.have_option?(:cp, :noop)     #=> true
  #   p FileUtils.have_option?(:rm, :force)    #=> true
  #   p FileUtils.have_option?(:rm, :perserve) #=> false
  #
  def FileUtils.have_option?(mid, opt)
    li = OPT_TABLE[mid.to_s] or raise ArgumentError, "no such method: #{mid}"
    li.include?(opt)
  end

  #
  # Returns an Array of option names of the method +mid+.
  #
  #   p FileUtils.options(:rm)  #=> ["noop", "verbose", "force"]
  #
  def FileUtils.options_of(mid)
    OPT_TABLE[mid.to_s].map {|sym| sym.to_s }
  end

  #
  # Returns an Array of method names which have the option +opt+.
  #
  #   p FileUtils.collect_method(:preserve) #=> ["cp", "cp_r", "copy", "install"]
  #
  def FileUtils.collect_method(opt)
    OPT_TABLE.keys.select {|m| OPT_TABLE[m].include?(opt) }
  end

  # LOW_METHODS
  #
  #   :pwd, :getwd, :cd, :chdir,
  #   :uptodate?, :copy_entry, :copy_file, :copy_stream, :remove_entry_secure,
  #   :remove_entry, :remove_file, :remove_dir, :compare_file, :identical?,
  #   :cmp, :compare_stream
  #
  # DEPRECATED - Only here for backward compatibility.
  LOW_METHODS = (commands - collect_method(:noop)).map(&:to_sym)


  # METHODS
  #
  #   :pwd, :getwd, :cd, :chdir, :uptodate?, :mkdir, :mkdir_p, :mkpath, :makedirs,
  #   :rmdir, :ln, :link, :ln_s, :symlink, :ln_sf, :cp, :copy, :cp_r, :copy_entry,
  #   :copy_file, :copy_stream, :mv, :move, :rm, :remove, :rm_f, :safe_unlink,
  #   :rm_r, :rm_rf, :rmtree, :remove_entry_secure, :remove_entry, :remove_file,
  #   :remove_dir, :compare_file, :identical?, :cmp, :compare_stream, :install,
  #   :chmod, :chmod_R, :chown, :chown_R, :touch
  #
  # DEPRECATED - Only here for backward compatibility.
  METHODS = commands.map(&:to_sym)

end
