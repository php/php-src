require 'test/unit'
require 'tmpdir'

begin
  require 'sdbm'
rescue LoadError
end

class TestSDBM < Test::Unit::TestCase
  def setup
    @tmpdir = Dir.mktmpdir("tmptest_sdbm")
    @prefix = "tmptest_sdbm_#{$$}"
    @path = "#{@tmpdir}/#{@prefix}_"
    assert_instance_of(SDBM, @sdbm = SDBM.new(@path))
  end
  def teardown
    assert_nil(@sdbm.close)
    ObjectSpace.each_object(SDBM) do |obj|
      obj.close unless obj.closed?
    end
    FileUtils.remove_entry_secure @tmpdir
  end

  def check_size(expect, sdbm=@sdbm)
    assert_equal(expect, sdbm.size)
    n = 0
    sdbm.each { n+=1 }
    assert_equal(expect, n)
    if expect == 0
      assert_equal(true, sdbm.empty?)
    else
      assert_equal(false, sdbm.empty?)
    end
  end

  def have_fork?
    begin
      fork{}
      true
    rescue NotImplementedError
      false
    end
  end

  def test_version
    assert(! SDBM.const_defined?(:VERSION))
  end

  def test_s_new_has_no_block
    # SDBM.new ignore the block
    foo = true
    assert_instance_of(SDBM, sdbm = SDBM.new("#{@tmpdir}/#{@prefix}") { foo = false })
    assert_equal(foo, true)
    assert_nil(sdbm.close)
  end
  def test_s_open_no_create
    assert_nil(sdbm = SDBM.open("#{@tmpdir}/#{@prefix}", nil))
  ensure
    sdbm.close if sdbm
  end
  def test_s_open_with_block
    assert_equal(SDBM.open("#{@tmpdir}/#{@prefix}") { :foo }, :foo)
  end
=begin
  # Is it guaranteed on many OS?
  def test_s_open_lock_one_process
    # locking on one process
    assert_instance_of(SDBM, sdbm  = SDBM.open("#{@tmpdir}/#{@prefix}", 0644))
    assert_raise(Errno::EWOULDBLOCK) {
      begin
	SDBM.open("#{@tmpdir}/#{@prefix}", 0644)
      rescue Errno::EAGAIN
	raise Errno::EWOULDBLOCK
      end
    }
  end
=end

  def test_s_open_nolock
    # sdbm 1.8.0 specific
    if not defined? SDBM::NOLOCK
      return
    end
    return unless have_fork?	# snip this test

    pid = fork() {
      assert_instance_of(SDBM, sdbm  = SDBM.open("#{@tmpdir}/#{@prefix}", 0644,
						SDBM::NOLOCK))
      sleep 2
    }
    sleep 1
    begin
      sdbm2 = nil
      assert_no_exception(Errno::EWOULDBLOCK, Errno::EAGAIN, Errno::EACCES) {
	assert_instance_of(SDBM, sdbm2 = SDBM.open("#{@tmpdir}/#{@prefix}", 0644))
      }
    ensure
      Process.wait pid
      sdbm2.close if sdbm2
    end

    p Dir.glob("#{@tmpdir}/#{@prefix}*") if $DEBUG

    pid = fork() {
      assert_instance_of(SDBM, sdbm  = SDBM.open("#{@tmpdir}/#{@prefix}", 0644))
      sleep 2
    }
    begin
      sleep 1
      sdbm2 = nil
      assert_no_exception(Errno::EWOULDBLOCK, Errno::EAGAIN, Errno::EACCES) {
	# this test is failed on Cygwin98 (???)
	assert_instance_of(SDBM, sdbm2 = SDBM.open("#{@tmpdir}/#{@prefix}", 0644,
						   SDBM::NOLOCK))
      }
    ensure
      Process.wait pid
      sdbm2.close if sdbm2
    end
  end

  def test_s_open_error
    skip "doesn't support to avoid read access by owner on Windows" if /mswin|mingw/ =~ RUBY_PLATFORM
    assert_instance_of(SDBM, sdbm = SDBM.open("#{@tmpdir}/#{@prefix}", 0))
    assert_raise(Errno::EACCES) {
      SDBM.open("#{@tmpdir}/#{@prefix}", 0)
    }
    sdbm.close
  end

  def test_close
    assert_instance_of(SDBM, sdbm = SDBM.open("#{@tmpdir}/#{@prefix}"))
    assert_nil(sdbm.close)

    # closed SDBM file
    assert_raise(SDBMError) { sdbm.close }
  end

  def test_aref
    assert_equal('bar', @sdbm['foo'] = 'bar')
    assert_equal('bar', @sdbm['foo'])

    assert_nil(@sdbm['bar'])
  end

  def test_fetch
    assert_equal('bar', @sdbm['foo']='bar')
    assert_equal('bar', @sdbm.fetch('foo'))

    # key not found
    assert_raise(IndexError) {
      @sdbm.fetch('bar')
    }

    # test for `ifnone' arg
    assert_equal('baz', @sdbm.fetch('bar', 'baz'))

    # test for `ifnone' block
    assert_equal('foobar', @sdbm.fetch('bar') {|key| 'foo' + key })
  end

  def test_aset
    num = 0
    2.times {|i|
      assert_equal('foo', @sdbm['foo'] = 'foo')
      assert_equal('foo', @sdbm['foo'])
      assert_equal('bar', @sdbm['foo'] = 'bar')
      assert_equal('bar', @sdbm['foo'])

      num += 1 if i == 0
      assert_equal(num, @sdbm.size)

      # assign nil
      assert_equal('', @sdbm['bar'] = '')
      assert_equal('', @sdbm['bar'])

      num += 1 if i == 0
      assert_equal(num, @sdbm.size)

      # empty string
      assert_equal('', @sdbm[''] = '')
      assert_equal('', @sdbm[''])

      num += 1 if i == 0
      assert_equal(num, @sdbm.size)

      # Fixnum
      assert_equal('200', @sdbm['100'] = '200')
      assert_equal('200', @sdbm['100'])

      num += 1 if i == 0
      assert_equal(num, @sdbm.size)

      # Big key and value
      assert_equal('y' * 100, @sdbm['x' * 100] = 'y' * 100)
      assert_equal('y' * 100, @sdbm['x' * 100])

      num += 1 if i == 0
      assert_equal(num, @sdbm.size)
    }
  end

  def test_key
    assert_equal('bar', @sdbm['foo'] = 'bar')
    assert_equal('foo', @sdbm.key('bar'))
    assert_nil(@sdbm['bar'])
  end

  def test_values_at
    keys = %w(foo bar baz)
    values = %w(FOO BAR BAZ)
    @sdbm[keys[0]], @sdbm[keys[1]], @sdbm[keys[2]] = values
    assert_equal(values.reverse, @sdbm.values_at(*keys.reverse))
  end

  def test_select_with_block
    keys = %w(foo bar baz)
    values = %w(FOO BAR BAZ)
    @sdbm[keys[0]], @sdbm[keys[1]], @sdbm[keys[2]] = values
    ret = @sdbm.select {|k,v|
      assert_equal(k.upcase, v)
      k != "bar"
    }
    assert_equal([['baz', 'BAZ'], ['foo', 'FOO']],
		  ret.sort)
  end

  def test_length
    num = 10
    assert_equal(0, @sdbm.size)
    num.times {|i|
      i = i.to_s
      @sdbm[i] = i
    }
    assert_equal(num, @sdbm.size)

    @sdbm.shift

    assert_equal(num - 1, @sdbm.size)
  end

  def test_empty?
    assert_equal(true, @sdbm.empty?)
    @sdbm['foo'] = 'FOO'
    assert_equal(false, @sdbm.empty?)
  end

  def test_each_pair
    n = 0
    @sdbm.each_pair { n += 1 }
    assert_equal(0, n)

    keys = %w(foo bar baz)
    values = %w(FOO BAR BAZ)

    @sdbm[keys[0]], @sdbm[keys[1]], @sdbm[keys[2]] = values

    n = 0
    ret = @sdbm.each_pair {|key, val|
      assert_not_nil(i = keys.index(key))
      assert_equal(val, values[i])

      n += 1
    }
    assert_equal(keys.size, n)
    assert_equal(@sdbm, ret)
  end

  def test_each_value
    n = 0
    @sdbm.each_value { n += 1 }
    assert_equal(0, n)

    keys = %w(foo bar baz)
    values = %w(FOO BAR BAZ)

    @sdbm[keys[0]], @sdbm[keys[1]], @sdbm[keys[2]] = values

    n = 0
    ret = @sdbm.each_value {|val|
      assert_not_nil(key = @sdbm.key(val))
      assert_not_nil(i = keys.index(key))
      assert_equal(val, values[i])

      n += 1
    }
    assert_equal(keys.size, n)
    assert_equal(@sdbm, ret)
  end

  def test_each_key
    n = 0
    @sdbm.each_key { n += 1 }
    assert_equal(0, n)

    keys = %w(foo bar baz)
    values = %w(FOO BAR BAZ)

    @sdbm[keys[0]], @sdbm[keys[1]], @sdbm[keys[2]] = values

    n = 0
    ret = @sdbm.each_key {|key|
      assert_not_nil(i = keys.index(key))
      assert_equal(@sdbm[key], values[i])

      n += 1
    }
    assert_equal(keys.size, n)
    assert_equal(@sdbm, ret)
  end

  def test_keys
    assert_equal([], @sdbm.keys)

    keys = %w(foo bar baz)
    values = %w(FOO BAR BAZ)

    @sdbm[keys[0]], @sdbm[keys[1]], @sdbm[keys[2]] = values

    assert_equal(keys.sort, @sdbm.keys.sort)
    assert_equal(values.sort, @sdbm.values.sort)
  end

  def test_values
    test_keys
  end

  def test_shift
    assert_nil(@sdbm.shift)
    assert_equal(0, @sdbm.size)

    keys = %w(foo bar baz)
    values = %w(FOO BAR BAZ)

    @sdbm[keys[0]], @sdbm[keys[1]], @sdbm[keys[2]] = values

    ret_keys = []
    ret_values = []
    while ret = @sdbm.shift
      ret_keys.push ret[0]
      ret_values.push ret[1]

      assert_equal(keys.size - ret_keys.size, @sdbm.size)
    end

    assert_equal(keys.sort, ret_keys.sort)
    assert_equal(values.sort, ret_values.sort)
  end

  def test_delete
    keys = %w(foo bar baz)
    values = %w(FOO BAR BAZ)
    key = keys[1]

    assert_nil(@sdbm.delete(key))
    assert_equal(0, @sdbm.size)

    @sdbm[keys[0]], @sdbm[keys[1]], @sdbm[keys[2]] = values

    assert_equal('BAR', @sdbm.delete(key))
    assert_nil(@sdbm[key])
    assert_equal(2, @sdbm.size)

    assert_nil(@sdbm.delete(key))
  end
  def test_delete_with_block
    key = 'no called block'
    @sdbm[key] = 'foo'
    assert_equal('foo', @sdbm.delete(key) {|k| k.replace 'called block'; :blockval})
    assert_equal(0, @sdbm.size)

    key = 'no called block'
    assert_equal(:blockval, @sdbm.delete(key) {|k| k.replace 'called block'; :blockval})
    assert_equal(0, @sdbm.size)
  end

  def test_delete_if
    v = "0"
    100.times {@sdbm[v] = v; v = v.next}

    ret = @sdbm.delete_if {|key, val| key.to_i < 50}
    assert_equal(@sdbm, ret)
    check_size(50, @sdbm)

    ret = @sdbm.delete_if {|key, val| key.to_i >= 50}
    assert_equal(@sdbm, ret)
    check_size(0, @sdbm)

    # break
    v = "0"
    100.times {@sdbm[v] = v; v = v.next}
    check_size(100, @sdbm)
    n = 0;
    @sdbm.delete_if {|key, val|
      break if n > 50
      n+=1
      true
    }
    assert_equal(51, n)
    check_size(49, @sdbm)

    @sdbm.clear

    # raise
    v = "0"
    100.times {@sdbm[v] = v; v = v.next}
    check_size(100, @sdbm)
    n = 0;
    begin
      @sdbm.delete_if {|key, val|
	raise "runtime error" if n > 50
	n+=1
	true
      }
    rescue
    end
    assert_equal(51, n)
    check_size(49, @sdbm)
  end

  def test_reject
    v = "0"
    100.times {@sdbm[v] = v; v = v.next}

    hash = @sdbm.reject {|key, val| key.to_i < 50}
    assert_instance_of(Hash, hash)
    assert_equal(100, @sdbm.size)

    assert_equal(50, hash.size)
    hash.each_pair {|key,val|
      assert_equal(false, key.to_i < 50)
      assert_equal(key, val)
    }

    hash = @sdbm.reject {|key, val| key.to_i < 100}
    assert_instance_of(Hash, hash)
    assert_equal(true, hash.empty?)
  end

  def test_clear
    v = "1"
    100.times {v = v.next; @sdbm[v] = v}

    assert_equal(@sdbm, @sdbm.clear)

    # validate SDBM#size
    i = 0
    @sdbm.each { i += 1 }
    assert_equal(@sdbm.size, i)
    assert_equal(0, i)
  end

  def test_invert
    v = "0"
    100.times {@sdbm[v] = v; v = v.next}

    hash = @sdbm.invert
    assert_instance_of(Hash, hash)
    assert_equal(100, hash.size)
    hash.each_pair {|key, val|
      assert_equal(key.to_i, val.to_i)
    }
  end

  def test_update
    hash = {}
    v = "0"
    100.times {v = v.next; hash[v] = v}

    @sdbm["101"] = "101"
    @sdbm.update hash
    assert_equal(101, @sdbm.size)
    @sdbm.each_pair {|key, val|
      assert_equal(key.to_i, val.to_i)
    }
  end

  def test_replace
    hash = {}
    v = "0"
    100.times {v = v.next; hash[v] = v}

    @sdbm["101"] = "101"
    @sdbm.replace hash
    assert_equal(100, @sdbm.size)
    @sdbm.each_pair {|key, val|
      assert_equal(key.to_i, val.to_i)
    }
  end

  def test_haskey?
    assert_equal('bar', @sdbm['foo']='bar')
    assert_equal(true,  @sdbm.has_key?('foo'))
    assert_equal(false, @sdbm.has_key?('bar'))
  end

  def test_has_value?
    assert_equal('bar', @sdbm['foo']='bar')
    assert_equal(true,  @sdbm.has_value?('bar'))
    assert_equal(false, @sdbm.has_value?('foo'))
  end

  def test_to_a
    v = "0"
    100.times {v = v.next; @sdbm[v] = v}

    ary = @sdbm.to_a
    assert_instance_of(Array, ary)
    assert_equal(100, ary.size)
    ary.each {|key,val|
      assert_equal(key.to_i, val.to_i)
    }
  end

  def test_to_hash
    v = "0"
    100.times {v = v.next; @sdbm[v] = v}

    hash = @sdbm.to_hash
    assert_instance_of(Hash, hash)
    assert_equal(100, hash.size)
    hash.each {|key,val|
      assert_equal(key.to_i, val.to_i)
    }
  end

  def test_closed
    assert_equal(false, @sdbm.closed?)
    @sdbm.close
    assert_equal(true, @sdbm.closed?)
    @sdbm = SDBM.new(@path)
  end

  def test_readonly
    @sdbm["bar"] = "baz"
    @sdbm.close
    File.chmod(0444, @path + ".dir")
    File.chmod(0444, @path + ".pag")
    @sdbm = SDBM.new(@path)
    assert_raise(SDBMError) { @sdbm["bar"] = "foo" }
    assert_raise(SDBMError) { @sdbm.delete("bar") }
    assert_raise(SDBMError) { @sdbm.delete_if { true } }
    assert_raise(SDBMError) { @sdbm.clear }
    assert_nil(@sdbm.store("bar", nil))
  end

  def test_update2
    obj = Object.new
    def obj.each_pair
      yield []
    end
    assert_raise(ArgumentError) { @sdbm.update(obj) }
  end
end

