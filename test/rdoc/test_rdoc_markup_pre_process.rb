# coding: utf-8

require 'tempfile'
require 'rubygems'
require 'minitest/autorun'
require 'rdoc/markup/pre_process'
require 'rdoc/code_objects'
require 'rdoc/options'

class TestRDocMarkupPreProcess < MiniTest::Unit::TestCase

  def setup
    RDoc::Markup::PreProcess.registered.clear

    @tempfile = Tempfile.new 'test_rdoc_markup_pre_process'
    @file_name = File.basename @tempfile.path
    @dir  = File.dirname @tempfile.path

    @pp = RDoc::Markup::PreProcess.new __FILE__, [@dir]
  end

  def teardown
    RDoc::Markup::PreProcess.registered.clear

    @tempfile.close
  end

  def test_include_file
    @tempfile.write <<-INCLUDE
# -*- mode: rdoc; coding: utf-8; fill-column: 74; -*-

Regular expressions (<i>regexp</i>s) are patterns which describe the
contents of a string.
    INCLUDE

    @tempfile.flush
    @tempfile.rewind

    content = @pp.include_file @file_name, '', nil

    expected = <<-EXPECTED
Regular expressions (<i>regexp</i>s) are patterns which describe the
contents of a string.
    EXPECTED

    assert_equal expected, content
  end

  def test_include_file_encoding_incompatible
    skip "Encoding not implemented" unless Object.const_defined? :Encoding

    @tempfile.write <<-INCLUDE
# -*- mode: rdoc; coding: utf-8; fill-column: 74; -*-

π
    INCLUDE

    @tempfile.flush
    @tempfile.rewind

    content = @pp.include_file @file_name, '', Encoding::US_ASCII

    expected = "?\n"

    assert_equal expected, content
  end

  def test_handle
    text = "# :main: M\n"
    out = @pp.handle text

    assert_same out, text
    assert_equal "#\n", text
  end

  def test_handle_unregistered
    text = "# :x: y\n"
    out = @pp.handle text

    assert_same out, text
    assert_equal "# :x: y\n", text
  end

  def test_handle_directive_blankline
    result = @pp.handle_directive '#', 'arg', 'a, b'

    assert_equal "#\n", result
  end

  def test_handle_directive_downcase
    method = RDoc::AnyMethod.new nil, 'm'

    @pp.handle_directive '', 'ARG', 'a, b', method

    assert_equal 'a, b', method.params
  end

  def test_handle_directive_arg
    method = RDoc::AnyMethod.new nil, 'm'

    @pp.handle_directive '', 'arg', 'a, b', method

    assert_equal 'a, b', method.params
  end

  def test_handle_directive_arg_no_context
    result = @pp.handle_directive '', 'arg', 'a, b', nil

    assert_equal "\n", result
  end

  def test_handle_directive_args
    method = RDoc::AnyMethod.new nil, 'm'

    @pp.handle_directive '', 'args', 'a, b', method

    assert_equal 'a, b', method.params
  end

  def test_handle_directive_block
    result = @pp.handle_directive '', 'x', 'y' do |directive, param|
      ''
    end

    assert_empty result
  end

  def test_handle_directive_block_false
    result = @pp.handle_directive '', 'x', 'y' do |directive, param|
      false
    end

    assert_equal ":x: y\n", result
  end

  def test_handle_directive_block_nil
    result = @pp.handle_directive '', 'x', 'y' do |directive, param|
      nil
    end

    assert_equal ":x: y\n", result
  end

  def test_handle_directive_category
    context = RDoc::Context.new
    original_section = context.current_section

    @pp.handle_directive '', 'category', 'other', context

    refute_equal original_section, context.current_section
  end

  def test_handle_directive_doc
    code_object = RDoc::CodeObject.new
    code_object.document_self = false
    code_object.force_documentation = false

    @pp.handle_directive '', 'doc', nil, code_object

    assert code_object.document_self
    assert code_object.force_documentation
  end

  def test_handle_directive_doc_no_context
    result = @pp.handle_directive '', 'doc', nil

    assert_equal "\n", result
  end

  def test_handle_directive_enddoc
    code_object = RDoc::CodeObject.new

    @pp.handle_directive '', 'enddoc', nil, code_object

    assert code_object.done_documenting
  end

  def test_handle_directive_include
    @tempfile.write 'included'
    @tempfile.flush

    result = @pp.handle_directive '', 'include', @file_name

    assert_equal 'included', result
  end

  def test_handle_directive_main
    @pp.options = RDoc::Options.new

    @pp.handle_directive '', 'main', 'M'

    assert_equal 'M', @pp.options.main_page
  end

  def test_handle_directive_notnew
    m = RDoc::AnyMethod.new nil, 'm'

    @pp.handle_directive '', 'notnew', nil, m

    assert m.dont_rename_initialize
  end

  def test_handle_directive_not_new
    m = RDoc::AnyMethod.new nil, 'm'

    @pp.handle_directive '', 'not_new', nil, m

    assert m.dont_rename_initialize
  end

  def test_handle_directive_not_dash_new
    m = RDoc::AnyMethod.new nil, 'm'

    @pp.handle_directive '', 'not-new', nil, m

    assert m.dont_rename_initialize
  end

  def test_handle_directive_nodoc
    code_object = RDoc::CodeObject.new
    code_object.document_self = true
    code_object.document_children = true

    @pp.handle_directive '', 'nodoc', nil, code_object

    refute code_object.document_self
    assert code_object.document_children
  end

  def test_handle_directive_nodoc_all
    code_object = RDoc::CodeObject.new
    code_object.document_self = true
    code_object.document_children = true

    @pp.handle_directive '', 'nodoc', 'all', code_object

    refute code_object.document_self
    refute code_object.document_children
  end

  def test_handle_directive_nodoc_no_context
    result = @pp.handle_directive '', 'nodoc', nil

    assert_equal "\n", result
  end

  def test_handle_directive_registered
    RDoc::Markup::PreProcess.register 'x'

    result = @pp.handle_directive '', 'x', 'y'

    assert_nil result

    result = @pp.handle_directive '', 'x', 'y' do |directive, param|
      false
    end

    assert_equal ":x: y\n", result

    result = @pp.handle_directive '', 'x', 'y' do |directive, param|
      ''
    end

    assert_equal '', result
  end

  def test_handle_directive_registered_block
    called = nil

    RDoc::Markup::PreProcess.register 'x' do |directive, param|
      called = [directive, param]
      'blah'
    end

    result = @pp.handle_directive '', 'x', 'y'

    assert_equal 'blah', result
    assert_equal %w[x y], called
  end

  def test_handle_directive_registered_code_object
    RDoc::Markup::PreProcess.register 'x'
    code_object = RDoc::CodeObject.new

    @pp.handle_directive '', 'x', 'y', code_object

    assert_equal 'y', code_object.metadata['x']

    code_object.metadata.clear

    result = @pp.handle_directive '', 'x', 'y' do |directive, param|
      false
    end

    assert_equal ":x: y\n", result
    assert_empty code_object.metadata

    result = @pp.handle_directive '', 'x', 'y' do |directive, param|
      ''
    end

    assert_equal '', result
    assert_empty code_object.metadata
  end

  def test_handle_directive_startdoc
    code_object = RDoc::CodeObject.new
    code_object.stop_doc
    code_object.force_documentation = false

    @pp.handle_directive '', 'startdoc', nil, code_object

    assert code_object.document_self
    assert code_object.document_children
    assert code_object.force_documentation
  end

  def test_handle_directive_stopdoc
    code_object = RDoc::CodeObject.new

    @pp.handle_directive '', 'stopdoc', nil, code_object

    refute code_object.document_self
    refute code_object.document_children
  end

  def test_handle_directive_title
    @pp.options = RDoc::Options.new

    @pp.handle_directive '', 'title', 'T'

    assert_equal 'T', @pp.options.title
  end

  def test_handle_directive_unhandled
    code_object = RDoc::CodeObject.new

    @pp.handle_directive '', 'x', 'y', code_object

    assert_equal 'y', code_object.metadata['x']

    code_object.metadata.clear

    @pp.handle_directive '', 'x', '', code_object

    assert_includes code_object.metadata, 'x'
  end

  def test_handle_directive_unhandled_block
    code_object = RDoc::CodeObject.new

    @pp.handle_directive '', 'x', 'y', code_object do
      false
    end

    assert_empty code_object.metadata

    @pp.handle_directive '', 'x', 'y', code_object do
      nil
    end

    assert_equal 'y', code_object.metadata['x']

    code_object.metadata.clear

    @pp.handle_directive '', 'x', 'y', code_object do
      ''
    end

    assert_empty code_object.metadata
  end

  def test_handle_directive_yield
    method = RDoc::AnyMethod.new nil, 'm'
    method.params = 'index, &block'

    @pp.handle_directive '', 'yield', 'item', method

    assert_equal 'item', method.block_params
    assert_equal 'index', method.params
  end

  def test_handle_directive_yield_block_param
    method = RDoc::AnyMethod.new nil, 'm'
    method.params = '&block'

    @pp.handle_directive '', 'yield', 'item', method

    assert_equal 'item', method.block_params
    assert_empty method.params
  end

  def test_handle_directive_yield_no_context
    method = RDoc::AnyMethod.new nil, 'm'

    @pp.handle_directive '', 'yield', 'item', method

    assert_equal 'item', method.block_params
  end

  def test_handle_directive_yields
    method = RDoc::AnyMethod.new nil, 'm'

    @pp.handle_directive '', 'yields', 'item', method

    assert_equal 'item', method.block_params
  end

end

