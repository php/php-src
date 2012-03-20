begin
  require 'io/console'
  require 'test/unit'
  require 'pty'
rescue LoadError
end
require_relative '../../ruby/envutil'

class TestIO_Console < Test::Unit::TestCase
  Bug6116 = '[ruby-dev:45309]'

  def test_raw
    helper {|m, s|
      s.print "abc\n"
      assert_equal("abc\r\n", m.gets)
      assert_send([s, :echo?])
      s.raw {
        assert_not_send([s, :echo?], Bug6116)
        s.print "def\n"
        assert_equal("def\n", m.gets)
      }
      assert_send([s, :echo?])
      s.print "ghi\n"
      assert_equal("ghi\r\n", m.gets)
    }
  end

  def test_cooked
    helper {|m, s|
      assert_send([s, :echo?])
      s.raw {
        s.print "abc\n"
        assert_equal("abc\n", m.gets)
        assert_not_send([s, :echo?], Bug6116)
        s.cooked {
          assert_send([s, :echo?])
          s.print "def\n"
          assert_equal("def\r\n", m.gets)
        }
        assert_not_send([s, :echo?], Bug6116)
      }
      assert_send([s, :echo?])
      s.print "ghi\n"
      assert_equal("ghi\r\n", m.gets)
    }
  end

  def test_echo
    helper {|m, s|
      assert_send([s, :echo?])
      m.print "a"
      assert_equal("a", m.readpartial(10))
    }
  end

  def test_noecho
    helper {|m, s|
      s.noecho {
	assert_not_send([s, :echo?])
	m.print "a"
	sleep 0.1
      }
      m.print "b"
      assert_equal("b", m.readpartial(10))
    }
  end

  def test_noecho2
    helper {|m, s|
      assert_send([s, :echo?])
      m.print "a\n"
      sleep 0.1
      s.print "b\n"
      sleep 0.1
      assert_equal("a\r\nb\r\n", m.readpartial(10))
      assert_equal("a\n", s.readpartial(10))
      s.noecho {
        assert_not_send([s, :echo?])
        m.print "a\n"
        s.print "b\n"
        assert_equal("b\r\n", m.readpartial(10))
        assert_equal("a\n", s.readpartial(10))
      }
      assert_send([s, :echo?])
      m.print "a\n"
      sleep 0.1
      s.print "b\n"
      sleep 0.1
      assert_equal("a\r\nb\r\n", m.readpartial(10))
      assert_equal("a\n", s.readpartial(10))
    }
  end

  def test_setecho
    helper {|m, s|
      assert_send([s, :echo?])
      s.echo = false
      m.print "a"
      sleep 0.1
      s.echo = true
      m.print "b"
      assert_equal("b", m.readpartial(10))
    }
  end

  def test_setecho2
    helper {|m, s|
      assert_send([s, :echo?])
      m.print "a\n"
      sleep 0.1
      s.print "b\n"
      sleep 0.1
      assert_equal("a\r\nb\r\n", m.readpartial(10))
      assert_equal("a\n", s.readpartial(10))
      s.echo = false
      assert_not_send([s, :echo?])
      m.print "a\n"
      s.print "b\n"
      assert_equal("b\r\n", m.readpartial(10))
      assert_equal("a\n", s.readpartial(10))
      s.echo = true
      assert_send([s, :echo?])
      m.print "a\n"
      sleep 0.1
      s.print "b\n"
      sleep 0.1
      assert_equal("a\r\nb\r\n", m.readpartial(10))
      assert_equal("a\n", s.readpartial(10))
    }
  end

  def test_iflush
    helper {|m, s|
      m.print "a"
      s.iflush
      m.print "b\n"
      assert_equal("b\n", s.readpartial(10))
    }
  end

  def test_oflush
    helper {|m, s|
      s.print "a"
      s.oflush # oflush may be issued after "a" is already sent.
      s.print "b"
      assert_include(["b", "ab"], m.readpartial(10))
    }
  end

  def test_ioflush
    helper {|m, s|
      m.print "a"
      s.ioflush
      m.print "b\n"
      assert_equal("b\n", s.readpartial(10))
    }
  end

  def test_ioflush2
    helper {|m, s|
      s.print "a"
      s.ioflush # ioflush may be issued after "a" is already sent.
      s.print "b"
      assert_include(["b", "ab"], m.readpartial(10))
    }
  end

  def test_winsize
    helper {|m, s|
      begin
        assert_equal([0, 0], s.winsize)
      rescue Errno::EINVAL # OpenSolaris 2009.06 TIOCGWINSZ causes Errno::EINVAL before TIOCSWINSZ.
      end
    }
  end

  if IO.console
    def test_sync
      assert(IO.console.sync, "console should be unbuffered")
    end
  else
    def test_sync
      r, w, pid = PTY.spawn(EnvUtil.rubybin, "-rio/console", "-e", "p IO.console.class")
    rescue RuntimeError
      skip $!
    else
      con = r.gets.chomp
      Process.wait(pid)
      assert_match("File", con)
    end
  end

  private
  def helper
    m, s = PTY.open
  rescue RuntimeError
    skip $!
  else
    yield m, s
  ensure
    m.close if m
    s.close if s
  end
end if defined?(PTY) and defined?(IO::console)

class TestIO_Console < Test::Unit::TestCase
  case
  when Process.respond_to?(:daemon)
    noctty = [EnvUtil.rubybin, "-e", "Process.daemon(true)"]
  when !(rubyw = RbConfig::CONFIG["RUBYW_INSTALL_NAME"]).empty?
    dir, base = File.split(EnvUtil.rubybin)
    noctty = [File.join(dir, base.sub(/ruby/, rubyw))]
  end

  if noctty
    require 'tempfile'
    NOCTTY = noctty
    def test_noctty
      t = Tempfile.new("console")
      t.close
      t2 = Tempfile.new("console")
      t2.close
      cmd = NOCTTY + [
        '--disable=gems',
        '-rio/console',
        '-e', 'open(ARGV[0], "w") {|f| f.puts IO.console.inspect}',
        '-e', 'File.unlink(ARGV[1])',
        '--', t.path, t2.path]
      system(*cmd)
      sleep 0.1 while File.exist?(t2.path)
      t.open
      assert_equal("nil", t.gets.chomp)
    ensure
      t.close! if t and !t.closed?
      t2.close!
    end
  end
end if defined?(IO.console)

class TestIO_Console < Test::Unit::TestCase
  def test_stringio_getch
    assert_ruby_status(%w"--disable=gems -rstringio -rio/console", "exit(StringIO.method_defined?(:getch))")
    assert_ruby_status(%w"--disable=gems -rio/console -rstringio", "exit(StringIO.method_defined?(:getch))")
    assert_ruby_status(%w"--disable=gems -rstringio", "exit(!StringIO.method_defined?(:getch))")
  end
end
