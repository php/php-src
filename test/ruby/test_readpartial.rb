require 'test/unit'
require 'timeout'
require 'fcntl'

class TestReadPartial < Test::Unit::TestCase
  def make_pipe
    r, w = IO.pipe
    r.binmode
    w.binmode
    begin
      yield r, w
    ensure
      r.close unless r.closed?
      w.close unless w.closed?
    end
  end

  def pipe
    make_pipe {|r, w|
      yield r, w
    }
    return unless defined?(Fcntl::F_SETFL)
    return unless defined?(Fcntl::F_GETFL)
    return unless defined?(Fcntl::O_NONBLOCK)
    make_pipe {|r, w|
      r.fcntl(Fcntl::F_SETFL, r.fcntl(Fcntl::F_GETFL) | Fcntl::O_NONBLOCK)
      yield r, w
    }
  end

  def test_length_zero
    pipe {|r, w|
      assert_equal('', r.readpartial(0))
    }
  end

  def test_closed_pipe
    pipe {|r, w|
      w << 'abc'
      w.close
      assert_equal('ab', r.readpartial(2))
      assert_equal('c', r.readpartial(2))
      assert_raise(EOFError) { r.readpartial(2) }
      assert_raise(EOFError) { r.readpartial(2) }
    }
  end

  def test_open_pipe
    pipe {|r, w|
      w << 'abc'
      assert_equal('ab', r.readpartial(2))
      assert_equal('c', r.readpartial(2))
      assert_raise(TimeoutError) {
        timeout(0.1) { r.readpartial(2) }
      }
    }
  end

  def test_with_stdio
    pipe {|r, w|
      w << "abc\ndef\n"
      assert_equal("abc\n", r.gets)
      w << "ghi\n"
      assert_equal("de", r.readpartial(2))
      assert_equal("f\n", r.readpartial(4096))
      assert_equal("ghi\n", r.readpartial(4096))
      assert_raise(TimeoutError) {
        timeout(0.1) { r.readpartial(2) }
      }
    }
  end
end
