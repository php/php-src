require 'test/unit'

class TestWaitForSingleFD < Test::Unit::TestCase
  require '-test-/wait_for_single_fd/wait_for_single_fd'

  def with_pipe
    r, w = IO.pipe
    begin
      yield r, w
    ensure
      r.close unless r.closed?
      w.close unless w.closed?
    end
  end

  def test_wait_for_valid_fd
    with_pipe do |r,w|
      rc = IO.wait_for_single_fd(w.fileno, RB_WAITFD_OUT, nil)
      assert_equal RB_WAITFD_OUT, rc
    end
  end

  def test_wait_for_invalid_fd
    # FreeBSD 8.2 or prior sticks this
    # http://bugs.ruby-lang.org/issues/5524
    skip if /freebsd[1-8]/ =~ RUBY_PLATFORM
    with_pipe do |r,w|
      wfd = w.fileno
      w.close
      assert_raises(Errno::EBADF) do
        IO.wait_for_single_fd(wfd, RB_WAITFD_OUT, nil)
      end
    end
  end

  def test_wait_for_closed_pipe
    with_pipe do |r,w|
      w.close
      rc = IO.wait_for_single_fd(r.fileno, RB_WAITFD_IN, nil)
      assert_equal RB_WAITFD_IN, rc
    end
  end


end
