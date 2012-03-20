require 'test/unit'

class TestOldThreadSelect < Test::Unit::TestCase
  require '-test-/old_thread_select/old_thread_select'

  ANCIENT_LINUX = RUBY_PLATFORM =~ /linux/ && `uname -r`.chomp < '2.6.32'
  DARWIN_10     = RUBY_PLATFORM =~ /darwin10/

  def with_pipe
    r, w = IO.pipe
    begin
      yield r, w
    ensure
      r.close unless r.closed?
      w.close unless w.closed?
    end
  end

  def test_old_select_read_timeout
    with_pipe do |r, w|
      t0 = Time.now
      rc = IO.old_thread_select([r.fileno], nil, nil, 0.001)
      diff = Time.now - t0
      assert_equal 0, rc
      assert_operator diff, :>=, 0.001, "returned too early: diff=#{diff}"
    end
  end unless ANCIENT_LINUX

  def test_old_select_error_timeout
    bug5299 = '[ruby-core:39380]'
    with_pipe do |r, w|
      t0 = Time.now
      rc = IO.old_thread_select(nil, nil, [r.fileno], 0.001)
      diff = Time.now - t0
      assert_equal 0, rc, bug5299
      assert_operator diff, :>=, 0.001, "returned too early: diff=#{diff}"
    end
  end unless ANCIENT_LINUX

  def test_old_select_false_positive
    bug5306 = '[ruby-core:39435]'
    with_pipe do |r2, w2|
      with_pipe do |r, w|
        t0 = Time.now
        w.syswrite '.'
        rfds = [ r.fileno, r2.fileno ]
        rc = IO.old_thread_select(rfds, nil, nil, nil)
        assert_equal [ r.fileno ], rfds, bug5306
        assert_equal 1, rc, bug5306
      end
    end
  end

  def test_old_select_read_write_check
    with_pipe do |r, w|
      w.syswrite('.')
      rc = IO.old_thread_select([r.fileno], nil, nil, nil)
      assert_equal 1, rc

      rc = IO.old_thread_select([r.fileno], [w.fileno], nil, nil)
      assert_equal 2, rc

      assert_equal '.', r.read(1)

      rc = IO.old_thread_select([r.fileno], [w.fileno], nil, nil)
      assert_equal 1, rc
    end
  end

  def test_old_select_signal_safe
    return unless Process.respond_to?(:kill)
    received = false
    trap(:INT) { received = true }
    main = Thread.current
    thr = Thread.new do
      Thread.pass until main.stop?
      Process.kill(:INT, $$)
      true
    end

    rc = nil
    diff = nil
    with_pipe do |r,w|
      assert_nothing_raised do
        t0 = Time.now
        rc = IO.old_thread_select([r.fileno], nil, nil, 1)
        diff = Time.now - t0
      end
    end

    unless ANCIENT_LINUX || DARWIN_10
      assert_operator diff, :>=, 1, "interrupted or short wait: diff=#{diff}"
    end
    assert_equal 0, rc
    assert_equal true, thr.value
    assert_not_equal false, received, "SIGINT not received"
  ensure
    trap(:INT, "DEFAULT")
  end
end
