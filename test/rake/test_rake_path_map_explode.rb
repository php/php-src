require File.expand_path('../helper', __FILE__)

class TestRakePathMapExplode < Rake::TestCase
  def setup
    super

    String.class_eval { public :pathmap_explode }
  end

  def teardown
    String.class_eval { protected :pathmap_explode }

    super
  end

  def test_explode
    assert_equal ['a'], 'a'.pathmap_explode
    assert_equal ['a', 'b'], 'a/b'.pathmap_explode
    assert_equal ['a', 'b', 'c'], 'a/b/c'.pathmap_explode
    assert_equal ['/', 'a'], '/a'.pathmap_explode
    assert_equal ['/', 'a', 'b'], '/a/b'.pathmap_explode
    assert_equal ['/', 'a', 'b', 'c'], '/a/b/c'.pathmap_explode

    if File::ALT_SEPARATOR
      assert_equal ['c:.', 'a'], 'c:a'.pathmap_explode
      assert_equal ['c:.', 'a', 'b'], 'c:a/b'.pathmap_explode
      assert_equal ['c:.', 'a', 'b', 'c'], 'c:a/b/c'.pathmap_explode
      assert_equal ['c:/', 'a'], 'c:/a'.pathmap_explode
      assert_equal ['c:/', 'a', 'b'], 'c:/a/b'.pathmap_explode
      assert_equal ['c:/', 'a', 'b', 'c'], 'c:/a/b/c'.pathmap_explode
    end
  end
end

