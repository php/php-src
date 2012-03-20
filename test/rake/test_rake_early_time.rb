require File.expand_path('../helper', __FILE__)

class TestRakeEarlyTime < Rake::TestCase
  def test_create
    early = Rake::EarlyTime.instance
    assert early <= Time.now
    assert early < Time.now
    assert early != Time.now
    assert Time.now > early
    assert Time.now >= early
    assert Time.now != early
  end

  def test_equality
    early = Rake::EarlyTime.instance
    assert_equal early, early, "two early times should be equal"
  end

  def test_original_time_compare_is_not_messed_up
    t1 = Time.mktime(1970, 1, 1, 0, 0, 0)
    t2 = Time.now
    assert t1 < t2
    assert t2 > t1
    assert t1 == t1
    assert t2 == t2
  end

  def test_to_s
    assert_equal "<EARLY TIME>", Rake::EARLY.to_s
  end
end
