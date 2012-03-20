# -*- coding: UTF-8 -*-

require 'psych/helper'
require 'tmpdir'

begin
  require 'yaml/dbm'
rescue LoadError
end

module Psych
  ::Psych::DBM = ::YAML::DBM unless defined?(::Psych::DBM)

  class YAMLDBMTest < TestCase
    def setup
      @engine, YAML::ENGINE.yamler = YAML::ENGINE.yamler, 'psych'
      @dir = Dir.mktmpdir("rubytest-file")
      File.chown(-1, Process.gid, @dir)
      @yamldbm_file = make_tmp_filename("yamldbm")
      @yamldbm = YAML::DBM.new(@yamldbm_file)
    end

    def teardown
      YAML::ENGINE.yamler = @engine
      @yamldbm.clear
      @yamldbm.close
      FileUtils.remove_entry_secure @dir
    end

    def make_tmp_filename(prefix)
      @dir + "/" + prefix + File.basename(__FILE__) + ".#{$$}.test"
    end

    def test_store
      @yamldbm.store('a','b')
      @yamldbm.store('c','d')
      assert_equal 'b', @yamldbm['a']
      assert_equal 'd', @yamldbm['c']
      assert_nil @yamldbm['e']
    end

    def test_store_using_carret
      @yamldbm['a'] = 'b'
      @yamldbm['c'] = 'd'
      assert_equal 'b', @yamldbm['a']
      assert_equal 'd', @yamldbm['c']
      assert_nil @yamldbm['e']
    end

    def test_to_a
      @yamldbm['a'] = 'b'
      @yamldbm['c'] = 'd'
      assert_equal([['a','b'],['c','d']], @yamldbm.to_a.sort)
    end

    def test_to_hash
      @yamldbm['a'] = 'b'
      @yamldbm['c'] = 'd'
      assert_equal({'a'=>'b','c'=>'d'}, @yamldbm.to_hash)
    end

    def test_has_value?
      @yamldbm['a'] = 'b'
      @yamldbm['c'] = 'd'
      assert_equal true, @yamldbm.has_value?('b')
      assert_equal true, @yamldbm.has_value?('d')
      assert_equal false, @yamldbm.has_value?('f')
    end

    # Note:
    # YAML::DBM#index makes warning from internal of ::DBM#index.
    # It says 'DBM#index is deprecated; use DBM#key', but DBM#key
    # behaves not same as DBM#index.
    #
    # def test_index
    #  @yamldbm['a'] = 'b'
    #  @yamldbm['c'] = 'd'
    #  assert_equal 'a', @yamldbm.index('b')
    #  assert_equal 'c', @yamldbm.index('d')
    #  assert_nil @yamldbm.index('f')
    # end

    def test_key
      skip 'only on ruby 2.0.0' if RUBY_VERSION < '2.0.0'
      @yamldbm['a'] = 'b'
      @yamldbm['c'] = 'd'
      assert_equal 'a', @yamldbm.key('b')
      assert_equal 'c', @yamldbm.key('d')
      assert_nil @yamldbm.key('f')
    end

    def test_fetch
      assert_equal('bar', @yamldbm['foo']='bar')
      assert_equal('bar', @yamldbm.fetch('foo'))
      assert_nil @yamldbm.fetch('bar')
      assert_equal('baz', @yamldbm.fetch('bar', 'baz'))
      assert_equal('foobar', @yamldbm.fetch('bar') {|key| 'foo' + key })
    end

    def test_shift
      @yamldbm['a'] = 'b'
      @yamldbm['c'] = 'd'
      assert_equal([['a','b'], ['c','d']],
                   [@yamldbm.shift, @yamldbm.shift].sort)
      assert_nil @yamldbm.shift
    end

    def test_invert
      @yamldbm['a'] = 'b'
      @yamldbm['c'] = 'd'
      assert_equal({'b'=>'a','d'=>'c'}, @yamldbm.invert)
    end

    def test_update
      @yamldbm['a'] = 'b'
      @yamldbm['c'] = 'd'
      @yamldbm.update({'c'=>'d','e'=>'f'})
      assert_equal 'b', @yamldbm['a']
      assert_equal 'd', @yamldbm['c']
      assert_equal 'f', @yamldbm['e']
    end

    def test_replace
      @yamldbm['a'] = 'b'
      @yamldbm['c'] = 'd'
      @yamldbm.replace({'c'=>'d','e'=>'f'})
      assert_nil @yamldbm['a']
      assert_equal 'd', @yamldbm['c']
      assert_equal 'f', @yamldbm['e']
    end

    def test_delete
      @yamldbm['a'] = 'b'
      @yamldbm['c'] = 'd'
      assert_equal 'b', @yamldbm.delete('a')
      assert_nil @yamldbm['a']
      assert_equal 'd', @yamldbm['c']
      assert_nil @yamldbm.delete('e')
    end

    def test_delete_if
      @yamldbm['a'] = 'b'
      @yamldbm['c'] = 'd'
      @yamldbm['e'] = 'f'

      @yamldbm.delete_if {|k,v| k == 'a'}
      assert_nil @yamldbm['a']
      assert_equal 'd', @yamldbm['c']
      assert_equal 'f', @yamldbm['e']

      @yamldbm.delete_if {|k,v| v == 'd'}
      assert_nil @yamldbm['c']
      assert_equal 'f', @yamldbm['e']

      @yamldbm.delete_if {|k,v| false }
      assert_equal 'f', @yamldbm['e']
    end

    def test_reject
      @yamldbm['a'] = 'b'
      @yamldbm['c'] = 'd'
      @yamldbm['e'] = 'f'
      assert_equal({'c'=>'d','e'=>'f'}, @yamldbm.reject {|k,v| k == 'a'})
      assert_equal({'a'=>'b','e'=>'f'}, @yamldbm.reject {|k,v| v == 'd'})
      assert_equal({'a'=>'b','c'=>'d','e'=>'f'}, @yamldbm.reject {false})
    end

    def test_values
      assert_equal [], @yamldbm.values
      @yamldbm['a'] = 'b'
      @yamldbm['c'] = 'd'
      assert_equal ['b','d'], @yamldbm.values.sort
    end

    def test_values_at
      @yamldbm['a'] = 'b'
      @yamldbm['c'] = 'd'
      assert_equal ['b','d'], @yamldbm.values_at('a','c')
    end

    def test_selsct
      @yamldbm['a'] = 'b'
      @yamldbm['c'] = 'd'
      @yamldbm['e'] = 'f'
      assert_equal(['b','d'], @yamldbm.select('a','c'))
    end

    def test_selsct_with_block
      @yamldbm['a'] = 'b'
      @yamldbm['c'] = 'd'
      @yamldbm['e'] = 'f'
      assert_equal([['a','b']], @yamldbm.select {|k,v| k == 'a'})
      assert_equal([['c','d']], @yamldbm.select {|k,v| v == 'd'})
      assert_equal([], @yamldbm.select {false})
    end
  end
end if defined?(YAML::DBM) && defined?(Psych)
