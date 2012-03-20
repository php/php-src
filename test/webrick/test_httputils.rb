require "test/unit"
require "webrick/httputils"

class TestWEBrickHTTPUtils < Test::Unit::TestCase
  include WEBrick::HTTPUtils

  def test_normilize_path
    assert_equal("/foo",       normalize_path("/foo"))
    assert_equal("/foo/bar/",  normalize_path("/foo/bar/"))

    assert_equal("/",          normalize_path("/foo/../"))
    assert_equal("/",          normalize_path("/foo/.."))
    assert_equal("/",          normalize_path("/foo/bar/../../"))
    assert_equal("/",          normalize_path("/foo/bar/../.."))
    assert_equal("/",          normalize_path("/foo/bar/../.."))
    assert_equal("/baz",       normalize_path("/foo/bar/../../baz"))
    assert_equal("/baz",       normalize_path("/foo/../bar/../baz"))
    assert_equal("/baz/",      normalize_path("/foo/../bar/../baz/"))
    assert_equal("/...",       normalize_path("/bar/../..."))
    assert_equal("/.../",      normalize_path("/bar/../.../"))

    assert_equal("/foo/",      normalize_path("/foo/./"))
    assert_equal("/foo/",      normalize_path("/foo/."))
    assert_equal("/foo/",      normalize_path("/foo/././"))
    assert_equal("/foo/",      normalize_path("/foo/./."))
    assert_equal("/foo/bar",   normalize_path("/foo/./bar"))
    assert_equal("/foo/bar/",  normalize_path("/foo/./bar/."))
    assert_equal("/foo/bar/",  normalize_path("/./././foo/./bar/."))

    assert_equal("/foo/bar/",  normalize_path("//foo///.//bar/.///.//"))
    assert_equal("/",          normalize_path("//foo///..///bar/.///..//.//"))

    assert_raise(RuntimeError){ normalize_path("foo/bar") }
    assert_raise(RuntimeError){ normalize_path("..") }
    assert_raise(RuntimeError){ normalize_path("/..") }
    assert_raise(RuntimeError){ normalize_path("/./..") }
    assert_raise(RuntimeError){ normalize_path("/./../") }
    assert_raise(RuntimeError){ normalize_path("/./../..") }
    assert_raise(RuntimeError){ normalize_path("/./../../") }
    assert_raise(RuntimeError){ normalize_path("/./../") }
    assert_raise(RuntimeError){ normalize_path("/../..") }
    assert_raise(RuntimeError){ normalize_path("/../../") }
    assert_raise(RuntimeError){ normalize_path("/../../..") }
    assert_raise(RuntimeError){ normalize_path("/../../../") }
    assert_raise(RuntimeError){ normalize_path("/../foo/../") }
    assert_raise(RuntimeError){ normalize_path("/../foo/../../") }
    assert_raise(RuntimeError){ normalize_path("/foo/bar/../../../../") }
    assert_raise(RuntimeError){ normalize_path("/foo/../bar/../../") }
    assert_raise(RuntimeError){ normalize_path("/./../bar/") }
    assert_raise(RuntimeError){ normalize_path("/./../") }
  end

  def test_split_header_value
    assert_equal(['foo', 'bar'], split_header_value('foo, bar'))
    assert_equal(['"foo"', 'bar'], split_header_value('"foo", bar'))
    assert_equal(['foo', '"bar"'], split_header_value('foo, "bar"'))
    assert_equal(['*'], split_header_value('*'))
    assert_equal(['W/"xyzzy"', 'W/"r2d2xxxx"', 'W/"c3piozzzz"'],
                 split_header_value('W/"xyzzy", W/"r2d2xxxx", W/"c3piozzzz"'))
  end

  def test_escape
    assert_equal("/foo/bar", escape("/foo/bar"))
    assert_equal("/~foo/bar", escape("/~foo/bar"))
    assert_equal("/~foo%20bar", escape("/~foo bar"))
    assert_equal("/~foo%20bar", escape("/~foo bar"))
    assert_equal("/~foo%09bar", escape("/~foo\tbar"))
    assert_equal("/~foo+bar", escape("/~foo+bar"))
  end

  def test_escape_form
    assert_equal("%2Ffoo%2Fbar", escape_form("/foo/bar"))
    assert_equal("%2F~foo%2Fbar", escape_form("/~foo/bar"))
    assert_equal("%2F~foo+bar", escape_form("/~foo bar"))
    assert_equal("%2F~foo+%2B+bar", escape_form("/~foo + bar"))
  end

  def test_unescape
    assert_equal("/foo/bar", unescape("%2ffoo%2fbar"))
    assert_equal("/~foo/bar", unescape("/%7efoo/bar"))
    assert_equal("/~foo/bar", unescape("%2f%7efoo%2fbar"))
    assert_equal("/~foo+bar", unescape("/%7efoo+bar"))
  end

  def test_unescape_form
    assert_equal("//foo/bar", unescape_form("/%2Ffoo/bar"))
    assert_equal("//foo/bar baz", unescape_form("/%2Ffoo/bar+baz"))
    assert_equal("/~foo/bar baz", unescape_form("/%7Efoo/bar+baz"))
  end

  def test_escape_path
    assert_equal("/foo/bar", escape_path("/foo/bar"))
    assert_equal("/foo/bar/", escape_path("/foo/bar/"))
    assert_equal("/%25foo/bar/", escape_path("/%foo/bar/"))
  end
end
