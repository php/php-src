require 'test/unit'
require 'tmpdir'

begin
  require 'gdbm'
rescue LoadError
end

if defined? GDBM
  require 'tmpdir'
  require 'fileutils'

  class TestGDBM_RDONLY < Test::Unit::TestCase
    def TestGDBM_RDONLY.uname_s
      require 'rbconfig'
      case RbConfig::CONFIG['target_os']
      when 'cygwin'
        require 'Win32API'
        uname = Win32API.new('cygwin1', 'uname', 'P', 'I')
        utsname = ' ' * 100
        raise 'cannot get system name' if uname.call(utsname) == -1

        utsname.unpack('A20' * 5)[0]
      else
        RbConfig::CONFIG['target_os']
      end
    end
    SYSTEM = uname_s

    def setup
      @tmpdir = Dir.mktmpdir("tmptest_gdbm")
      @prefix = "tmptest_gdbm_#{$$}"
      @path = "#{@tmpdir}/#{@prefix}_"

      # prepare to make readonly GDBM file
      GDBM.open("#{@tmpdir}/#{@prefix}_rdonly", 0400) {|gdbm|
        gdbm['foo'] = 'FOO'
      }
      assert_instance_of(GDBM, @gdbm_rdonly = GDBM.new("#{@tmpdir}/#{@prefix}_rdonly", nil))
    end
    def teardown
      assert_nil(@gdbm_rdonly.close)
      ObjectSpace.each_object(GDBM) do |obj|
        obj.close unless obj.closed?
      end
      FileUtils.remove_entry_secure @tmpdir
    end

    def test_delete_rdonly
      if /^CYGWIN_9/ !~ SYSTEM
        assert_raise(GDBMError) {
          @gdbm_rdonly.delete("foo")
        }

        assert_nil(@gdbm_rdonly.delete("bar"))
      end
    end
  end

  class TestGDBM < Test::Unit::TestCase
    SYSTEM = TestGDBM_RDONLY::SYSTEM

    def setup
      @tmpdir = Dir.mktmpdir("tmptest_gdbm")
      @prefix = "tmptest_gdbm_#{$$}"
      @path = "#{@tmpdir}/#{@prefix}_"
      assert_instance_of(GDBM, @gdbm = GDBM.new(@path))
    end
    def teardown
      assert_nil(@gdbm.close)
      ObjectSpace.each_object(GDBM) do |obj|
        obj.close unless obj.closed?
      end
      FileUtils.remove_entry_secure @tmpdir
    end

    def check_size(expect, gdbm=@gdbm)
      assert_equal(expect, gdbm.size)
      n = 0
      gdbm.each { n+=1 }
      assert_equal(expect, n)
      if expect == 0
        assert_equal(true, gdbm.empty?)
      else
        assert_equal(false, gdbm.empty?)
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

    def test_s_new_has_no_block
      # GDBM.new ignore the block
      foo = true
      assert_instance_of(GDBM, gdbm = GDBM.new("#{@tmpdir}/#{@prefix}") { foo = false })
      assert_equal(foo, true)
      assert_nil(gdbm.close)
    end
    def test_s_open_create_new
      return if /^CYGWIN_9/ =~ SYSTEM

      save_mask = File.umask(0)
      begin
        assert_instance_of(GDBM, gdbm = GDBM.open("#{@tmpdir}/#{@prefix}"))
        gdbm.close
        assert_equal(File.stat("#{@tmpdir}/#{@prefix}").mode & 0777, 0666) unless /mswin|mingw/ =~ RUBY_PLATFORM
        assert_instance_of(GDBM, gdbm = GDBM.open("#{@tmpdir}/#{@prefix}2", 0644))
        gdbm.close
        assert_equal(File.stat("#{@tmpdir}/#{@prefix}2").mode & 0777, 0644)
      ensure
        File.umask save_mask
      end
    end
    def test_s_open_no_create
      skip "gdbm_open(GDBM_WRITER) is broken on libgdbm 1.8.0" if /1\.8\.0/ =~ GDBM::VERSION
      assert_nil(gdbm = GDBM.open("#{@tmpdir}/#{@prefix}", nil))
    ensure
      gdbm.close if gdbm
    end
    def test_s_open_3rd_arg
      assert_instance_of(GDBM, gdbm = GDBM.open("#{@tmpdir}/#{@prefix}", 0644,
                                                GDBM::FAST))
      gdbm.close

      # gdbm 1.8.0 specific
      if defined? GDBM::SYNC
        assert_instance_of(GDBM, gdbm = GDBM.open("#{@tmpdir}/#{@prefix}", 0644,
                                                  GDBM::SYNC))
        gdbm.close
      end
      # gdbm 1.8.0 specific
      if defined? GDBM::NOLOCK
        assert_instance_of(GDBM, gdbm = GDBM.open("#{@tmpdir}/#{@prefix}", 0644,
                                                  GDBM::NOLOCK))
        gdbm.close
      end
    end
    def test_s_open_with_block
      assert_equal(GDBM.open("#{@tmpdir}/#{@prefix}") { :foo }, :foo)
    end
    def test_s_open_lock
      return unless have_fork?	# snip this test
      pid = fork() {
        assert_instance_of(GDBM, gdbm  = GDBM.open("#{@tmpdir}/#{@prefix}", 0644))
        sleep 2
      }
      begin
        sleep 1
        assert_raise(Errno::EWOULDBLOCK) {
          begin
            assert_instance_of(GDBM, gdbm2 = GDBM.open("#{@tmpdir}/#{@prefix}", 0644))
          rescue Errno::EAGAIN, Errno::EACCES
            raise Errno::EWOULDBLOCK
          end
        }
      ensure
        Process.wait pid
      end
    end

=begin
    # Is it guaranteed on many OS?
    def test_s_open_lock_one_process
      # locking on one process
      assert_instance_of(GDBM, gdbm  = GDBM.open("#{@tmpdir}/#{@prefix}", 0644))
      assert_raise(Errno::EWOULDBLOCK) {
        begin
          GDBM.open("#{@tmpdir}/#{@prefix}", 0644)
        rescue Errno::EAGAIN
          raise Errno::EWOULDBLOCK
        end
      }
    end
=end

    def test_s_open_nolock
      # gdbm 1.8.0 specific
      if not defined? GDBM::NOLOCK
        return
      end
      return unless have_fork?	# snip this test

      pid = fork() {
        assert_instance_of(GDBM, gdbm  = GDBM.open("#{@tmpdir}/#{@prefix}", 0644,
                                                  GDBM::NOLOCK))
        sleep 2
      }
      sleep 1
      begin
        gdbm2 = nil
        assert_nothing_raised(Errno::EWOULDBLOCK, Errno::EAGAIN, Errno::EACCES) {
          assert_instance_of(GDBM, gdbm2 = GDBM.open("#{@tmpdir}/#{@prefix}", 0644))
        }
      ensure
        Process.wait pid
        gdbm2.close if gdbm2
      end

      p Dir.glob("#{@tmpdir}/#{@prefix}*") if $DEBUG

      pid = fork() {
        assert_instance_of(GDBM, gdbm  = GDBM.open("#{@tmpdir}/#{@prefix}", 0644))
        sleep 2
      }
      begin
        sleep 1
        gdbm2 = nil
        assert_nothing_raised(Errno::EWOULDBLOCK, Errno::EAGAIN, Errno::EACCES) {
          # this test is failed on Cygwin98 (???)
          assert_instance_of(GDBM, gdbm2 = GDBM.open("#{@tmpdir}/#{@prefix}", 0644,
                                                     GDBM::NOLOCK))
        }
      ensure
        Process.wait pid
        gdbm2.close if gdbm2
      end
    end

    def test_s_open_error
      assert_instance_of(GDBM, gdbm = GDBM.open("#{@tmpdir}/#{@prefix}", 0))
      assert_raise(Errno::EACCES, Errno::EWOULDBLOCK) {
        GDBM.open("#{@tmpdir}/#{@prefix}", 0)
      }
      gdbm.close
    end

    def test_close
      assert_instance_of(GDBM, gdbm = GDBM.open("#{@tmpdir}/#{@prefix}"))
      assert_nil(gdbm.close)

      # closed GDBM file
      assert_raise(RuntimeError) { gdbm.close }
    end

    def test_aref
      assert_equal('bar', @gdbm['foo'] = 'bar')
      assert_equal('bar', @gdbm['foo'])

      assert_nil(@gdbm['bar'])
    end

    def test_fetch
      assert_equal('bar', @gdbm['foo']='bar')
      assert_equal('bar', @gdbm.fetch('foo'))

      # key not found
      assert_raise(IndexError) {
        @gdbm.fetch('bar')
      }

      # test for `ifnone' arg
      assert_equal('baz', @gdbm.fetch('bar', 'baz'))

      # test for `ifnone' block
      assert_equal('foobar', @gdbm.fetch('bar') {|key| 'foo' + key })
    end

    def test_aset
      num = 0
      2.times {|i|
        assert_equal('foo', @gdbm['foo'] = 'foo')
        assert_equal('foo', @gdbm['foo'])
        assert_equal('bar', @gdbm['foo'] = 'bar')
        assert_equal('bar', @gdbm['foo'])

        num += 1 if i == 0
        assert_equal(num, @gdbm.size)

        # assign nil
        assert_equal('', @gdbm['bar'] = '')
        assert_equal('', @gdbm['bar'])

        num += 1 if i == 0
        assert_equal(num, @gdbm.size)

        # empty string
        assert_equal('', @gdbm[''] = '')
        assert_equal('', @gdbm[''])

        num += 1 if i == 0
        assert_equal(num, @gdbm.size)

        # Fixnum
        assert_equal('200', @gdbm['100'] = '200')
        assert_equal('200', @gdbm['100'])

        num += 1 if i == 0
        assert_equal(num, @gdbm.size)

        # Big key and value
        assert_equal('y' * 100, @gdbm['x' * 100] = 'y' * 100)
        assert_equal('y' * 100, @gdbm['x' * 100])

        num += 1 if i == 0
        assert_equal(num, @gdbm.size)
      }
    end

    def test_key
      assert_equal('bar', @gdbm['foo'] = 'bar')
      assert_equal('foo', @gdbm.key('bar'))
      assert_nil(@gdbm['bar'])
    end

    def test_values_at
      keys = %w(foo bar baz)
      values = %w(FOO BAR BAZ)
      @gdbm[keys[0]], @gdbm[keys[1]], @gdbm[keys[2]] = values
      assert_equal(values.reverse, @gdbm.values_at(*keys.reverse))
    end

    def test_select_with_block
      keys = %w(foo bar baz)
      values = %w(FOO BAR BAZ)
      @gdbm[keys[0]], @gdbm[keys[1]], @gdbm[keys[2]] = values
      ret = @gdbm.select {|k,v|
        assert_equal(k.upcase, v)
        k != "bar"
      }
      assert_equal([['baz', 'BAZ'], ['foo', 'FOO']],
                    ret.sort)
    end

    def test_length
      num = 10
      assert_equal(0, @gdbm.size)
      num.times {|i|
        i = i.to_s
        @gdbm[i] = i
      }
      assert_equal(num, @gdbm.size)

      @gdbm.shift

      assert_equal(num - 1, @gdbm.size)
    end

    def test_empty?
      assert_equal(true, @gdbm.empty?)
      @gdbm['foo'] = 'FOO'
      assert_equal(false, @gdbm.empty?)
    end

    def test_each_pair
      n = 0
      @gdbm.each_pair { n += 1 }
      assert_equal(0, n)

      keys = %w(foo bar baz)
      values = %w(FOO BAR BAZ)

      @gdbm[keys[0]], @gdbm[keys[1]], @gdbm[keys[2]] = values

      n = 0
      ret = @gdbm.each_pair {|key, val|
        assert_not_nil(i = keys.index(key))
        assert_equal(val, values[i])

        n += 1
      }
      assert_equal(keys.size, n)
      assert_equal(@gdbm, ret)
    end

    def test_each_value
      n = 0
      @gdbm.each_value { n += 1 }
      assert_equal(0, n)

      keys = %w(foo bar baz)
      values = %w(FOO BAR BAZ)

      @gdbm[keys[0]], @gdbm[keys[1]], @gdbm[keys[2]] = values

      n = 0
      ret = @gdbm.each_value {|val|
        assert_not_nil(key = @gdbm.key(val))
        assert_not_nil(i = keys.index(key))
        assert_equal(val, values[i])

        n += 1
      }
      assert_equal(keys.size, n)
      assert_equal(@gdbm, ret)
    end

    def test_each_key
      n = 0
      @gdbm.each_key { n += 1 }
      assert_equal(0, n)

      keys = %w(foo bar baz)
      values = %w(FOO BAR BAZ)

      @gdbm[keys[0]], @gdbm[keys[1]], @gdbm[keys[2]] = values

      n = 0
      ret = @gdbm.each_key {|key|
        assert_not_nil(i = keys.index(key))
        assert_equal(@gdbm[key], values[i])

        n += 1
      }
      assert_equal(keys.size, n)
      assert_equal(@gdbm, ret)
    end

    def test_keys
      assert_equal([], @gdbm.keys)

      keys = %w(foo bar baz)
      values = %w(FOO BAR BAZ)

      @gdbm[keys[0]], @gdbm[keys[1]], @gdbm[keys[2]] = values

      assert_equal(keys.sort, @gdbm.keys.sort)
      assert_equal(values.sort, @gdbm.values.sort)
    end

    def test_values
      test_keys
    end

    def test_shift
      assert_nil(@gdbm.shift)
      assert_equal(0, @gdbm.size)

      keys = %w(foo bar baz)
      values = %w(FOO BAR BAZ)

      @gdbm[keys[0]], @gdbm[keys[1]], @gdbm[keys[2]] = values

      ret_keys = []
      ret_values = []
      while ret = @gdbm.shift
        ret_keys.push ret[0]
        ret_values.push ret[1]

        assert_equal(keys.size - ret_keys.size, @gdbm.size)
      end

      assert_equal(keys.sort, ret_keys.sort)
      assert_equal(values.sort, ret_values.sort)
    end

    def test_delete
      keys = %w(foo bar baz)
      values = %w(FOO BAR BAZ)
      key = keys[1]

      assert_nil(@gdbm.delete(key))
      assert_equal(0, @gdbm.size)

      @gdbm[keys[0]], @gdbm[keys[1]], @gdbm[keys[2]] = values

      assert_equal('BAR', @gdbm.delete(key))
      assert_nil(@gdbm[key])
      assert_equal(2, @gdbm.size)

      assert_nil(@gdbm.delete(key))
    end

    def test_delete_with_block
      key = 'no called block'
      @gdbm[key] = 'foo'
      assert_equal('foo', @gdbm.delete(key) {|k| k.replace 'called block'})
      assert_equal('no called block', key)
      assert_equal(0, @gdbm.size)

      key = 'no called block'
      assert_equal(:blockval,
                    @gdbm.delete(key) {|k| k.replace 'called block'; :blockval})
      assert_equal('called block', key)
      assert_equal(0, @gdbm.size)
    end

    def test_delete_if
      v = "0"
      100.times {@gdbm[v] = v; v = v.next}

      ret = @gdbm.delete_if {|key, val| key.to_i < 50}
      assert_equal(@gdbm, ret)
      check_size(50, @gdbm)

      ret = @gdbm.delete_if {|key, val| key.to_i >= 50}
      assert_equal(@gdbm, ret)
      check_size(0, @gdbm)

      # break
      v = "0"
      100.times {@gdbm[v] = v; v = v.next}
      check_size(100, @gdbm)
      n = 0;
      @gdbm.delete_if {|key, val|
        break if n > 50
        n+=1
        true
      }
      assert_equal(51, n)
      check_size(49, @gdbm)

      @gdbm.clear

      # raise
      v = "0"
      100.times {@gdbm[v] = v; v = v.next}
      check_size(100, @gdbm)
      n = 0;
      begin
        @gdbm.delete_if {|key, val|
          raise "runtime error" if n > 50
          n+=1
          true
        }
      rescue
      end
      assert_equal(51, n)
      check_size(49, @gdbm)
    end

    def test_reject
      v = "0"
      100.times {@gdbm[v] = v; v = v.next}

      hash = @gdbm.reject {|key, val| key.to_i < 50}
      assert_instance_of(Hash, hash)
      assert_equal(100, @gdbm.size)

      assert_equal(50, hash.size)
      hash.each_pair {|key,val|
        assert_equal(false, key.to_i < 50)
        assert_equal(key, val)
      }

      hash = @gdbm.reject {|key, val| key.to_i < 100}
      assert_instance_of(Hash, hash)
      assert_equal(true, hash.empty?)
    end

    def test_clear
      v = "1"
      100.times {v = v.next; @gdbm[v] = v}

      assert_equal(@gdbm, @gdbm.clear)

      # validate GDBM#size
      i = 0
      @gdbm.each { i += 1 }
      assert_equal(@gdbm.size, i)
      assert_equal(0, i)
    end

    def test_invert
      v = "0"
      100.times {@gdbm[v] = v; v = v.next}

      hash = @gdbm.invert
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

      @gdbm["101"] = "101"
      @gdbm.update hash
      assert_equal(101, @gdbm.size)
      @gdbm.each_pair {|key, val|
        assert_equal(key.to_i, val.to_i)
      }
    end

    def test_replace
      hash = {}
      v = "0"
      100.times {v = v.next; hash[v] = v}

      @gdbm["101"] = "101"
      @gdbm.replace hash
      assert_equal(100, @gdbm.size)
      @gdbm.each_pair {|key, val|
        assert_equal(key.to_i, val.to_i)
      }
    end

    def test_reorganize
      size1 = File.size(@path)
      i = "1"
      1000.times {i = i.next; @gdbm[i] = i}
      @gdbm.clear
      @gdbm.sync

      size2 = File.size(@path)
      @gdbm.reorganize
      size3 = File.size(@path)

      # p [size1, size2, size3]
      assert_equal(true, size1 < size2)
      # this test is failed on Cygwin98. `GDBM version 1.8.0, as of May 19, 1999'
      assert_equal(true, size3 < size2)
      assert_equal(size1, size3)
    end

    def test_sync
      assert_instance_of(GDBM, gdbm = GDBM.open("#{@tmpdir}/#{@prefix}", 0666, GDBM::FAST))
      assert_equal(gdbm.sync, gdbm)
      gdbm.close
      assert_instance_of(GDBM, gdbm = GDBM.open("#{@tmpdir}/#{@prefix}", 0666))
      assert_equal(gdbm.sync, gdbm)
      gdbm.close
    end

    def test_cachesize=
        assert_equal(@gdbm.cachesize = 1024, 1024)
    end

    def test_fastmode=
        assert_equal(@gdbm.fastmode = true, true)
    end

    def test_syncmode=
        assert_equal(@gdbm.syncmode = true, true)
    end

    def test_haskey?
      assert_equal('bar', @gdbm['foo']='bar')
      assert_equal(true,  @gdbm.has_key?('foo'))
      assert_equal(false, @gdbm.has_key?('bar'))
    end

    def test_has_value?
      assert_equal('bar', @gdbm['foo']='bar')
      assert_equal(true,  @gdbm.has_value?('bar'))
      assert_equal(false, @gdbm.has_value?('foo'))
    end

    def test_to_a
      v = "0"
      100.times {v = v.next; @gdbm[v] = v}

      ary = @gdbm.to_a
      assert_instance_of(Array, ary)
      assert_equal(100, ary.size)
      ary.each {|key,val|
        assert_equal(key.to_i, val.to_i)
      }
    end

    def test_to_hash
      v = "0"
      100.times {v = v.next; @gdbm[v] = v}

      hash = @gdbm.to_hash
      assert_instance_of(Hash, hash)
      assert_equal(100, hash.size)
      hash.each {|key,val|
        assert_equal(key.to_i, val.to_i)
      }
    end
  end

  class TestGDBM2 < Test::Unit::TestCase
    def setup
      @tmproot = Dir.mktmpdir('ruby-gdbm')
    end

    def teardown
      FileUtils.remove_entry_secure @tmproot if File.directory?(@tmproot)
    end

    def test_reader_open_notexist
      assert_raise(Errno::ENOENT) {
        GDBM.open("#{@tmproot}/a", 0666, GDBM::READER)
      }
    end

    def test_writer_open_notexist
      skip "gdbm_open(GDBM_WRITER) is broken on libgdbm 1.8.0" if /1\.8\.0/ =~ GDBM::VERSION

      assert_raise(Errno::ENOENT) {
        GDBM.open("#{@tmproot}/a", 0666, GDBM::WRITER)
      }
    end

    def test_wrcreat_open_notexist
      v = GDBM.open("#{@tmproot}/a", 0666, GDBM::WRCREAT)
      assert_instance_of(GDBM, v)
      v.close
    end

    def test_newdb_open_notexist
      v = GDBM.open("#{@tmproot}/a", 0666, GDBM::NEWDB)
      assert_instance_of(GDBM, v)
      v.close
    end

    def test_reader_open
      GDBM.open("#{@tmproot}/a.dbm") {} # create a db.
      v = GDBM.open("#{@tmproot}/a.dbm", nil, GDBM::READER) {|d|
        assert_raise(GDBMError) { d["k"] = "v" }
        true
      }
      assert(v)
    end

    def test_newdb_open
      GDBM.open("#{@tmproot}/a.dbm") {|dbm|
        dbm["k"] = "v"
      }
      v = GDBM.open("#{@tmproot}/a.dbm", nil, GDBM::NEWDB) {|d|
        assert_equal(0, d.length)
        assert_nil(d["k"])
        true
      }
      assert(v)
    end

    def test_freeze
      GDBM.open("#{@tmproot}/a.dbm") {|d|
        d.freeze
        assert_raise(RuntimeError) { d["k"] = "v" }
      }
    end
  end
end
