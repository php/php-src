#
# = pathname.rb
#
# Object-Oriented Pathname Class
#
# Author:: Tanaka Akira <akr@m17n.org>
# Documentation:: Author and Gavin Sinclair
#
# For documentation, see class Pathname.
#
# <tt>pathname.rb</tt> is distributed with Ruby since 1.8.0.
#

require 'pathname.so'

class Pathname

  # :stopdoc:
  if RUBY_VERSION < "1.9"
    TO_PATH = :to_str
  else
    # to_path is implemented so Pathname objects are usable with File.open, etc.
    TO_PATH = :to_path
  end

  SAME_PATHS = if File::FNM_SYSCASE.nonzero?
    proc {|a, b| a.casecmp(b).zero?}
  else
    proc {|a, b| a == b}
  end

  # :startdoc:

  if File::ALT_SEPARATOR
    SEPARATOR_LIST = "#{Regexp.quote File::ALT_SEPARATOR}#{Regexp.quote File::SEPARATOR}"
    SEPARATOR_PAT = /[#{SEPARATOR_LIST}]/
  else
    SEPARATOR_LIST = "#{Regexp.quote File::SEPARATOR}"
    SEPARATOR_PAT = /#{Regexp.quote File::SEPARATOR}/
  end

  # chop_basename(path) -> [pre-basename, basename] or nil
  def chop_basename(path)
    base = File.basename(path)
    if /\A#{SEPARATOR_PAT}?\z/o =~ base
      return nil
    else
      return path[0, path.rindex(base)], base
    end
  end
  private :chop_basename

  # split_names(path) -> prefix, [name, ...]
  def split_names(path)
    names = []
    while r = chop_basename(path)
      path, basename = r
      names.unshift basename
    end
    return path, names
  end
  private :split_names

  def prepend_prefix(prefix, relpath)
    if relpath.empty?
      File.dirname(prefix)
    elsif /#{SEPARATOR_PAT}/o =~ prefix
      prefix = File.dirname(prefix)
      prefix = File.join(prefix, "") if File.basename(prefix + 'a') != 'a'
      prefix + relpath
    else
      prefix + relpath
    end
  end
  private :prepend_prefix

  # Returns clean pathname of +self+ with consecutive slashes and useless dots
  # removed.  The filesystem is not accessed.
  #
  # If +consider_symlink+ is +true+, then a more conservative algorithm is used
  # to avoid breaking symbolic linkages.  This may retain more <tt>..</tt>
  # entries than absolutely necessary, but without accessing the filesystem,
  # this can't be avoided.  See #realpath.
  #
  def cleanpath(consider_symlink=false)
    if consider_symlink
      cleanpath_conservative
    else
      cleanpath_aggressive
    end
  end

  #
  # Clean the path simply by resolving and removing excess "." and ".." entries.
  # Nothing more, nothing less.
  #
  def cleanpath_aggressive
    path = @path
    names = []
    pre = path
    while r = chop_basename(pre)
      pre, base = r
      case base
      when '.'
      when '..'
        names.unshift base
      else
        if names[0] == '..'
          names.shift
        else
          names.unshift base
        end
      end
    end
    if /#{SEPARATOR_PAT}/o =~ File.basename(pre)
      names.shift while names[0] == '..'
    end
    self.class.new(prepend_prefix(pre, File.join(*names)))
  end
  private :cleanpath_aggressive

  # has_trailing_separator?(path) -> bool
  def has_trailing_separator?(path)
    if r = chop_basename(path)
      pre, basename = r
      pre.length + basename.length < path.length
    else
      false
    end
  end
  private :has_trailing_separator?

  # add_trailing_separator(path) -> path
  def add_trailing_separator(path)
    if File.basename(path + 'a') == 'a'
      path
    else
      File.join(path, "") # xxx: Is File.join is appropriate to add separator?
    end
  end
  private :add_trailing_separator

  def del_trailing_separator(path)
    if r = chop_basename(path)
      pre, basename = r
      pre + basename
    elsif /#{SEPARATOR_PAT}+\z/o =~ path
      $` + File.dirname(path)[/#{SEPARATOR_PAT}*\z/o]
    else
      path
    end
  end
  private :del_trailing_separator

  def cleanpath_conservative
    path = @path
    names = []
    pre = path
    while r = chop_basename(pre)
      pre, base = r
      names.unshift base if base != '.'
    end
    if /#{SEPARATOR_PAT}/o =~ File.basename(pre)
      names.shift while names[0] == '..'
    end
    if names.empty?
      self.class.new(File.dirname(pre))
    else
      if names.last != '..' && File.basename(path) == '.'
        names << '.'
      end
      result = prepend_prefix(pre, File.join(*names))
      if /\A(?:\.|\.\.)\z/ !~ names.last && has_trailing_separator?(path)
        self.class.new(add_trailing_separator(result))
      else
        self.class.new(result)
      end
    end
  end
  private :cleanpath_conservative

  # #parent returns the parent directory.
  #
  # This is same as <tt>self + '..'</tt>.
  def parent
    self + '..'
  end

  # #mountpoint? returns +true+ if <tt>self</tt> points to a mountpoint.
  def mountpoint?
    begin
      stat1 = self.lstat
      stat2 = self.parent.lstat
      stat1.dev == stat2.dev && stat1.ino == stat2.ino ||
        stat1.dev != stat2.dev
    rescue Errno::ENOENT
      false
    end
  end

  #
  # #root? is a predicate for root directories.  I.e. it returns +true+ if the
  # pathname consists of consecutive slashes.
  #
  # It doesn't access actual filesystem.  So it may return +false+ for some
  # pathnames which points to roots such as <tt>/usr/..</tt>.
  #
  def root?
    !!(chop_basename(@path) == nil && /#{SEPARATOR_PAT}/o =~ @path)
  end

  # Predicate method for testing whether a path is absolute.
  # It returns +true+ if the pathname begins with a slash.
  def absolute?
    !relative?
  end

  # The opposite of #absolute?
  def relative?
    path = @path
    while r = chop_basename(path)
      path, = r
    end
    path == ''
  end

  #
  # Iterates over each component of the path.
  #
  #   Pathname.new("/usr/bin/ruby").each_filename {|filename| ... }
  #     # yields "usr", "bin", and "ruby".
  #
  def each_filename # :yield: filename
    return to_enum(__method__) unless block_given?
    _, names = split_names(@path)
    names.each {|filename| yield filename }
    nil
  end

  # Iterates over and yields a new Pathname object
  # for each element in the given path in descending order.
  #
  #  Pathname.new('/path/to/some/file.rb').descend {|v| p v}
  #     #<Pathname:/>
  #     #<Pathname:/path>
  #     #<Pathname:/path/to>
  #     #<Pathname:/path/to/some>
  #     #<Pathname:/path/to/some/file.rb>
  #
  #  Pathname.new('path/to/some/file.rb').descend {|v| p v}
  #     #<Pathname:path>
  #     #<Pathname:path/to>
  #     #<Pathname:path/to/some>
  #     #<Pathname:path/to/some/file.rb>
  #
  # It doesn't access actual filesystem.
  #
  # This method is available since 1.8.5.
  #
  def descend
    vs = []
    ascend {|v| vs << v }
    vs.reverse_each {|v| yield v }
    nil
  end

  # Iterates over and yields a new Pathname object
  # for each element in the given path in ascending order.
  #
  #  Pathname.new('/path/to/some/file.rb').ascend {|v| p v}
  #     #<Pathname:/path/to/some/file.rb>
  #     #<Pathname:/path/to/some>
  #     #<Pathname:/path/to>
  #     #<Pathname:/path>
  #     #<Pathname:/>
  #
  #  Pathname.new('path/to/some/file.rb').ascend {|v| p v}
  #     #<Pathname:path/to/some/file.rb>
  #     #<Pathname:path/to/some>
  #     #<Pathname:path/to>
  #     #<Pathname:path>
  #
  # It doesn't access actual filesystem.
  #
  # This method is available since 1.8.5.
  #
  def ascend
    path = @path
    yield self
    while r = chop_basename(path)
      path, = r
      break if path.empty?
      yield self.class.new(del_trailing_separator(path))
    end
  end

  #
  # Pathname#+ appends a pathname fragment to this one to produce a new Pathname
  # object.
  #
  #   p1 = Pathname.new("/usr")      # Pathname:/usr
  #   p2 = p1 + "bin/ruby"           # Pathname:/usr/bin/ruby
  #   p3 = p1 + "/etc/passwd"        # Pathname:/etc/passwd
  #
  # This method doesn't access the file system; it is pure string manipulation.
  #
  def +(other)
    other = Pathname.new(other) unless Pathname === other
    Pathname.new(plus(@path, other.to_s))
  end

  def plus(path1, path2) # -> path
    prefix2 = path2
    index_list2 = []
    basename_list2 = []
    while r2 = chop_basename(prefix2)
      prefix2, basename2 = r2
      index_list2.unshift prefix2.length
      basename_list2.unshift basename2
    end
    return path2 if prefix2 != ''
    prefix1 = path1
    while true
      while !basename_list2.empty? && basename_list2.first == '.'
        index_list2.shift
        basename_list2.shift
      end
      break unless r1 = chop_basename(prefix1)
      prefix1, basename1 = r1
      next if basename1 == '.'
      if basename1 == '..' || basename_list2.empty? || basename_list2.first != '..'
        prefix1 = prefix1 + basename1
        break
      end
      index_list2.shift
      basename_list2.shift
    end
    r1 = chop_basename(prefix1)
    if !r1 && /#{SEPARATOR_PAT}/o =~ File.basename(prefix1)
      while !basename_list2.empty? && basename_list2.first == '..'
        index_list2.shift
        basename_list2.shift
      end
    end
    if !basename_list2.empty?
      suffix2 = path2[index_list2.first..-1]
      r1 ? File.join(prefix1, suffix2) : prefix1 + suffix2
    else
      r1 ? prefix1 : File.dirname(prefix1)
    end
  end
  private :plus

  #
  # Pathname#join joins pathnames.
  #
  # <tt>path0.join(path1, ..., pathN)</tt> is the same as
  # <tt>path0 + path1 + ... + pathN</tt>.
  #
  def join(*args)
    args.unshift self
    result = args.pop
    result = Pathname.new(result) unless Pathname === result
    return result if result.absolute?
    args.reverse_each {|arg|
      arg = Pathname.new(arg) unless Pathname === arg
      result = arg + result
      return result if result.absolute?
    }
    result
  end

  #
  # Returns the children of the directory (files and subdirectories, not
  # recursive) as an array of Pathname objects.  By default, the returned
  # pathnames will have enough information to access the files.  If you set
  # +with_directory+ to +false+, then the returned pathnames will contain the
  # filename only.
  #
  # For example:
  #   pn = Pathname("/usr/lib/ruby/1.8")
  #   pn.children
  #       # -> [ Pathname:/usr/lib/ruby/1.8/English.rb,
  #              Pathname:/usr/lib/ruby/1.8/Env.rb,
  #              Pathname:/usr/lib/ruby/1.8/abbrev.rb, ... ]
  #   pn.children(false)
  #       # -> [ Pathname:English.rb, Pathname:Env.rb, Pathname:abbrev.rb, ... ]
  #
  # Note that the results never contain the entries <tt>.</tt> and <tt>..</tt> in
  # the directory because they are not children.
  #
  # This method has existed since 1.8.1.
  #
  def children(with_directory=true)
    with_directory = false if @path == '.'
    result = []
    Dir.foreach(@path) {|e|
      next if e == '.' || e == '..'
      if with_directory
        result << self.class.new(File.join(@path, e))
      else
        result << self.class.new(e)
      end
    }
    result
  end

  # Iterates over the children of the directory
  # (files and subdirectories, not recursive).
  # It yields Pathname object for each child.
  # By default, the yielded pathnames will have enough information to access the files.
  # If you set +with_directory+ to +false+, then the returned pathnames will contain the filename only.
  #
  #   Pathname("/usr/local").each_child {|f| p f }
  #   #=> #<Pathname:/usr/local/share>
  #   #   #<Pathname:/usr/local/bin>
  #   #   #<Pathname:/usr/local/games>
  #   #   #<Pathname:/usr/local/lib>
  #   #   #<Pathname:/usr/local/include>
  #   #   #<Pathname:/usr/local/sbin>
  #   #   #<Pathname:/usr/local/src>
  #   #   #<Pathname:/usr/local/man>
  #
  #   Pathname("/usr/local").each_child(false) {|f| p f }
  #   #=> #<Pathname:share>
  #   #   #<Pathname:bin>
  #   #   #<Pathname:games>
  #   #   #<Pathname:lib>
  #   #   #<Pathname:include>
  #   #   #<Pathname:sbin>
  #   #   #<Pathname:src>
  #   #   #<Pathname:man>
  #
  def each_child(with_directory=true, &b)
    children(with_directory).each(&b)
  end

  #
  # #relative_path_from returns a relative path from the argument to the
  # receiver.  If +self+ is absolute, the argument must be absolute too.  If
  # +self+ is relative, the argument must be relative too.
  #
  # #relative_path_from doesn't access the filesystem.  It assumes no symlinks.
  #
  # ArgumentError is raised when it cannot find a relative path.
  #
  # This method has existed since 1.8.1.
  #
  def relative_path_from(base_directory)
    dest_directory = self.cleanpath.to_s
    base_directory = base_directory.cleanpath.to_s
    dest_prefix = dest_directory
    dest_names = []
    while r = chop_basename(dest_prefix)
      dest_prefix, basename = r
      dest_names.unshift basename if basename != '.'
    end
    base_prefix = base_directory
    base_names = []
    while r = chop_basename(base_prefix)
      base_prefix, basename = r
      base_names.unshift basename if basename != '.'
    end
    unless SAME_PATHS[dest_prefix, base_prefix]
      raise ArgumentError, "different prefix: #{dest_prefix.inspect} and #{base_directory.inspect}"
    end
    while !dest_names.empty? &&
          !base_names.empty? &&
          SAME_PATHS[dest_names.first, base_names.first]
      dest_names.shift
      base_names.shift
    end
    if base_names.include? '..'
      raise ArgumentError, "base_directory has ..: #{base_directory.inspect}"
    end
    base_names.fill('..')
    relpath_names = base_names + dest_names
    if relpath_names.empty?
      Pathname.new('.')
    else
      Pathname.new(File.join(*relpath_names))
    end
  end
end


class Pathname    # * Find *
  #
  # Pathname#find is an iterator to traverse a directory tree in a depth first
  # manner.  It yields a Pathname for each file under "this" directory.
  #
  # Returns an enumerator if no block is given.
  #
  # Since it is implemented by <tt>find.rb</tt>, <tt>Find.prune</tt> can be used
  # to control the traversal.
  #
  # If +self+ is <tt>.</tt>, yielded pathnames begin with a filename in the
  # current directory, not <tt>./</tt>.
  #
  def find # :yield: pathname
    return to_enum(__method__) unless block_given?
    require 'find'
    if @path == '.'
      Find.find(@path) {|f| yield self.class.new(f.sub(%r{\A\./}, '')) }
    else
      Find.find(@path) {|f| yield self.class.new(f) }
    end
  end
end


class Pathname    # * FileUtils *
  # See <tt>FileUtils.mkpath</tt>.  Creates a full path, including any
  # intermediate directories that don't yet exist.
  def mkpath
    require 'fileutils'
    FileUtils.mkpath(@path)
    nil
  end

  # See <tt>FileUtils.rm_r</tt>.  Deletes a directory and all beneath it.
  def rmtree
    # The name "rmtree" is borrowed from File::Path of Perl.
    # File::Path provides "mkpath" and "rmtree".
    require 'fileutils'
    FileUtils.rm_r(@path)
    nil
  end
end

