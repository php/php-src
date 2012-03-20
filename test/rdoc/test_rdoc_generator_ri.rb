require 'rubygems'
require 'minitest/autorun'
require 'rdoc/rdoc'
require 'rdoc/generator/ri'
require 'tmpdir'
require 'fileutils'

class TestRDocGeneratorRI < MiniTest::Unit::TestCase

  def setup
    @options = RDoc::Options.new
    @options.encoding = Encoding::UTF_8 if Object.const_defined? :Encoding

    @pwd = Dir.pwd
    RDoc::TopLevel.reset

    @tmpdir = File.join Dir.tmpdir, "test_rdoc_generator_ri_#{$$}"
    FileUtils.mkdir_p @tmpdir
    Dir.chdir @tmpdir

    @g = RDoc::Generator::RI.new @options

    @top_level = RDoc::TopLevel.new 'file.rb'
    @klass = @top_level.add_class RDoc::NormalClass, 'Object'

    @meth = RDoc::AnyMethod.new nil, 'method'
    @meth.record_location @top_level

    @meth_bang = RDoc::AnyMethod.new nil, 'method!'
    @meth_bang.record_location @top_level

    @attr = RDoc::Attr.new nil, 'attr', 'RW', ''
    @attr.record_location @top_level

    @klass.add_method @meth
    @klass.add_method @meth_bang
    @klass.add_attribute @attr
  end

  def teardown
    Dir.chdir @pwd
    FileUtils.rm_rf @tmpdir
  end

  def assert_file path
    assert File.file?(path), "#{path} is not a file"
  end

  def refute_file path
    refute File.exist?(path), "#{path} exists"
  end

  def test_generate
    @g.generate nil

    assert_file File.join(@tmpdir, 'cache.ri')

    assert_file File.join(@tmpdir, 'Object', 'cdesc-Object.ri')

    assert_file File.join(@tmpdir, 'Object', 'attr-i.ri')
    assert_file File.join(@tmpdir, 'Object', 'method-i.ri')
    assert_file File.join(@tmpdir, 'Object', 'method%21-i.ri')

    store = RDoc::RI::Store.new @tmpdir
    store.load_cache

    encoding = Object.const_defined?(:Encoding) ? Encoding::UTF_8 : nil

    assert_equal encoding, store.encoding
  end

  def test_generate_dry_run
    @options.dry_run = true
    @g = RDoc::Generator::RI.new @options

    top_level = RDoc::TopLevel.new 'file.rb'
    top_level.add_class @klass.class, @klass.name

    @g.generate nil

    refute_file File.join(@tmpdir, 'cache.ri')

    refute_file File.join(@tmpdir, 'Object')
  end

end

