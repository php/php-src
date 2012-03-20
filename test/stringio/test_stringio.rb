require 'test/unit'
require 'stringio'
require_relative '../ruby/ut_eof'

class TestStringIO < Test::Unit::TestCase
  include TestEOF
  def open_file(content)
    f = StringIO.new(content)
    yield f
  end
  alias open_file_rw open_file

  include TestEOF::Seek

  def test_truncate
    io = StringIO.new("")
    io.puts "abc"
    io.truncate(0)
    io.puts "def"
    assert_equal("\0\0\0\0def\n", io.string, "[ruby-dev:24190]")
    assert_raise(Errno::EINVAL) { io.truncate(-1) }
    io.truncate(10)
    assert_equal("\0\0\0\0def\n\0\0", io.string)
  end

  def test_seek_beyond_eof
    io = StringIO.new
    n = 100
    io.seek(n)
    io.print "last"
    assert_equal("\0" * n + "last", io.string, "[ruby-dev:24194]")
  end

  def test_overwrite
    stringio = StringIO.new
    responses = ['', 'just another ruby', 'hacker']
    responses.each do |resp|
      stringio.puts(resp)
      stringio.rewind
    end
    assert_equal("hacker\nother ruby\n", stringio.string, "[ruby-core:3836]")
  end

  def test_gets
    assert_equal(nil, StringIO.new("").gets)
    assert_equal("\n", StringIO.new("\n").gets)
    assert_equal("a\n", StringIO.new("a\n").gets)
    assert_equal("a\n", StringIO.new("a\nb\n").gets)
    assert_equal("a", StringIO.new("a").gets)
    assert_equal("a\n", StringIO.new("a\nb").gets)
    assert_equal("abc\n", StringIO.new("abc\n\ndef\n").gets)
    assert_equal("abc\n\ndef\n", StringIO.new("abc\n\ndef\n").gets(nil))
    assert_equal("abc\n\n", StringIO.new("abc\n\ndef\n").gets(""))
    assert_raise(TypeError){StringIO.new("").gets(1, 1)}
    assert_raise(TypeError){StringIO.new("").gets(nil, nil)}
  end

  def test_readlines
    assert_equal([], StringIO.new("").readlines)
    assert_equal(["\n"], StringIO.new("\n").readlines)
    assert_equal(["a\n"], StringIO.new("a\n").readlines)
    assert_equal(["a\n", "b\n"], StringIO.new("a\nb\n").readlines)
    assert_equal(["a"], StringIO.new("a").readlines)
    assert_equal(["a\n", "b"], StringIO.new("a\nb").readlines)
    assert_equal(["abc\n", "\n", "def\n"], StringIO.new("abc\n\ndef\n").readlines)
    assert_equal(["abc\n\ndef\n"], StringIO.new("abc\n\ndef\n").readlines(nil), "[ruby-dev:34591]")
    assert_equal(["abc\n\n", "def\n"], StringIO.new("abc\n\ndef\n").readlines(""))
  end

  def test_write
    s = ""
    f = StringIO.new(s, "w")
    f.print("foo")
    f.close
    assert_equal("foo", s)

    f = StringIO.new(s, File::WRONLY)
    f.print("bar")
    f.close
    assert_equal("bar", s)

    f = StringIO.new(s, "a")
    o = Object.new
    def o.to_s; "baz"; end
    f.print(o)
    f.close
    assert_equal("barbaz", s)
  ensure
    f.close unless f.closed?
  end

  def test_write_nonblock
    s = ""
    f = StringIO.new(s, "w")
    f.write_nonblock("foo")
    f.close
    assert_equal("foo", s)

    f = StringIO.new(s, File::WRONLY)
    f.write_nonblock("bar")
    f.close
    assert_equal("bar", s)

    f = StringIO.new(s, "a")
    o = Object.new
    def o.to_s; "baz"; end
    f.write_nonblock(o)
    f.close
    assert_equal("barbaz", s)
  ensure
    f.close unless f.closed?
  end

  def test_mode_error
    f = StringIO.new("", "r")
    assert_raise(IOError) { f.write("foo") }

    f = StringIO.new("", "w")
    assert_raise(IOError) { f.read }

    assert_raise(Errno::EACCES) { StringIO.new("".freeze, "w") }
    s = ""
    f = StringIO.new(s, "w")
    s.freeze
    assert_raise(IOError) { f.write("foo") }

    assert_raise(IOError) { StringIO.allocate.read }
  ensure
    f.close unless f.closed?
  end

  def test_open
    s = ""
    StringIO.open("foo") {|f| s = f.read }
    assert_equal("foo", s)
  end

  def test_isatty
    assert_equal(false, StringIO.new("").isatty)
  end

  def test_fsync
    assert_equal(0, StringIO.new("").fsync)
  end

  def test_sync
    assert_equal(true, StringIO.new("").sync)
    assert_equal(false, StringIO.new("").sync = false)
  end

  def test_set_fcntl
    assert_raise(NotImplementedError) { StringIO.new("").fcntl }
  end

  def test_close
    f = StringIO.new("")
    f.close
    assert_raise(IOError) { f.close }

    f = StringIO.new("")
    f.close_read
    f.close_write
    assert_raise(IOError) { f.close }
  ensure
    f.close unless f.closed?
  end

  def test_close_read
    f = StringIO.new("")
    f.close_read
    assert_raise(IOError) { f.read }
    assert_raise(IOError) { f.close_read }
    f.close

    f = StringIO.new("", "w")
    assert_raise(IOError) { f.close_read }
    f.close
  ensure
    f.close unless f.closed?
  end

  def test_close_write
    f = StringIO.new("")
    f.close_write
    assert_raise(IOError) { f.write("foo") }
    assert_raise(IOError) { f.close_write }
    f.close

    f = StringIO.new("", "r")
    assert_raise(IOError) { f.close_write }
    f.close
  ensure
    f.close unless f.closed?
  end

  def test_closed
    f = StringIO.new("")
    assert_equal(false, f.closed?)
    f.close
    assert_equal(true, f.closed?)
  ensure
    f.close unless f.closed?
  end

  def test_closed_read
    f = StringIO.new("")
    assert_equal(false, f.closed_read?)
    f.close_write
    assert_equal(false, f.closed_read?)
    f.close_read
    assert_equal(true, f.closed_read?)
  ensure
    f.close unless f.closed?
  end

  def test_closed_write
    f = StringIO.new("")
    assert_equal(false, f.closed_write?)
    f.close_read
    assert_equal(false, f.closed_write?)
    f.close_write
    assert_equal(true, f.closed_write?)
  ensure
    f.close unless f.closed?
  end

  def test_dup
    f1 = StringIO.new("1234")
    assert_equal("1", f1.getc)
    f2 = f1.dup
    assert_equal("2", f2.getc)
    assert_equal("3", f1.getc)
    assert_equal("4", f2.getc)
    assert_equal(nil, f1.getc)
    assert_equal(true, f2.eof?)
    f1.close
    assert_equal(true, f2.closed?)
  ensure
    f1.close unless f1.closed?
    f2.close unless f2.closed?
  end

  def test_lineno
    f = StringIO.new("foo\nbar\nbaz\n")
    assert_equal([0, "foo\n"], [f.lineno, f.gets])
    assert_equal([1, "bar\n"], [f.lineno, f.gets])
    f.lineno = 1000
    assert_equal([1000, "baz\n"], [f.lineno, f.gets])
    assert_equal([1001, nil], [f.lineno, f.gets])
  ensure
    f.close unless f.closed?
  end

  def test_pos
    f = StringIO.new("foo\nbar\nbaz\n")
    assert_equal([0, "foo\n"], [f.pos, f.gets])
    assert_equal([4, "bar\n"], [f.pos, f.gets])
    assert_raise(Errno::EINVAL) { f.pos = -1 }
    f.pos = 1
    assert_equal([1, "oo\n"], [f.pos, f.gets])
    assert_equal([4, "bar\n"], [f.pos, f.gets])
    assert_equal([8, "baz\n"], [f.pos, f.gets])
    assert_equal([12, nil], [f.pos, f.gets])
  ensure
    f.close unless f.closed?
  end

  def test_reopen
    f = StringIO.new("foo\nbar\nbaz\n")
    assert_equal("foo\n", f.gets)
    f.reopen("qux\nquux\nquuux\n")
    assert_equal("qux\n", f.gets)

    f2 = StringIO.new("")
    f2.reopen(f)
    assert_equal("quux\n", f2.gets)
  ensure
    f.close unless f.closed?
  end

  def test_seek
    f = StringIO.new("1234")
    assert_raise(Errno::EINVAL) { f.seek(-1) }
    f.seek(-1, 2)
    assert_equal("4", f.getc)
    assert_raise(Errno::EINVAL) { f.seek(1, 3) }
    f.close
    assert_raise(IOError) { f.seek(0) }
  ensure
    f.close unless f.closed?
  end

  def test_each_byte
    f = StringIO.new("1234")
    a = []
    f.each_byte {|c| a << c }
    assert_equal(%w(1 2 3 4).map {|c| c.ord }, a)
  ensure
    f.close unless f.closed?
  end

  def test_getbyte
    f = StringIO.new("1234")
    assert_equal("1".ord, f.getbyte)
    assert_equal("2".ord, f.getbyte)
    assert_equal("3".ord, f.getbyte)
    assert_equal("4".ord, f.getbyte)
    assert_equal(nil, f.getbyte)
  ensure
    f.close unless f.closed?
  end

  def test_ungetbyte
    s = "foo\nbar\n"
    t = StringIO.new(s, "r")
    t.ungetbyte(0x41)
    assert_equal(0x41, t.getbyte)
    t.ungetbyte("qux")
    assert_equal("quxfoo\n", t.gets)
    t.set_encoding("utf-8")
    t.ungetbyte(0x89)
    t.ungetbyte(0x8e)
    t.ungetbyte("\xe7")
    t.ungetbyte("\xe7\xb4\x85")
    assert_equal("\u7d05\u7389bar\n", t.gets)
  end

  def test_ungetc
    s = "1234"
    f = StringIO.new(s, "r")
    assert_nothing_raised { f.ungetc("x") }
    assert_equal("x", f.getc) # bug? -> it's a feature from 1.9.
    assert_equal("1", f.getc)

    s = "1234"
    f = StringIO.new(s, "r")
    assert_equal("1", f.getc)
    f.ungetc("y".ord)
    assert_equal("y", f.getc)
    assert_equal("2", f.getc)
  ensure
    f.close unless f.closed?
  end

  def test_readchar
    f = StringIO.new("1234")
    a = ""
    assert_raise(EOFError) { loop { a << f.readchar } }
    assert_equal("1234", a)
  end

  def test_readbyte
    f = StringIO.new("1234")
    a = []
    assert_raise(EOFError) { loop { a << f.readbyte } }
    assert_equal("1234".unpack("C*"), a)
  end

  def test_each_char
    f = StringIO.new("1234")
    assert_equal(%w(1 2 3 4), f.each_char.to_a)
  end

  def test_each_codepoint
    f = StringIO.new("1234")
    assert_equal([49, 50, 51, 52], f.each_codepoint.to_a)
  end

  def test_gets2
    f = StringIO.new("foo\nbar\nbaz\n")
    assert_equal("fo", f.gets(2))

    o = Object.new
    def o.to_str; "z"; end
    assert_equal("o\nbar\nbaz", f.gets(o))

    f = StringIO.new("foo\nbar\nbaz\n")
    assert_equal("foo\nbar\nbaz", f.gets("az"))
    f = StringIO.new("a" * 10000 + "zz!")
    assert_equal("a" * 10000 + "zz", f.gets("zz"))
    f = StringIO.new("a" * 10000 + "zz!")
    assert_equal("a" * 10000 + "zz!", f.gets("zzz"))

    bug4112 = '[ruby-dev:42674]'
    ["a".encode("utf-16be"), "\u3042"].each do |s|
      assert_equal(s, StringIO.new(s).gets(1), bug4112)
      assert_equal(s, StringIO.new(s).gets(nil, 1), bug4112)
    end
  end

  def test_each
    f = StringIO.new("foo\nbar\nbaz\n")
    assert_equal(["foo\n", "bar\n", "baz\n"], f.each.to_a)
  end

  def test_putc
    s = ""
    f = StringIO.new(s, "w")
    f.putc("1")
    f.putc("2")
    f.putc("3")
    f.close
    assert_equal("123", s)

    s = "foo"
    f = StringIO.new(s, "a")
    f.putc("1")
    f.putc("2")
    f.putc("3")
    f.close
    assert_equal("foo123", s)
  end

  def test_read
    f = StringIO.new("\u3042\u3044")
    assert_raise(ArgumentError) { f.read(-1) }
    assert_raise(ArgumentError) { f.read(1, 2, 3) }
    assert_equal("\u3042\u3044", f.read)
    f.rewind
    assert_equal("\u3042\u3044".force_encoding(Encoding::ASCII_8BIT), f.read(f.size))

    bug5207 = '[ruby-core:39026]'
    f.rewind
    assert_equal("\u3042\u3044", f.read(nil, nil), bug5207)
    f.rewind
    s = ""
    f.read(nil, s)
    assert_equal("\u3042\u3044", s, bug5207)
  end

  def test_readpartial
    f = StringIO.new("\u3042\u3044")
    assert_raise(ArgumentError) { f.readpartial(-1) }
    assert_raise(ArgumentError) { f.readpartial(1, 2, 3) }
    assert_equal("\u3042\u3044", f.readpartial)
    f.rewind
    assert_equal("\u3042\u3044".force_encoding(Encoding::ASCII_8BIT), f.readpartial(f.size))
  end

  def test_read_nonblock
    f = StringIO.new("\u3042\u3044")
    assert_raise(ArgumentError) { f.read_nonblock(-1) }
    assert_raise(ArgumentError) { f.read_nonblock(1, 2, 3) }
    assert_equal("\u3042\u3044", f.read_nonblock)
    f.rewind
    assert_equal("\u3042\u3044".force_encoding(Encoding::ASCII_8BIT), f.read_nonblock(f.size))
  end

  def test_size
    f = StringIO.new("1234")
    assert_equal(4, f.size)
  end

  # This test is should in ruby/test_method.rb
  # However this test depends on stringio library,
  # we write it here.
  class C < StringIO
    alias old_init initialize
    attr_reader :foo
    def initialize
      @foo = :ok
      old_init
    end
  end

  def test_method
    assert_equal(:ok, C.new.foo, 'Bug #632 [ruby-core:19282]')
  end

  def test_ungetc_pos
    b = '\\b00010001 \\B00010001 \\b1 \\B1 \\b000100011'
    s = StringIO.new( b )
    expected_pos = 0
    while n = s.getc
      assert_equal( expected_pos + 1, s.pos )

      s.ungetc( n )
      assert_equal( expected_pos, s.pos )
      assert_equal( n, s.getc )

      expected_pos += 1
    end
  end

  def test_frozen
    s = StringIO.new
    s.freeze
    bug = '[ruby-core:33648]'
    assert_raise(RuntimeError, bug) {s.puts("foo")}
    assert_raise(RuntimeError, bug) {s.string = "foo"}
    assert_raise(RuntimeError, bug) {s.reopen("")}
  end

  def test_readlines_limit_0
    assert_raise(ArgumentError, "[ruby-dev:43392]") { StringIO.new.readlines(0) }
  end

  def test_each_line_limit_0
    assert_raise(ArgumentError, "[ruby-dev:43392]") { StringIO.new.each_line(0){} }
    assert_raise(ArgumentError, "[ruby-dev:43392]") { StringIO.new.each_line("a",0){} }
  end
end
