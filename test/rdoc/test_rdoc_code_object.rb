# coding: US-ASCII

require 'rubygems'
require 'minitest/autorun'
require File.expand_path '../xref_test_case', __FILE__
require 'rdoc/code_object'

class TestRDocCodeObject < XrefTestCase

  def setup
    super

    @co = RDoc::CodeObject.new
  end

  def test_initialize
    assert @co.document_self, 'document_self'
    assert @co.document_children, 'document_children'
    refute @co.force_documentation, 'force_documentation'
    refute @co.done_documenting, 'done_documenting'
    refute @co.received_nodoc, 'received_nodoc'
    assert_equal '', @co.comment, 'comment is empty'
  end

  def test_comment_equals
    @co.comment = ''

    assert_equal '', @co.comment

    @co.comment = 'I am a comment'

    assert_equal 'I am a comment', @co.comment
  end

  def test_comment_equals_document
    doc = RDoc::Markup::Document.new
    @co.comment = doc

    @co.comment = ''

    assert_equal doc, @co.comment
  end

  def test_comment_equals_encoding
    skip "Encoding not implemented" unless Object.const_defined? :Encoding

    refute_equal Encoding::UTF_8, ''.encoding, 'Encoding sanity check'

    input = 'text'
    input.force_encoding Encoding::UTF_8

    @co.comment = input

    assert_equal 'text', @co.comment
    assert_equal Encoding::UTF_8, @co.comment.encoding
  end

  def test_comment_equals_encoding_blank
    skip "Encoding not implemented" unless Object.const_defined? :Encoding

    refute_equal Encoding::UTF_8, ''.encoding, 'Encoding sanity check'

    input = ''
    input.force_encoding Encoding::UTF_8

    @co.comment = input

    assert_equal '', @co.comment
    assert_equal Encoding::UTF_8, @co.comment.encoding
  end

  def test_display_eh_document_self
    assert @co.display?

    @co.document_self = false

    refute @co.display?
  end

  def test_display_eh_ignore
    assert @co.display?

    @co.ignore

    refute @co.display?

    @co.stop_doc

    refute @co.display?

    @co.done_documenting = false

    refute @co.display?
  end

  def test_document_children_equals
    @co.document_children = false
    refute @co.document_children

    # TODO this is not true anymore:
    # test all the nodoc stuff etc...
    #@c2.document_children = false
    #assert_empty @c2.classes
  end

  def test_document_self_equals
    @co.document_self = false
    refute @co.document_self

    # TODO this is not true anymore:
    # test all the nodoc stuff etc...
    #@c1.document_self = false
    #assert_empty @c1.method_list
  end

  def test_documented_eh
    refute @co.documented?

    @co.comment = 'hi'

    assert @co.documented?

    @co.comment.replace ''

    refute @co.documented?

    @co.document_self = nil # notify :nodoc:

    assert @co.documented?
  end

  def test_done_documenting
    # once done_documenting is set, other properties refuse to go to "true"
    @co.done_documenting = true

    @co.document_self = true
    refute @co.document_self

    @co.document_children = true
    refute @co.document_children

    @co.force_documentation = true
    refute @co.force_documentation

    @co.start_doc
    refute @co.document_self
    refute @co.document_children

    # turning done_documenting on
    # resets others to true

    @co.done_documenting = false
    assert @co.document_self
    assert @co.document_children
  end

  def test_each_parent
    parents = []

    @parent_m.each_parent do |code_object|
      parents << code_object
    end

    assert_equal [@parent, @xref_data], parents
  end

  def test_file_name
    assert_equal nil, @co.file_name

    @co.record_location RDoc::TopLevel.new 'lib/file.rb'

    assert_equal 'lib/file.rb', @co.file_name
  end

  def test_full_name_equals
    @co.full_name = 'hi'

    assert_equal 'hi', @co.instance_variable_get(:@full_name)

    @co.full_name = nil

    assert_nil @co.instance_variable_get(:@full_name)
  end

  def test_ignore
    @co.ignore

    refute @co.document_self
    refute @co.document_children
    assert @co.ignored?
  end

  def test_ignore_eh
    refute @co.ignored?

    @co.ignore

    assert @co.ignored?
  end

  def test_line
    @c1_m.line = 5

    assert_equal 5, @c1_m.line
  end

  def test_metadata
    assert_empty @co.metadata

    @co.metadata['markup'] = 'not_rdoc'

    expected = { 'markup' => 'not_rdoc' }

    assert_equal expected, @co.metadata

    assert_equal 'not_rdoc', @co.metadata['markup']
  end

  def test_offset
    @c1_m.offset = 5

    assert_equal 5, @c1_m.offset
  end

  def test_parent_file_name
    assert_equal '(unknown)', @co.parent_file_name
    assert_equal 'xref_data.rb', @c1.parent_file_name
  end

  def test_parent_name
    assert_equal '(unknown)', @co.parent_name
    assert_equal 'xref_data.rb', @c1.parent_name
    assert_equal 'C2', @c2_c3.parent_name
  end

  def test_received_ndoc
    @co.document_self = false
    refute @co.received_nodoc

    @co.document_self = nil
    assert @co.received_nodoc

    @co.document_self = true
  end

  def test_record_location
    @co.record_location @xref_data

    assert_equal 'xref_data.rb', @co.file.relative_name
  end

  def test_record_location_ignored
    @co.ignore
    @co.record_location @xref_data

    refute @co.ignored?
  end

  def test_start_doc
    @co.document_self = false
    @co.document_children = false

    @co.start_doc

    assert @co.document_self
    assert @co.document_children
  end

  def test_start_doc_ignored
    @co.ignore

    @co.start_doc

    assert @co.document_self
    assert @co.document_children
    refute @co.ignored?
  end

  def test_stop_doc
    @co.document_self = true
    @co.document_children = true

    @co.stop_doc

    refute @co.document_self
    refute @co.document_children
  end

end

