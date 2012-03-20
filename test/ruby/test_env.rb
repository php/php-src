require 'test/unit'

class TestEnv < Test::Unit::TestCase
  IGNORE_CASE = /bccwin|mswin|mingw/ =~ RUBY_PLATFORM
  PATH_ENV = "PATH"

  def setup
    @verbose = $VERBOSE
    $VERBOSE = nil
    @backup = ENV.to_hash
    ENV.delete('test')
    ENV.delete('TEST')
  end

  def teardown
    $VERBOSE = @verbose
    ENV.clear
    @backup.each {|k, v| ENV[k] = v }
  end

  def test_bracket
    assert_nil(ENV['test'])
    assert_nil(ENV['TEST'])
    ENV['test'] = 'foo'
    assert_equal('foo', ENV['test'])
    if IGNORE_CASE
      assert_equal('foo', ENV['TEST'])
    else
      assert_nil(ENV['TEST'])
    end
    ENV['TEST'] = 'bar'
    assert_equal('bar', ENV['TEST'])
    if IGNORE_CASE
      assert_equal('bar', ENV['test'])
    else
      assert_equal('foo', ENV['test'])
    end

    assert_raise(TypeError) {
      tmp = ENV[1]
    }
    assert_raise(TypeError) {
      ENV[1] = 'foo'
    }
    assert_raise(TypeError) {
      ENV['test'] = 0
    }
  end

  def test_has_value
    val = 'a'
    val.succ! while ENV.has_value?(val) || ENV.has_value?(val.upcase)
    ENV['test'] = val[0...-1]

    assert_equal(false, ENV.has_value?(val))
    assert_equal(false, ENV.has_value?(val.upcase))
    ENV['test'] = val
    assert_equal(true, ENV.has_value?(val))
    assert_equal(false, ENV.has_value?(val.upcase))
    ENV['test'] = val.upcase
    assert_equal(false, ENV.has_value?(val))
    assert_equal(true, ENV.has_value?(val.upcase))
  end

  def test_key
    val = 'a'
    val.succ! while ENV.has_value?(val) || ENV.has_value?(val.upcase)
    ENV['test'] = val[0...-1]

    assert_nil(ENV.key(val))
    assert_nil(ENV.index(val))
    assert_nil(ENV.key(val.upcase))
    ENV['test'] = val
    if IGNORE_CASE
      assert_equal('TEST', ENV.key(val).upcase)
    else
      assert_equal('test', ENV.key(val))
    end
    assert_nil(ENV.key(val.upcase))
    ENV['test'] = val.upcase
    assert_nil(ENV.key(val))
    if IGNORE_CASE
      assert_equal('TEST', ENV.key(val.upcase).upcase)
    else
      assert_equal('test', ENV.key(val.upcase))
    end
  end

  def test_delete
    assert_raise(ArgumentError) { ENV.delete("foo\0bar") }
    assert_nil(ENV.delete("TEST"))
    assert_nothing_raised { ENV.delete(PATH_ENV) }
  end

  def test_getenv
    assert_raise(ArgumentError) { ENV["foo\0bar"] }
    ENV[PATH_ENV] = ""
    assert_equal("", ENV[PATH_ENV])
    assert_nil(ENV[""])
  end

  def test_fetch
    ENV["test"] = "foo"
    assert_equal("foo", ENV.fetch("test"))
    ENV.delete("test")
    assert_raise(KeyError) { ENV.fetch("test") }
    assert_equal("foo", ENV.fetch("test", "foo"))
    assert_equal("bar", ENV.fetch("test") { "bar" })
    assert_equal("bar", ENV.fetch("test", "foo") { "bar" })
    assert_raise(ArgumentError) { ENV.fetch("foo\0bar") }
    assert_nothing_raised { ENV.fetch(PATH_ENV, "foo") }
    ENV[PATH_ENV] = ""
    assert_equal("", ENV.fetch(PATH_ENV))
  end

  def test_aset
    assert_raise(SecurityError) do
      Thread.new do
        $SAFE = 4
        ENV["test"] = "foo"
      end.join
    end
    assert_nothing_raised { ENV["test"] = nil }
    assert_equal(nil, ENV["test"])
    assert_raise(ArgumentError) { ENV["foo\0bar"] = "test" }
    assert_raise(ArgumentError) { ENV["test"] = "foo\0bar" }

    begin
      # setenv(3) allowed the name includes '=',
      # but POSIX.1-2001 says it should fail with EINVAL.
      # see also http://togetter.com/li/22380
      ENV["foo=bar"] = "test"
      assert_equal("test", ENV["foo=bar"])
      assert_equal("test", ENV["foo"])
    rescue Errno::EINVAL
    end

    ENV[PATH_ENV] = "/tmp/".taint
    assert_equal("/tmp/", ENV[PATH_ENV])
  end

  def test_keys
    a = nil
    assert_block { a = ENV.keys }
    assert_kind_of(Array, a)
    a.each {|k| assert_kind_of(String, k) }
  end

  def test_each_key
    ENV.each_key {|k| assert_kind_of(String, k) }
  end

  def test_values
    a = nil
    assert_block { a = ENV.values }
    assert_kind_of(Array, a)
    a.each {|k| assert_kind_of(String, k) }
  end

  def test_each_value
    ENV.each_value {|k| assert_kind_of(String, k) }
  end

  def test_each_pair
    ENV.each_pair do |k, v|
      assert_kind_of(String, k)
      assert_kind_of(String, v)
    end
  end

  def test_reject_bang
    h1 = {}
    ENV.each_pair {|k, v| h1[k] = v }
    ENV["test"] = "foo"
    ENV.reject! {|k, v| IGNORE_CASE ? k.upcase == "TEST" : k == "test" }
    h2 = {}
    ENV.each_pair {|k, v| h2[k] = v }
    assert_equal(h1, h2)

    h1 = {}
    ENV.each_pair {|k, v| h1[k] = v }
    ENV["test"] = "foo"
    ENV.delete_if {|k, v| IGNORE_CASE ? k.upcase == "TEST" : k == "test" }
    h2 = {}
    ENV.each_pair {|k, v| h2[k] = v }
    assert_equal(h1, h2)
  end

  def test_select_bang
    h1 = {}
    ENV.each_pair {|k, v| h1[k] = v }
    ENV["test"] = "foo"
    ENV.select! {|k, v| IGNORE_CASE ? k.upcase != "TEST" : k != "test" }
    h2 = {}
    ENV.each_pair {|k, v| h2[k] = v }
    assert_equal(h1, h2)

    h1 = {}
    ENV.each_pair {|k, v| h1[k] = v }
    ENV["test"] = "foo"
    ENV.keep_if {|k, v| IGNORE_CASE ? k.upcase != "TEST" : k != "test" }
    h2 = {}
    ENV.each_pair {|k, v| h2[k] = v }
    assert_equal(h1, h2)
  end

  def test_values_at
    ENV["test"] = "foo"
    assert_equal(["foo", "foo"], ENV.values_at("test", "test"))
  end

  def test_select
    ENV["test"] = "foo"
    h = ENV.select {|k| IGNORE_CASE ? k.upcase == "TEST" : k == "test" }
    assert_equal(1, h.size)
    k = h.keys.first
    v = h.values.first
    if IGNORE_CASE
      assert_equal("TEST", k.upcase)
      assert_equal("FOO", v.upcase)
    else
      assert_equal("test", k)
      assert_equal("foo", v)
    end
  end

  def test_clear
    ENV.clear
    assert_equal(0, ENV.size)
  end

  def test_to_s
    assert_equal("ENV", ENV.to_s)
  end

  def test_inspect
    ENV.clear
    ENV["foo"] = "bar"
    ENV["baz"] = "qux"
    s = ENV.inspect
    if IGNORE_CASE
      s = s.upcase
      assert(s == '{"FOO"=>"BAR", "BAZ"=>"QUX"}' || s == '{"BAZ"=>"QUX", "FOO"=>"BAR"}')
    else
      assert(s == '{"foo"=>"bar", "baz"=>"qux"}' || s == '{"baz"=>"qux", "foo"=>"bar"}')
    end
  end

  def test_to_a
    ENV.clear
    ENV["foo"] = "bar"
    ENV["baz"] = "qux"
    a = ENV.to_a
    assert_equal(2, a.size)
    if IGNORE_CASE
      a = a.map {|x| x.map {|y| y.upcase } }
      assert(a == [%w(FOO BAR), %w(BAZ QUX)] || a == [%w(BAZ QUX), %w(FOO BAR)])
    else
      assert(a == [%w(foo bar), %w(baz qux)] || a == [%w(baz qux), %w(foo bar)])
    end
  end

  def test_rehash
    assert_nil(ENV.rehash)
  end

  def test_size
    s = ENV.size
    ENV["test"] = "foo"
    assert_equal(s + 1, ENV.size)
  end

  def test_empty_p
    ENV.clear
    assert(ENV.empty?)
    ENV["test"] = "foo"
    assert(!ENV.empty?)
  end

  def test_has_key
    assert(!ENV.has_key?("test"))
    ENV["test"] = "foo"
    assert(ENV.has_key?("test"))
    assert_raise(ArgumentError) { ENV.has_key?("foo\0bar") }
  end

  def test_assoc
    assert_nil(ENV.assoc("test"))
    ENV["test"] = "foo"
    k, v = ENV.assoc("test")
    if IGNORE_CASE
      assert_equal("TEST", k.upcase)
      assert_equal("FOO", v.upcase)
    else
      assert_equal("test", k)
      assert_equal("foo", v)
    end
    assert_raise(ArgumentError) { ENV.assoc("foo\0bar") }
  end

  def test_has_value2
    ENV.clear
    assert(!ENV.has_value?("foo"))
    ENV["test"] = "foo"
    assert(ENV.has_value?("foo"))
  end

  def test_rassoc
    ENV.clear
    assert_nil(ENV.rassoc("foo"))
    ENV["foo"] = "bar"
    ENV["test"] = "foo"
    ENV["baz"] = "qux"
    k, v = ENV.rassoc("foo")
    if IGNORE_CASE
      assert_equal("TEST", k.upcase)
      assert_equal("FOO", v.upcase)
    else
      assert_equal("test", k)
      assert_equal("foo", v)
    end
  end

  def test_to_hash
    h = {}
    ENV.each {|k, v| h[k] = v }
    assert_equal(h, ENV.to_hash)
  end

  def test_reject
    h1 = {}
    ENV.each_pair {|k, v| h1[k] = v }
    ENV["test"] = "foo"
    h2 = ENV.reject {|k, v| IGNORE_CASE ? k.upcase == "TEST" : k == "test" }
    assert_equal(h1, h2)
  end

  def check(as, bs)
    if IGNORE_CASE
      as = as.map {|xs| xs.map {|x| x.upcase } }
      bs = bs.map {|xs| xs.map {|x| x.upcase } }
    end
    assert_equal(as.sort, bs.sort)
  end

  def test_shift
    ENV.clear
    ENV["foo"] = "bar"
    ENV["baz"] = "qux"
    a = ENV.shift
    b = ENV.shift
    check([a, b], [%w(foo bar), %w(baz qux)])
    assert_nil(ENV.shift)
  end

  def test_invert
    ENV.clear
    ENV["foo"] = "bar"
    ENV["baz"] = "qux"
    check(ENV.invert.to_a, [%w(bar foo), %w(qux baz)])
  end

  def test_replace
    ENV["foo"] = "xxx"
    ENV.replace({"foo"=>"bar", "baz"=>"qux"})
    check(ENV.to_hash.to_a, [%w(foo bar), %w(baz qux)])
  end

  def test_update
    ENV.clear
    ENV["foo"] = "bar"
    ENV["baz"] = "qux"
    ENV.update({"baz"=>"quux","a"=>"b"})
    check(ENV.to_hash.to_a, [%w(foo bar), %w(baz quux), %w(a b)])

    ENV.clear
    ENV["foo"] = "bar"
    ENV["baz"] = "qux"
    ENV.update({"baz"=>"quux","a"=>"b"}) {|k, v1, v2| v1 ? k + "_" + v1 + "_" + v2 : v2 }
    check(ENV.to_hash.to_a, [%w(foo bar), %w(baz baz_qux_quux), %w(a b)])
  end

  def test_huge_value
    huge_value = "bar" * 40960
    ENV["foo"] = "bar"
    if /mswin|mingw/ =~ RUBY_PLATFORM
      assert_raise(Errno::EINVAL) { ENV["foo"] = huge_value }
      assert_equal("bar", ENV["foo"])
    else
      assert_nothing_raised { ENV["foo"] = huge_value }
      assert_equal(huge_value, ENV["foo"])
    end
  end

  if /mswin|mingw/ =~ RUBY_PLATFORM
    def test_win32_blocksize
      len = 32767 - ENV.to_a.flatten.inject(0) {|r,e| r + e.size + 2 }
      val = "bar" * 1000
      key = nil
      1.upto(12) {|i|
        ENV[key] = val while (len -= val.size + (key="foo#{len}").size + 2) > 0
        assert_raise(Errno::EINVAL) { ENV[key] = val }
      }
    end
  end
end
