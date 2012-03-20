require 'test/unit'
require 'timeout'
require 'socket'
begin
  require 'io/wait'
rescue LoadError
end

class TestIOWait < Test::Unit::TestCase

  def setup
    if /mswin|mingw/ =~ RUBY_PLATFORM
      @r, @w = Socket.pair(Socket::AF_INET, Socket::SOCK_STREAM, 0)
    else
      @r, @w = IO.pipe
    end
  end

  def teardown
    @r.close unless @r.closed?
    @w.close unless @w.closed?
  end

  def test_nread
    assert_equal 0, @r.nread
    @w.syswrite "."
    sleep 0.1
    assert_equal 1, @r.nread
  end

  def test_nread_buffered
    @w.syswrite ".\n!"
    assert_equal ".\n", @r.read(2)
    assert_equal 1, @r.nread
  end

  def test_ready?
    refute @r.ready?, "shouldn't ready, but ready"
    @w.syswrite "."
    sleep 0.1
    assert @r.ready?, "should ready, but not"
  end

  def test_buffered_ready?
    @w.syswrite ".\n!"
    assert_equal ".\n", @r.gets
    assert @r.ready?
  end

  def test_wait
    assert_nil @r.wait(0)
    @w.syswrite "."
    sleep 0.1
    assert_equal @r, @r.wait(0)
  end

  def test_wait_buffered
    @w.syswrite ".\n!"
    assert_equal ".\n", @r.gets
    assert_equal true, @r.wait(0)
  end

  def test_wait_forever
    Thread.new { sleep 0.01; @w.syswrite "." }
    assert_equal @r, @r.wait
  end

  def test_wait_eof
    Thread.new { sleep 0.01; @w.close }
    assert_nil @r.wait
  end
end if IO.method_defined?(:wait)
