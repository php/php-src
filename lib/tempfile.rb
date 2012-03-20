#
# tempfile - manipulates temporary files
#
# $Id$
#

require 'delegate'
require 'tmpdir'
require 'thread'

# A utility class for managing temporary files. When you create a Tempfile
# object, it will create a temporary file with a unique filename. A Tempfile
# objects behaves just like a File object, and you can perform all the usual
# file operations on it: reading data, writing data, changing its permissions,
# etc. So although this class does not explicitly document all instance methods
# supported by File, you can in fact call any File instance method on a
# Tempfile object.
#
# == Synopsis
#
#   require 'tempfile'
#
#   file = Tempfile.new('foo')
#   file.path      # => A unique filename in the OS's temp directory,
#                  #    e.g.: "/tmp/foo.24722.0"
#                  #    This filename contains 'foo' in its basename.
#   file.write("hello world")
#   file.rewind
#   file.read      # => "hello world"
#   file.close
#   file.unlink    # deletes the temp file
#
# == Good practices
#
# === Explicit close
#
# When a Tempfile object is garbage collected, or when the Ruby interpreter
# exits, its associated temporary file is automatically deleted. This means
# that's it's unnecessary to explicitly delete a Tempfile after use, though
# it's good practice to do so: not explicitly deleting unused Tempfiles can
# potentially leave behind large amounts of tempfiles on the filesystem
# until they're garbage collected. The existence of these temp files can make
# it harder to determine a new Tempfile filename.
#
# Therefore, one should always call #unlink or close in an ensure block, like
# this:
#
#   file = Tempfile.new('foo')
#   begin
#      ...do something with file...
#   ensure
#      file.close
#      file.unlink   # deletes the temp file
#   end
#
# === Unlink after creation
#
# On POSIX systems, it's possible to unlink a file right after creating it,
# and before closing it. This removes the filesystem entry without closing
# the file handle, so it ensures that only the processes that already had
# the file handle open can access the file's contents. It's strongly
# recommended that you do this if you do not want any other processes to
# be able to read from or write to the Tempfile, and you do not need to
# know the Tempfile's filename either.
#
# For example, a practical use case for unlink-after-creation would be this:
# you need a large byte buffer that's too large to comfortably fit in RAM,
# e.g. when you're writing a web server and you want to buffer the client's
# file upload data.
#
# Please refer to #unlink for more information and a code example.
#
# == Minor notes
#
# Tempfile's filename picking method is both thread-safe and inter-process-safe:
# it guarantees that no other threads or processes will pick the same filename.
#
# Tempfile itself however may not be entirely thread-safe. If you access the
# same Tempfile object from multiple threads then you should protect it with a
# mutex.
class Tempfile < DelegateClass(File)
  include Dir::Tmpname

  # call-seq:
  #    new(basename, [tmpdir = Dir.tmpdir], [options])
  #
  # Creates a temporary file with permissions 0600 (= only readable and
  # writable by the owner) and opens it with mode "w+".
  #
  # The +basename+ parameter is used to determine the name of the
  # temporary file. You can either pass a String or an Array with
  # 2 String elements. In the former form, the temporary file's base
  # name will begin with the given string. In the latter form,
  # the temporary file's base name will begin with the array's first
  # element, and end with the second element. For example:
  #
  #   file = Tempfile.new('hello')
  #   file.path  # => something like: "/tmp/hello2843-8392-92849382--0"
  #
  #   # Use the Array form to enforce an extension in the filename:
  #   file = Tempfile.new(['hello', '.jpg'])
  #   file.path  # => something like: "/tmp/hello2843-8392-92849382--0.jpg"
  #
  # The temporary file will be placed in the directory as specified
  # by the +tmpdir+ parameter. By default, this is +Dir.tmpdir+.
  # When $SAFE > 0 and the given +tmpdir+ is tainted, it uses
  # '/tmp' as the temporary directory. Please note that ENV values
  # are tainted by default, and +Dir.tmpdir+'s return value might
  # come from environment variables (e.g. <tt>$TMPDIR</tt>).
  #
  #   file = Tempfile.new('hello', '/home/aisaka')
  #   file.path  # => something like: "/home/aisaka/hello2843-8392-92849382--0"
  #
  # You can also pass an options hash. Under the hood, Tempfile creates
  # the temporary file using +File.open+. These options will be passed to
  # +File.open+. This is mostly useful for specifying encoding
  # options, e.g.:
  #
  #   Tempfile.new('hello', '/home/aisaka', :encoding => 'ascii-8bit')
  #
  #   # You can also omit the 'tmpdir' parameter:
  #   Tempfile.new('hello', :encoding => 'ascii-8bit')
  #
  # === Exceptions
  #
  # If Tempfile.new cannot find a unique filename within a limited
  # number of tries, then it will raise an exception.
  def initialize(basename, *rest)
    if block_given?
      warn "Tempfile.new doesn't call the given block."
    end
    @data = []
    @clean_proc = Remover.new(@data)
    ObjectSpace.define_finalizer(self, @clean_proc)

    create(basename, *rest) do |tmpname, n, opts|
      mode = File::RDWR|File::CREAT|File::EXCL
      perm = 0600
      if opts
        mode |= opts.delete(:mode) || 0
        opts[:perm] = perm
        perm = nil
      else
        opts = perm
      end
      @data[1] = @tmpfile = File.open(tmpname, mode, opts)
      @data[0] = @tmpname = tmpname
      @mode = mode & ~(File::CREAT|File::EXCL)
      perm or opts.freeze
      @opts = opts
    end

    super(@tmpfile)
  end

  # Opens or reopens the file with mode "r+".
  def open
    @tmpfile.close if @tmpfile
    @tmpfile = File.open(@tmpname, @mode, @opts)
    @data[1] = @tmpfile
    __setobj__(@tmpfile)
  end

  def _close    # :nodoc:
    begin
      @tmpfile.close if @tmpfile
    ensure
      @tmpfile = nil
      @data[1] = nil if @data
    end
  end
  protected :_close

  # Closes the file. If +unlink_now+ is true, then the file will be unlinked
  # (deleted) after closing. Of course, you can choose to later call #unlink
  # if you do not unlink it now.
  #
  # If you don't explicitly unlink the temporary file, the removal
  # will be delayed until the object is finalized.
  def close(unlink_now=false)
    if unlink_now
      close!
    else
      _close
    end
  end

  # Closes and unlinks (deletes) the file. Has the same effect as called
  # <tt>close(true)</tt>.
  def close!
    _close
    unlink
    ObjectSpace.undefine_finalizer(self)
  end

  # Unlinks (deletes) the file from the filesystem. One should always unlink
  # the file after using it, as is explained in the "Explicit close" good
  # practice section in the Tempfile overview:
  #
  #   file = Tempfile.new('foo')
  #   begin
  #      ...do something with file...
  #   ensure
  #      file.close
  #      file.unlink   # deletes the temp file
  #   end
  #
  # === Unlink-before-close
  #
  # On POSIX systems it's possible to unlink a file before closing it. This
  # practice is explained in detail in the Tempfile overview (section
  # "Unlink after creation"); please refer there for more information.
  #
  # However, unlink-before-close may not be supported on non-POSIX operating
  # systems. Microsoft Windows is the most notable case: unlinking a non-closed
  # file will result in an error, which this method will silently ignore. If
  # you want to practice unlink-before-close whenever possible, then you should
  # write code like this:
  #
  #   file = Tempfile.new('foo')
  #   file.unlink   # On Windows this silently fails.
  #   begin
  #      ... do something with file ...
  #   ensure
  #      file.close!   # Closes the file handle. If the file wasn't unlinked
  #                    # because #unlink failed, then this method will attempt
  #                    # to do so again.
  #   end
  def unlink
    return unless @tmpname
    begin
      File.unlink(@tmpname)
    rescue Errno::ENOENT
    rescue Errno::EACCES
      # may not be able to unlink on Windows; just ignore
      return
    end
    # remove tmpname from remover
    @data[0] = @data[1] = nil
    @tmpname = nil
  end
  alias delete unlink

  # Returns the full path name of the temporary file.
  # This will be nil if #unlink has been called.
  def path
    @tmpname
  end

  # Returns the size of the temporary file.  As a side effect, the IO
  # buffer is flushed before determining the size.
  def size
    if @tmpfile
      @tmpfile.flush
      @tmpfile.stat.size
    elsif @tmpname
      File.size(@tmpname)
    else
      0
    end
  end
  alias length size

  # :stopdoc:
  class Remover
    def initialize(data)
      @pid = $$
      @data = data
    end

    def call(*args)
      return if @pid != $$

      path, tmpfile = *@data

      STDERR.print "removing ", path, "..." if $DEBUG

      tmpfile.close if tmpfile

      if path
        begin
          File.unlink(path)
        rescue Errno::ENOENT
        end
      end

      STDERR.print "done\n" if $DEBUG
    end
  end
  # :startdoc:

  class << self
    # Creates a new Tempfile.
    #
    # If no block is given, this is a synonym for Tempfile.new.
    #
    # If a block is given, then a Tempfile object will be constructed,
    # and the block is run with said object as argument. The Tempfile
    # object will be automatically closed after the block terminates.
    # The call returns the value of the block.
    #
    # In any case, all arguments (+*args+) will be passed to Tempfile.new.
    #
    #   Tempfile.open('foo', '/home/temp') do |f|
    #      ... do something with f ...
    #   end
    #
    #   # Equivalent:
    #   f = Tempfile.open('foo', '/home/temp')
    #   begin
    #      ... do something with f ...
    #   ensure
    #      f.close
    #   end
    def open(*args)
      tempfile = new(*args)

      if block_given?
        begin
          yield(tempfile)
        ensure
          tempfile.close
        end
      else
        tempfile
      end
    end
  end
end

if __FILE__ == $0
#  $DEBUG = true
  f = Tempfile.new("foo")
  f.print("foo\n")
  f.close
  f.open
  p f.gets # => "foo\n"
  f.close!
end
