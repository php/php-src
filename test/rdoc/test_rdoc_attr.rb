require 'rubygems'
require 'minitest/autorun'
require 'rdoc/rdoc'

class TestRDocAttr < MiniTest::Unit::TestCase

  def setup
    @a = RDoc::Attr.new nil, 'attr', 'RW', ''
  end

  def test_aref
    m = RDoc::Attr.new nil, 'attr', 'RW', nil

    assert_equal 'attribute-i-attr', m.aref
  end

  def test_arglists
    assert_nil @a.arglists
  end

  def test_block_params
    assert_nil @a.block_params
  end

  def test_call_seq
    assert_nil @a.call_seq
  end

  def test_definition
    assert_equal 'attr_accessor', @a.definition

    @a.rw = 'R'

    assert_equal 'attr_reader', @a.definition

    @a.rw = 'W'

    assert_equal 'attr_writer', @a.definition
  end

  def test_full_name
    assert_equal '(unknown)#attr', @a.full_name
  end

  def test_marshal_dump
    tl = RDoc::TopLevel.new 'file.rb'

    @a.comment = 'this is a comment'
    @a.record_location tl

    cm = RDoc::ClassModule.new 'Klass'
    cm.add_attribute @a

    loaded = Marshal.load Marshal.dump @a

    assert_equal @a, loaded

    comment = RDoc::Markup::Document.new(
                RDoc::Markup::Paragraph.new('this is a comment'))

    assert_equal comment,      loaded.comment
    assert_equal 'file.rb',    loaded.file.relative_name
    assert_equal 'Klass#attr', loaded.full_name
    assert_equal 'attr',       loaded.name
    assert_equal 'RW',         loaded.rw
    assert_equal false,        loaded.singleton
    assert_equal :public,      loaded.visibility

    @a.rw = 'R'
    @a.singleton = true
    @a.visibility = :protected

    loaded = Marshal.load Marshal.dump @a

    assert_equal @a, loaded

    assert_equal comment,       loaded.comment
    assert_equal 'Klass::attr', loaded.full_name
    assert_equal 'attr',        loaded.name
    assert_equal 'R',           loaded.rw
    assert_equal true,          loaded.singleton
    assert_equal :protected,    loaded.visibility
  end

  def test_marshal_load_version_1
    data = "\x04\bU:\x0FRDoc::Attr[\fi\x06I\"\tattr\x06:\x06EF" \
           "\"\x0FKlass#attrI\"\aRW\x06;\x06F:\vpublic" \
           "o:\eRDoc::Markup::Document\x06:\v@parts[\x06" \
           "o:\x1CRDoc::Markup::Paragraph\x06;\t[\x06I" \
           "\"\x16this is a comment\x06;\x06FF"

    loaded = Marshal.load data

    comment = RDoc::Markup::Document.new(
                RDoc::Markup::Paragraph.new('this is a comment'))

    assert_equal comment,      loaded.comment
    assert_equal 'Klass#attr', loaded.full_name
    assert_equal 'attr',       loaded.name
    assert_equal 'RW',         loaded.rw
    assert_equal false,        loaded.singleton
    assert_equal :public,      loaded.visibility

    assert_equal nil,          loaded.file # version 2
  end

  def test_params
    assert_nil @a.params
  end

  def test_singleton
    refute @a.singleton
  end

  def test_type
    assert_equal 'instance', @a.type

    @a.singleton = true
    assert_equal 'class', @a.type
  end

end

