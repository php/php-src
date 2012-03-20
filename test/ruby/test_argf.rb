require 'test/unit'
require 'timeout'
require 'tmpdir'
require 'tempfile'
require_relative 'envutil'

class TestArgf < Test::Unit::TestCase
  def setup
    @t1 = Tempfile.new("argf-foo")
    @t1.binmode
    @t1.puts "1"
    @t1.puts "2"
    @t1.close
    @t2 = Tempfile.new("argf-bar")
    @t2.binmode
    @t2.puts "3"
    @t2.puts "4"
    @t2.close
    @t3 = Tempfile.new("argf-baz")
    @t3.binmode
    @t3.puts "5"
    @t3.puts "6"
    @t3.close
    @tmps = [@t1, @t2, @t3]
  end

  def teardown
    @tmps.each {|t|
      bak = t.path + ".bak"
      File.unlink bak if File.file? bak
      t.close(true)
    }
  end

  def make_tempfile
    t = Tempfile.new("argf-qux")
    t.puts "foo"
    t.puts "bar"
    t.puts "baz"
    t.close
    @tmps << t
    t
  end

  def ruby(*args)
    args = ['-e', '$>.write($<.read)'] if args.empty?
    ruby = EnvUtil.rubybin
    f = IO.popen([ruby] + args, 'r+')
    yield(f)
  ensure
    f.close unless !f || f.closed?
  end

  def no_safe_rename
    /cygwin|mswin|mingw|bccwin/ =~ RUBY_PLATFORM
  end

  def assert_src_expected(line, src, args = nil)
    args ||= [@t1.path, @t2.path, @t3.path]
    expected = src.split(/^/)
    ruby('-e', src, *args) do |f|
      expected.each_with_index do |e, i|
        /#=> *(.*)/ =~ e or next
        a = f.gets
        assert_not_nil(a, "[ruby-dev:34445]: remained")
        assert_equal($1, a.chomp, "[ruby-dev:34445]: line #{line+i}")
      end
    end
  end

  def test_argf
    assert_src_expected(__LINE__+1, <<-'SRC')
      a = ARGF
      b = a.dup
      p [a.gets.chomp, a.lineno, b.gets.chomp, b.lineno] #=> ["1", 1, "1", 1]
      p [a.gets.chomp, a.lineno, b.gets.chomp, b.lineno] #=> ["2", 2, "2", 2]
      a.rewind
      b.rewind
      p [a.gets.chomp, a.lineno, b.gets.chomp, b.lineno] #=> ["1", 1, "1", 3]
      p [a.gets.chomp, a.lineno, b.gets.chomp, b.lineno] #=> ["2", 2, "2", 4]
      p [a.gets.chomp, a.lineno, b.gets.chomp, b.lineno] #=> ["3", 3, "3", 5]
      p [a.gets.chomp, a.lineno, b.gets.chomp, b.lineno] #=> ["4", 4, "4", 6]
      p [a.gets.chomp, a.lineno, b.gets.chomp, b.lineno] #=> ["5", 5, "5", 7]
      a.rewind
      b.rewind
      p [a.gets.chomp, a.lineno, b.gets.chomp, b.lineno] #=> ["5", 5, "5", 8]
      p [a.gets.chomp, a.lineno, b.gets.chomp, b.lineno] #=> ["6", 6, "6", 9]
    SRC
  end

  def test_lineno
    assert_src_expected(__LINE__+1, <<-'SRC')
      a = ARGF
      a.gets; p $.  #=> 1
      a.gets; p $.  #=> 2
      a.gets; p $.  #=> 3
      a.rewind; p $.  #=> 3
      a.gets; p $.  #=> 3
      a.gets; p $.  #=> 4
      a.rewind; p $.  #=> 4
      a.gets; p $.  #=> 3
      a.lineno = 1000; p $.  #=> 1000
      a.gets; p $.  #=> 1001
      a.gets; p $.  #=> 1002
      $. = 2000
      a.gets; p $.  #=> 2001
      a.gets; p $.  #=> 2001
    SRC
  end

  def test_lineno2
    assert_src_expected(__LINE__+1, <<-'SRC')
      a = ARGF.dup
      a.gets; p $.  #=> 1
      a.gets; p $.  #=> 2
      a.gets; p $.  #=> 1
      a.rewind; p $.  #=> 1
      a.gets; p $.  #=> 1
      a.gets; p $.  #=> 2
      a.gets; p $.  #=> 1
      a.lineno = 1000; p $.  #=> 1
      a.gets; p $.  #=> 2
      a.gets; p $.  #=> 2
      $. = 2000
      a.gets; p $.  #=> 2000
      a.gets; p $.  #=> 2000
    SRC
  end

  def test_lineno3
    assert_in_out_err(["-", @t1.path, @t2.path], <<-INPUT, %w"1 1 1 2 2 2 3 3 1 4 4 2", [], "[ruby-core:25205]")
      ARGF.each do |line|
        puts [$., ARGF.lineno, ARGF.file.lineno]
      end
    INPUT
  end

  def test_inplace
    assert_in_out_err(["-", @t1.path, @t2.path, @t3.path], <<-INPUT, [], [])
      ARGF.inplace_mode = '.bak'
      while line = ARGF.gets
        puts line.chomp + '.new'
      end
    INPUT
    assert_equal("1.new\n2.new\n", File.read(@t1.path))
    assert_equal("3.new\n4.new\n", File.read(@t2.path))
    assert_equal("5.new\n6.new\n", File.read(@t3.path))
    assert_equal("1\n2\n", File.read(@t1.path + ".bak"))
    assert_equal("3\n4\n", File.read(@t2.path + ".bak"))
    assert_equal("5\n6\n", File.read(@t3.path + ".bak"))
  end

  def test_inplace2
    assert_in_out_err(["-", @t1.path, @t2.path, @t3.path], <<-INPUT, [], [])
      ARGF.inplace_mode = '.bak'
      puts ARGF.gets.chomp + '.new'
      puts ARGF.gets.chomp + '.new'
      p ARGF.inplace_mode
      ARGF.inplace_mode = nil
      puts ARGF.gets.chomp + '.new'
      puts ARGF.gets.chomp + '.new'
      p ARGF.inplace_mode
      ARGF.inplace_mode = '.bak'
      puts ARGF.gets.chomp + '.new'
      p ARGF.inplace_mode
      ARGF.inplace_mode = nil
      puts ARGF.gets.chomp + '.new'
    INPUT
    assert_equal("1.new\n2.new\n\".bak\"\n3.new\n4.new\nnil\n", File.read(@t1.path))
    assert_equal("3\n4\n", File.read(@t2.path))
    assert_equal("5.new\n\".bak\"\n6.new\n", File.read(@t3.path))
    assert_equal("1\n2\n", File.read(@t1.path + ".bak"))
    assert_equal(false, File.file?(@t2.path + ".bak"))
    assert_equal("5\n6\n", File.read(@t3.path + ".bak"))
  end

  def test_inplace3
    assert_in_out_err(["-i.bak", "-", @t1.path, @t2.path, @t3.path], <<-INPUT, [], [])
      puts ARGF.gets.chomp + '.new'
      puts ARGF.gets.chomp + '.new'
      p $-i
      $-i = nil
      puts ARGF.gets.chomp + '.new'
      puts ARGF.gets.chomp + '.new'
      p $-i
      $-i = '.bak'
      puts ARGF.gets.chomp + '.new'
      p $-i
      $-i = nil
      puts ARGF.gets.chomp + '.new'
    INPUT
    assert_equal("1.new\n2.new\n\".bak\"\n3.new\n4.new\nnil\n", File.read(@t1.path))
    assert_equal("3\n4\n", File.read(@t2.path))
    assert_equal("5.new\n\".bak\"\n6.new\n", File.read(@t3.path))
    assert_equal("1\n2\n", File.read(@t1.path + ".bak"))
    assert_equal(false, File.file?(@t2.path + ".bak"))
    assert_equal("5\n6\n", File.read(@t3.path + ".bak"))
  end

  def test_inplace_rename_impossible
    t = make_tempfile

    assert_in_out_err(["-", t.path], <<-INPUT) do |r, e|
      ARGF.inplace_mode = '/\\\\:'
      while line = ARGF.gets
        puts line.chomp + '.new'
      end
    INPUT
      assert_match(/Can't rename .* to .*: .*. skipping file/, e.first) #'
      assert_equal([], r)
      assert_equal("foo\nbar\nbaz\n", File.read(t.path))
    end
  end

  def test_inplace_no_backup
    t = make_tempfile

    assert_in_out_err(["-", t.path], <<-INPUT) do |r, e|
      ARGF.inplace_mode = ''
      while line = ARGF.gets
        puts line.chomp + '.new'
      end
    INPUT
      if no_safe_rename
        assert_match(/Can't do inplace edit without backup/, e.join) #'
      else
        assert_equal([], e)
        assert_equal([], r)
        assert_equal("foo.new\nbar.new\nbaz.new\n", File.read(t.path))
      end
    end
  end

  def test_inplace_dup
    t = make_tempfile

    assert_in_out_err(["-", t.path], <<-INPUT, [], [])
      ARGF.inplace_mode = '.bak'
      f = ARGF.dup
      while line = f.gets
        puts line.chomp + '.new'
      end
    INPUT
    assert_equal("foo.new\nbar.new\nbaz.new\n", File.read(t.path))
  end

  def test_inplace_stdin
    t = make_tempfile

    assert_in_out_err(["-", "-"], <<-INPUT, [], /Can't do inplace edit for stdio; skipping/)
      ARGF.inplace_mode = '.bak'
      f = ARGF.dup
      while line = f.gets
        puts line.chomp + '.new'
      end
    INPUT
  end

  def test_inplace_stdin2
    t = make_tempfile

    assert_in_out_err(["-"], <<-INPUT, [], /Can't do inplace edit for stdio/)
      ARGF.inplace_mode = '.bak'
      while line = ARGF.gets
        puts line.chomp + '.new'
      end
    INPUT
  end

  def test_encoding
    ruby('-e', <<-SRC, @t1.path, @t2.path, @t3.path) do |f|
      p ARGF.external_encoding.is_a?(Encoding)
      p ARGF.internal_encoding.is_a?(Encoding)
      ARGF.gets
      p ARGF.external_encoding.is_a?(Encoding)
      p ARGF.internal_encoding
    SRC
      assert_equal("true\ntrue\ntrue\nnil\n", f.read)
    end
  end

  def test_tell
    ruby('-e', <<-SRC, @t1.path, @t2.path, @t3.path) do |f|
      begin
        ARGF.binmode
        loop do
          p ARGF.tell
          p ARGF.gets
        end
      rescue ArgumentError
        puts "end"
      end
    SRC
      a = f.read.split("\n")
      [0, 2, 4, 2, 4, 2, 4].map {|i| i.to_s }.
      zip((1..6).map {|i| '"' + i.to_s + '\n"' } + ["nil"]).flatten.
      each do |x|
        assert_equal(x, a.shift)
      end
      assert_equal('end', a.shift)
    end
  end

  def test_seek
    assert_src_expected(__LINE__+1, <<-'SRC')
      ARGF.seek(4)
      p ARGF.gets #=> "3\n"
      ARGF.seek(0, IO::SEEK_END)
      p ARGF.gets #=> "5\n"
      ARGF.seek(4)
      p ARGF.gets #=> nil
      begin
        ARGF.seek(0)
      rescue
        puts "end" #=> end
      end
    SRC
  end

  def test_set_pos
    assert_src_expected(__LINE__+1, <<-'SRC')
      ARGF.pos = 4
      p ARGF.gets #=> "3\n"
      ARGF.pos = 4
      p ARGF.gets #=> "5\n"
      ARGF.pos = 4
      p ARGF.gets #=> nil
      begin
        ARGF.pos = 4
      rescue
        puts "end" #=> end
      end
    SRC
  end

  def test_rewind
    assert_src_expected(__LINE__+1, <<-'SRC')
      ARGF.pos = 4
      ARGF.rewind
      p ARGF.gets #=> "1\n"
      ARGF.pos = 4
      p ARGF.gets #=> "3\n"
      ARGF.pos = 4
      p ARGF.gets #=> "5\n"
      ARGF.pos = 4
      p ARGF.gets #=> nil
      begin
        ARGF.rewind
      rescue
        puts "end" #=> end
      end
    SRC
  end

  def test_fileno
    ruby('-e', <<-SRC, @t1.path, @t2.path, @t3.path) do |f|
      p ARGF.fileno
      ARGF.gets
      ARGF.gets
      p ARGF.fileno
      ARGF.gets
      ARGF.gets
      p ARGF.fileno
      ARGF.gets
      ARGF.gets
      p ARGF.fileno
      ARGF.gets
      begin
        ARGF.fileno
      rescue
        puts "end"
      end
    SRC
      a = f.read.split("\n")
      fd1, fd2, fd3, fd4, tag = a
      assert_match(/^\d+$/, fd1)
      assert_match(/^\d+$/, fd2)
      assert_match(/^\d+$/, fd3)
      assert_match(/^\d+$/, fd4)
      assert_equal('end', tag)
    end
  end

  def test_to_io
    ruby('-e', <<-SRC, @t1.path, @t2.path, @t3.path) do |f|
      8.times do
        p ARGF.to_io
        ARGF.gets
      end
    SRC
      a = f.read.split("\n")
      f11, f12, f13, f21, f22, f31, f32, f4 = a
      assert_equal(f11, f12)
      assert_equal(f11, f13)
      assert_equal(f21, f22)
      assert_equal(f31, f32)
      assert_match(/\(closed\)/, f4)
      f4.sub!(/ \(closed\)/, "")
      assert_equal(f31, f4)
    end
  end

  def test_eof
    ruby('-e', <<-SRC, @t1.path, @t2.path, @t3.path) do |f|
      begin
        8.times do
          p ARGF.eof?
          ARGF.gets
        end
      rescue IOError
        puts "end"
      end
    SRC
      a = f.read.split("\n")
      (%w(false) + (%w(false true) * 3) + %w(end)).each do |x|
        assert_equal(x, a.shift)
      end
    end

    t1 = Tempfile.new("argf-foo")
    t1.binmode
    t1.puts "foo"
    t1.close
    t2 = Tempfile.new("argf-bar")
    t2.binmode
    t2.puts "bar"
    t2.close
    ruby('-e', 'STDERR.reopen(STDOUT); ARGF.gets; ARGF.skip; p ARGF.eof?', t1.path, t2.path) do |f|
      assert_equal(%w(false), f.read.split(/\n/))
    end
  end

  def test_read
    ruby('-e', "p ARGF.read(8)", @t1.path, @t2.path, @t3.path) do |f|
      assert_equal("\"1\\n2\\n3\\n4\\n\"\n", f.read)
    end
  end

  def test_read2
    ruby('-e', <<-SRC, @t1.path, @t2.path, @t3.path) do |f|
      s = ""
      ARGF.read(8, s)
      p s
    SRC
      assert_equal("\"1\\n2\\n3\\n4\\n\"\n", f.read)
    end
  end

  def test_read3
    ruby('-e', <<-SRC, @t1.path, @t2.path, @t3.path) do |f|
      nil while ARGF.gets
      p ARGF.read
      p ARGF.read(0, "")
    SRC
      assert_equal("nil\n\"\"\n", f.read)
    end
  end

  def test_readpartial
    ruby('-e', <<-SRC, @t1.path, @t2.path, @t3.path) do |f|
      s = ""
      begin
        loop do
          s << ARGF.readpartial(1)
          t = ""; ARGF.readpartial(1, t); s << t
        end
      rescue EOFError
        puts s
      end
    SRC
      assert_equal("1\n2\n3\n4\n5\n6\n", f.read)
    end
  end

  def test_readpartial2
    ruby('-e', <<-SRC) do |f|
      s = ""
      begin
        loop do
          s << ARGF.readpartial(1)
          t = ""; ARGF.readpartial(1, t); s << t
        end
      rescue EOFError
        $stdout.binmode
        puts s
      end
    SRC
      f.binmode
      f.puts("foo")
      f.puts("bar")
      f.puts("baz")
      f.close_write
      assert_equal("foo\nbar\nbaz\n", f.read)
    end
  end

  def test_getc
    ruby('-e', <<-SRC, @t1.path, @t2.path, @t3.path) do |f|
      s = ""
      while c = ARGF.getc
        s << c
      end
      puts s
    SRC
      assert_equal("1\n2\n3\n4\n5\n6\n", f.read)
    end
  end

  def test_getbyte
    ruby('-e', <<-SRC, @t1.path, @t2.path, @t3.path) do |f|
      s = []
      while c = ARGF.getbyte
        s << c
      end
      p s
    SRC
      assert_equal("[49, 10, 50, 10, 51, 10, 52, 10, 53, 10, 54, 10]\n", f.read)
    end
  end

  def test_readchar
    ruby('-e', <<-SRC, @t1.path, @t2.path, @t3.path) do |f|
      s = ""
      begin
        while c = ARGF.readchar
          s << c
        end
      rescue EOFError
        puts s
      end
    SRC
      assert_equal("1\n2\n3\n4\n5\n6\n", f.read)
    end
  end

  def test_readbyte
    ruby('-e', <<-SRC, @t1.path, @t2.path, @t3.path) do |f|
      begin
        s = []
        while c = ARGF.readbyte
          s << c
        end
      rescue EOFError
        p s
      end
    SRC
      assert_equal("[49, 10, 50, 10, 51, 10, 52, 10, 53, 10, 54, 10]\n", f.read)
    end
  end

  def test_each_line
    ruby('-e', <<-SRC, @t1.path, @t2.path, @t3.path) do |f|
      s = []
      ARGF.each_line {|l| s << l }
      p s
    SRC
      assert_equal("[\"1\\n\", \"2\\n\", \"3\\n\", \"4\\n\", \"5\\n\", \"6\\n\"]\n", f.read)
    end
  end

  def test_each_line_paragraph
    assert_in_out_err(['-e', 'ARGF.each_line("") {|para| p para}'], "a\n\nb\n",
                      ["\"a\\n\\n\"", "\"b\\n\""], [])
  end

  def test_each_byte
    ruby('-e', <<-SRC, @t1.path, @t2.path, @t3.path) do |f|
      s = []
      ARGF.each_byte {|c| s << c }
      p s
    SRC
      assert_equal("[49, 10, 50, 10, 51, 10, 52, 10, 53, 10, 54, 10]\n", f.read)
    end
  end

  def test_each_char
    ruby('-e', <<-SRC, @t1.path, @t2.path, @t3.path) do |f|
      s = ""
      ARGF.each_char {|c| s << c }
      puts s
    SRC
      assert_equal("1\n2\n3\n4\n5\n6\n", f.read)
    end
  end

  def test_filename
    ruby('-e', <<-SRC, @t1.path, @t2.path, @t3.path) do |f|
      begin
        puts ARGF.filename.dump
      end while ARGF.gets
      puts ARGF.filename.dump
    SRC
      a = f.read.split("\n")
      assert_equal(@t1.path.dump, a.shift)
      assert_equal(@t1.path.dump, a.shift)
      assert_equal(@t1.path.dump, a.shift)
      assert_equal(@t2.path.dump, a.shift)
      assert_equal(@t2.path.dump, a.shift)
      assert_equal(@t3.path.dump, a.shift)
      assert_equal(@t3.path.dump, a.shift)
      assert_equal(@t3.path.dump, a.shift)
    end
  end

  def test_filename2
    ruby('-e', <<-SRC, @t1.path, @t2.path, @t3.path) do |f|
      begin
        puts $FILENAME.dump
      end while ARGF.gets
      puts $FILENAME.dump
    SRC
      a = f.read.split("\n")
      assert_equal(@t1.path.dump, a.shift)
      assert_equal(@t1.path.dump, a.shift)
      assert_equal(@t1.path.dump, a.shift)
      assert_equal(@t2.path.dump, a.shift)
      assert_equal(@t2.path.dump, a.shift)
      assert_equal(@t3.path.dump, a.shift)
      assert_equal(@t3.path.dump, a.shift)
      assert_equal(@t3.path.dump, a.shift)
    end
  end

  def test_file
    ruby('-e', <<-SRC, @t1.path, @t2.path, @t3.path) do |f|
      begin
        puts ARGF.file.path.dump
      end while ARGF.gets
      puts ARGF.file.path.dump
    SRC
      a = f.read.split("\n")
      assert_equal(@t1.path.dump, a.shift)
      assert_equal(@t1.path.dump, a.shift)
      assert_equal(@t1.path.dump, a.shift)
      assert_equal(@t2.path.dump, a.shift)
      assert_equal(@t2.path.dump, a.shift)
      assert_equal(@t3.path.dump, a.shift)
      assert_equal(@t3.path.dump, a.shift)
      assert_equal(@t3.path.dump, a.shift)
    end
  end

  def test_binmode
    bug5268 = '[ruby-core:39234]'
    open(@t3.path, "wb") {|f| f.write "5\r\n6\r\n"}
    ruby('-e', "ARGF.binmode; STDOUT.binmode; puts ARGF.read", @t1.path, @t2.path, @t3.path) do |f|
      f.binmode
      assert_equal("1\n2\n3\n4\n5\r\n6\r\n", f.read, bug5268)
    end
  end

  def test_textmode
    bug5268 = '[ruby-core:39234]'
    open(@t3.path, "wb") {|f| f.write "5\r\n6\r\n"}
    ruby('-e', "STDOUT.binmode; puts ARGF.read", @t1.path, @t2.path, @t3.path) do |f|
      f.binmode
      assert_equal("1\n2\n3\n4\n5\n6\n", f.read, bug5268)
    end
  end unless IO::BINARY.zero?

  def test_skip
    ruby('-e', <<-SRC, @t1.path, @t2.path, @t3.path) do |f|
      ARGF.skip
      puts ARGF.gets
      ARGF.skip
      puts ARGF.read
    SRC
      assert_equal("1\n3\n4\n5\n6\n", f.read)
    end
  end

  def test_close
    ruby('-e', <<-SRC, @t1.path, @t2.path, @t3.path) do |f|
      ARGF.close
      puts ARGF.read
    SRC
      assert_equal("3\n4\n5\n6\n", f.read)
    end
  end

  def test_close_replace
    ruby('-e', <<-SRC) do |f|
      ARGF.close
      ARGV.replace ['#{@t1.path}', '#{@t2.path}', '#{@t3.path}']
      puts ARGF.read
    SRC
      assert_equal("1\n2\n3\n4\n5\n6\n", f.read)
    end
  end

  def test_closed
    ruby('-e', <<-SRC, @t1.path, @t2.path, @t3.path) do |f|
      3.times do
        p ARGF.closed?
        ARGF.gets
        ARGF.gets
      end
      p ARGF.closed?
      ARGF.gets
      p ARGF.closed?
    SRC
      assert_equal("false\nfalse\nfalse\nfalse\ntrue\n", f.read)
    end
  end

  def test_argv
    ruby('-e', "p ARGF.argv; p $*", @t1.path, @t2.path, @t3.path) do |f|
      assert_equal([@t1.path, @t2.path, @t3.path].inspect, f.gets.chomp)
      assert_equal([@t1.path, @t2.path, @t3.path].inspect, f.gets.chomp)
    end
  end

  def test_readlines_limit_0
    bug4024 = '[ruby-dev:42538]'
    t = make_tempfile
    argf = ARGF.class.new(t.path)
    begin
      assert_raise(ArgumentError, bug4024) do
        argf.readlines(0)
      end
    ensure
      argf.close
    end
  end

  def test_each_line_limit_0
    bug4024 = '[ruby-dev:42538]'
    t = make_tempfile
    argf = ARGF.class.new(t.path)
    begin
      assert_raise(ArgumentError, bug4024) do
        argf.each_line(0).next
      end
    ensure
      argf.close
    end
  end

  def test_unreadable
    bug4274 = '[ruby-core:34446]'
    paths = (1..2).map do
      t = Tempfile.new("bug4274-")
      path = t.path
      t.close!
      path
    end
    argf = ARGF.class.new(*paths)
    paths.each do |path|
      e = assert_raise(Errno::ENOENT) {argf.gets}
      assert_match(/- #{Regexp.quote(path)}\z/, e.message)
    end
    assert_nil(argf.gets, bug4274)
  end

  def test_readlines_twice
    bug5952 = '[ruby-dev:45160]'
    assert_ruby_status(["-e", "2.times {STDIN.tty?; readlines}"], "", bug5952)
  end
end
