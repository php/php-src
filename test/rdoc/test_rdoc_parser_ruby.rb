# coding: utf-8

require 'stringio'
require 'tempfile'
require 'rubygems'
require 'minitest/autorun'

require 'rdoc/options'
require 'rdoc/parser/ruby'
require 'rdoc/stats'

class TestRDocParserRuby < MiniTest::Unit::TestCase

  def setup
    @tempfile = Tempfile.new self.class.name
    @filename = @tempfile.path

    # Some tests need two paths.
    @tempfile2 = Tempfile.new self.class.name
    @filename2 = @tempfile2.path

    util_top_level
    @options = RDoc::Options.new
    @options.quiet = true
    @options.option_parser = OptionParser.new

    @stats = RDoc::Stats.new 0
  end

  def teardown
    @tempfile.close
    @tempfile2.close
  end

  def test_collect_first_comment
    p = util_parser <<-CONTENT
# first

# second
class C; end
    CONTENT

    comment = p.collect_first_comment

    assert_equal "# first\n", comment
  end

  def test_collect_first_comment_encoding
    skip "Encoding not implemented" unless Object.const_defined? :Encoding

    @options.encoding = Encoding::CP852

    p = util_parser <<-CONTENT
# first

# second
class C; end
    CONTENT

    comment = p.collect_first_comment

    assert_equal Encoding::CP852, comment.encoding
  end

  def test_extract_call_seq
    m = RDoc::AnyMethod.new nil, 'm'
    p = util_parser ''

    comment = <<-COMMENT
  # call-seq:
  #   bla => true or false
  #
  # moar comment
    COMMENT

    p.extract_call_seq comment, m

    assert_equal "bla => true or false\n", m.call_seq
  end

  def test_extract_call_seq_blank
    m = RDoc::AnyMethod.new nil, 'm'
    p = util_parser ''

    comment = <<-COMMENT
  # call-seq:
  #   bla => true or false
  #
    COMMENT

    p.extract_call_seq comment, m

    assert_equal "bla => true or false\n", m.call_seq
  end

  def test_extract_call_seq_no_blank
    m = RDoc::AnyMethod.new nil, 'm'
    p = util_parser ''

    comment = <<-COMMENT
  # call-seq:
  #   bla => true or false
    COMMENT

    p.extract_call_seq comment, m

    assert_equal "bla => true or false\n", m.call_seq
  end

  def test_extract_call_seq_undent
    m = RDoc::AnyMethod.new nil, 'm'
    p = util_parser ''

    comment = <<-COMMENT
  # call-seq:
  #   bla => true or false
  # moar comment
    COMMENT

    p.extract_call_seq comment, m

    assert_equal "bla => true or false\nmoar comment\n", m.call_seq
  end

  def test_get_symbol_or_name
    util_parser "* & | + 5 / 4"

    assert_equal '*', @parser.get_symbol_or_name

    @parser.skip_tkspace

    assert_equal '&', @parser.get_symbol_or_name

    @parser.skip_tkspace

    assert_equal '|', @parser.get_symbol_or_name

    @parser.skip_tkspace

    assert_equal '+', @parser.get_symbol_or_name

    @parser.skip_tkspace
    @parser.get_tk
    @parser.skip_tkspace

    assert_equal '/', @parser.get_symbol_or_name
  end

  def test_look_for_directives_in_attr
    util_parser ""

    comment = "# :attr: my_attr\n"

    @parser.look_for_directives_in @top_level, comment

    assert_equal "# :attr: my_attr\n", comment

    comment = "# :attr_reader: my_method\n"

    @parser.look_for_directives_in @top_level, comment

    assert_equal "# :attr_reader: my_method\n", comment

    comment = "# :attr_writer: my_method\n"

    @parser.look_for_directives_in @top_level, comment

    assert_equal "# :attr_writer: my_method\n", comment
  end

  def test_remove_private_comments
    util_parser ''

    comment = <<-EOS
# This is text
#--
# this is private
    EOS

    expected = <<-EOS
# This is text
    EOS

    @parser.remove_private_comments(comment)

    assert_equal expected, comment
  end

  def test_remove_private_comments_encoding
    skip "Encoding not implemented" unless Object.const_defined? :Encoding

    util_parser ''

    comment = <<-EOS
# This is text
#--
# this is private
    EOS
    comment.force_encoding Encoding::IBM437

    @parser.remove_private_comments comment

    assert_equal Encoding::IBM437, comment.encoding
  end

  def test_remove_private_comments_long
    util_parser ''

    comment = <<-EOS
#-----
#++
# this is text
#-----
    EOS

    expected = <<-EOS
# this is text
    EOS

    @parser.remove_private_comments(comment)

    assert_equal expected, comment
  end

  def test_remove_private_comments_rule
    util_parser ''

    comment = <<-EOS
# This is text with a rule:
# ---
# this is also text
    EOS

    expected = comment.dup

    @parser.remove_private_comments(comment)

    assert_equal expected, comment
  end

  def test_remove_private_comments_toggle
    util_parser ''

    comment = <<-EOS
# This is text
#--
# this is private
#++
# This is text again.
    EOS

    expected = <<-EOS
# This is text
# This is text again.
    EOS

    @parser.remove_private_comments(comment)

    assert_equal expected, comment
  end

  def test_remove_private_comments_toggle_encoding
    skip "Encoding not implemented" unless Object.const_defined? :Encoding

    util_parser ''

    comment = <<-EOS
# This is text
#--
# this is private
#++
# This is text again.
    EOS

    comment.force_encoding Encoding::IBM437

    @parser.remove_private_comments comment

    assert_equal Encoding::IBM437, comment.encoding
  end

  def test_remove_private_comments_toggle_encoding_ruby_bug?
    skip "Encoding not implemented" unless Object.const_defined? :Encoding

    util_parser ''

    comment = <<-EOS
#--
# this is private
#++
# This is text again.
    EOS

    comment.force_encoding Encoding::IBM437

    @parser.remove_private_comments comment

    assert_equal Encoding::IBM437, comment.encoding
  end

  def test_look_for_directives_in_commented
    util_parser ""

    comment = "# how to make a section:\n# # :section: new section\n"

    @parser.look_for_directives_in @top_level, comment

    section = @top_level.current_section
    assert_equal nil, section.title
    assert_equal nil, section.comment

    assert_equal "# how to make a section:\n# # :section: new section\n",
                 comment
  end

  def test_look_for_directives_in_method
    util_parser ""

    comment = "# :method: my_method\n"

    @parser.look_for_directives_in @top_level, comment

    assert_equal "# :method: my_method\n", comment

    comment = "# :singleton-method: my_method\n"

    @parser.look_for_directives_in @top_level, comment

    assert_equal "# :singleton-method: my_method\n", comment
  end

  def test_look_for_directives_in_section
    util_parser ""

    comment = "# :section: new section\n# woo stuff\n"

    @parser.look_for_directives_in @top_level, comment

    section = @top_level.current_section
    assert_equal 'new section', section.title
    assert_equal "# woo stuff\n", section.comment

    assert_equal '', comment
  end

  def test_look_for_directives_in_unhandled
    util_parser ""

    @parser.look_for_directives_in @top_level, "# :unhandled: blah\n"

    assert_equal 'blah', @top_level.metadata['unhandled']
  end

  def test_parse_alias
    klass = RDoc::NormalClass.new 'Foo'
    klass.parent = @top_level

    util_parser "alias :next= :bar"

    tk = @parser.get_tk

    alas = @parser.parse_alias klass, RDoc::Parser::Ruby::NORMAL, tk, 'comment'

    assert_equal 'bar',      alas.old_name
    assert_equal 'next=',    alas.new_name
    assert_equal klass,      alas.parent
    assert_equal 'comment',  alas.comment
    assert_equal @top_level, alas.file
    assert_equal 0,          alas.offset
    assert_equal 1,          alas.line
  end

  def test_parse_alias_singleton
    klass = RDoc::NormalClass.new 'Foo'
    klass.parent = @top_level

    util_parser "alias :next= :bar"

    tk = @parser.get_tk

    alas = @parser.parse_alias klass, RDoc::Parser::Ruby::SINGLE, tk, 'comment'

    assert_equal 'bar',      alas.old_name
    assert_equal 'next=',    alas.new_name
    assert_equal klass,      alas.parent
    assert_equal 'comment',  alas.comment
    assert_equal @top_level, alas.file
    assert                   alas.singleton
  end

  def test_parse_alias_stopdoc
    klass = RDoc::NormalClass.new 'Foo'
    klass.parent = @top_level
    klass.stop_doc

    util_parser "alias :next= :bar"

    tk = @parser.get_tk

    @parser.parse_alias klass, RDoc::Parser::Ruby::NORMAL, tk, 'comment'

    assert_empty klass.aliases
    assert_empty klass.unmatched_alias_lists
  end

  def test_parse_alias_meta
    klass = RDoc::NormalClass.new 'Foo'
    klass.parent = @top_level

    util_parser "alias m.chop m"

    tk = @parser.get_tk

    alas = @parser.parse_alias klass, RDoc::Parser::Ruby::NORMAL, tk, 'comment'

    assert_nil alas
  end

  def test_parse_attr
    klass = RDoc::NormalClass.new 'Foo'
    klass.parent = @top_level

    comment = "##\n# my attr\n"

    util_parser "attr :foo, :bar"

    tk = @parser.get_tk

    @parser.parse_attr klass, RDoc::Parser::Ruby::NORMAL, tk, comment

    assert_equal 1, klass.attributes.length

    foo = klass.attributes.first
    assert_equal 'foo', foo.name
    assert_equal 'my attr', foo.comment
    assert_equal @top_level, foo.file
    assert_equal 0, foo.offset
    assert_equal 1, foo.line
  end

  def test_parse_attr_stopdoc
    klass = RDoc::NormalClass.new 'Foo'
    klass.parent = @top_level
    klass.stop_doc

    comment = "##\n# my attr\n"

    util_parser "attr :foo, :bar"

    tk = @parser.get_tk

    @parser.parse_attr klass, RDoc::Parser::Ruby::NORMAL, tk, comment

    assert_empty klass.attributes
  end

  def test_parse_attr_accessor
    klass = RDoc::NormalClass.new 'Foo'
    klass.parent = @top_level

    comment = "##\n# my attr\n"

    util_parser "attr_accessor :foo, :bar"

    tk = @parser.get_tk

    @parser.parse_attr_accessor klass, RDoc::Parser::Ruby::NORMAL, tk, comment

    assert_equal 2, klass.attributes.length

    foo = klass.attributes.first
    assert_equal 'foo', foo.name
    assert_equal 'RW', foo.rw
    assert_equal 'my attr', foo.comment
    assert_equal @top_level, foo.file
    assert_equal 0, foo.offset
    assert_equal 1, foo.line

    bar = klass.attributes.last
    assert_equal 'bar', bar.name
    assert_equal 'RW', bar.rw
    assert_equal 'my attr', bar.comment
  end

  def test_parse_attr_accessor_nodoc
    klass = RDoc::NormalClass.new 'Foo'
    klass.parent = @top_level

    comment = "##\n# my attr\n"

    util_parser "attr_accessor :foo, :bar # :nodoc:"

    tk = @parser.get_tk

    @parser.parse_attr_accessor klass, RDoc::Parser::Ruby::NORMAL, tk, comment

    assert_equal 0, klass.attributes.length
  end

  def test_parse_attr_accessor_stopdoc
    klass = RDoc::NormalClass.new 'Foo'
    klass.parent = @top_level
    klass.stop_doc

    comment = "##\n# my attr\n"

    util_parser "attr_accessor :foo, :bar"

    tk = @parser.get_tk

    @parser.parse_attr_accessor klass, RDoc::Parser::Ruby::NORMAL, tk, comment

    assert_empty klass.attributes
  end

  def test_parse_attr_accessor_writer
    klass = RDoc::NormalClass.new 'Foo'
    klass.parent = @top_level

    comment = "##\n# my attr\n"

    util_parser "attr_writer :foo, :bar"

    tk = @parser.get_tk

    @parser.parse_attr_accessor klass, RDoc::Parser::Ruby::NORMAL, tk, comment

    assert_equal 2, klass.attributes.length

    foo = klass.attributes.first
    assert_equal 'foo', foo.name
    assert_equal 'W', foo.rw
    assert_equal "my attr", foo.comment
    assert_equal @top_level, foo.file

    bar = klass.attributes.last
    assert_equal 'bar', bar.name
    assert_equal 'W', bar.rw
    assert_equal "my attr", bar.comment
  end

  def test_parse_meta_attr
    klass = RDoc::NormalClass.new 'Foo'
    klass.parent = @top_level

    comment = "##\n# :attr: \n# my method\n"

    util_parser "add_my_method :foo, :bar"

    tk = @parser.get_tk

    @parser.parse_meta_attr klass, RDoc::Parser::Ruby::NORMAL, tk, comment

    assert_equal 2, klass.attributes.length
    foo = klass.attributes.first
    assert_equal 'foo', foo.name
    assert_equal 'RW', foo.rw
    assert_equal "my method", foo.comment
    assert_equal @top_level, foo.file
  end

  def test_parse_meta_attr_accessor
    klass = RDoc::NormalClass.new 'Foo'
    klass.parent = @top_level

    comment = "##\n# :attr_accessor: \n# my method\n"

    util_parser "add_my_method :foo, :bar"

    tk = @parser.get_tk

    @parser.parse_meta_attr klass, RDoc::Parser::Ruby::NORMAL, tk, comment

    assert_equal 2, klass.attributes.length
    foo = klass.attributes.first
    assert_equal 'foo', foo.name
    assert_equal 'RW', foo.rw
    assert_equal 'my method', foo.comment
    assert_equal @top_level, foo.file
  end

  def test_parse_meta_attr_named
    klass = RDoc::NormalClass.new 'Foo'
    klass.parent = @top_level

    comment = "##\n# :attr: foo\n# my method\n"

    util_parser "add_my_method :foo, :bar"

    tk = @parser.get_tk

    @parser.parse_meta_attr klass, RDoc::Parser::Ruby::NORMAL, tk, comment

    assert_equal 1, klass.attributes.length
    foo = klass.attributes.first
    assert_equal 'foo', foo.name
    assert_equal 'RW', foo.rw
    assert_equal 'my method', foo.comment
    assert_equal @top_level, foo.file
  end

  def test_parse_meta_attr_reader
    klass = RDoc::NormalClass.new 'Foo'
    klass.parent = @top_level

    comment = "##\n# :attr_reader: \n# my method\n"

    util_parser "add_my_method :foo, :bar"

    tk = @parser.get_tk

    @parser.parse_meta_attr klass, RDoc::Parser::Ruby::NORMAL, tk, comment

    foo = klass.attributes.first
    assert_equal 'foo', foo.name
    assert_equal 'R', foo.rw
    assert_equal 'my method', foo.comment
    assert_equal @top_level, foo.file
  end

  def test_parse_meta_attr_stopdoc
    klass = RDoc::NormalClass.new 'Foo'
    klass.parent = @top_level
    klass.stop_doc

    comment = "##\n# :attr: \n# my method\n"

    util_parser "add_my_method :foo, :bar"

    tk = @parser.get_tk

    @parser.parse_meta_attr klass, RDoc::Parser::Ruby::NORMAL, tk, comment

    assert_empty klass.attributes
  end

  def test_parse_meta_attr_writer
    klass = RDoc::NormalClass.new 'Foo'
    klass.parent = @top_level

    comment = "##\n# :attr_writer: \n# my method\n"

    util_parser "add_my_method :foo, :bar"

    tk = @parser.get_tk

    @parser.parse_meta_attr klass, RDoc::Parser::Ruby::NORMAL, tk, comment

    foo = klass.attributes.first
    assert_equal 'foo', foo.name
    assert_equal 'W', foo.rw
    assert_equal "my method", foo.comment
    assert_equal @top_level, foo.file
  end

  def test_parse_class
    comment = "##\n# my class\n"

    util_parser "class Foo\nend"

    tk = @parser.get_tk

    @parser.parse_class @top_level, RDoc::Parser::Ruby::NORMAL, tk, comment

    foo = @top_level.classes.first
    assert_equal 'Foo', foo.full_name
    assert_equal 'my class', foo.comment
    assert_equal [@top_level], foo.in_files
    assert_equal 0, foo.offset
    assert_equal 1, foo.line
  end

  def test_parse_class_ghost_method
    util_parser <<-CLASS
class Foo
  ##
  # :method: blah
  # my method
end
    CLASS

    tk = @parser.get_tk

    @parser.parse_class @top_level, RDoc::Parser::Ruby::NORMAL, tk, ''

    foo = @top_level.classes.first
    assert_equal 'Foo', foo.full_name

    blah = foo.method_list.first
    assert_equal 'Foo#blah', blah.full_name
    assert_equal @top_level, blah.file
  end

  def test_parse_class_multi_ghost_methods
    util_parser <<-'CLASS'
class Foo
  ##
  # :method: one
  #
  # my method

  ##
  # :method: two
  #
  # my method

  [:one, :two].each do |t|
    eval("def #{t}; \"#{t}\"; end")
  end
end
    CLASS

    tk = @parser.get_tk

    @parser.parse_class @top_level, RDoc::Parser::Ruby::NORMAL, tk, ''

    foo = @top_level.classes.first
    assert_equal 'Foo', foo.full_name

    assert_equal 2, foo.method_list.length
  end

  def test_parse_class_nodoc
    comment = "##\n# my class\n"

    util_parser "class Foo # :nodoc:\nend"

    tk = @parser.get_tk

    @parser.parse_class @top_level, RDoc::Parser::Ruby::NORMAL, tk, comment

    foo = @top_level.classes.first
    assert_equal 'Foo', foo.full_name
    assert_empty foo.comment
    assert_equal [@top_level], foo.in_files
    assert_equal 0, foo.offset
    assert_equal 1, foo.line
  end

  def test_parse_class_stopdoc
    @top_level.stop_doc

    comment = "##\n# my class\n"

    util_parser "class Foo\nend"

    tk = @parser.get_tk

    @parser.parse_class @top_level, RDoc::Parser::Ruby::NORMAL, tk, comment

    assert_empty @top_level.classes.first.comment
  end

  def test_parse_multi_ghost_methods
    util_parser <<-'CLASS'
class Foo
  ##
  # :method: one
  #
  # my method

  ##
  # :method: two
  #
  # my method

  [:one, :two].each do |t|
    eval("def #{t}; \"#{t}\"; end")
  end
end
    CLASS

    tk = @parser.get_tk

    @parser.parse_class @top_level, RDoc::Parser::Ruby::NORMAL, tk, ''

    foo = @top_level.classes.first
    assert_equal 'Foo', foo.full_name

    assert_equal 2, foo.method_list.length
  end

  def test_parse_const_fail_w_meta
    util_parser <<-CLASS
class ConstFailMeta
  ##
  # :attr: one
  #
  # an attribute

  OtherModule.define_attr(self, :one)
end
    CLASS

    tk = @parser.get_tk

    @parser.parse_class @top_level, RDoc::Parser::Ruby::NORMAL, tk, ''

    const_fail_meta = @top_level.classes.first
    assert_equal 'ConstFailMeta', const_fail_meta.full_name

    assert_equal 1, const_fail_meta.attributes.length
  end

  def test_parse_class_nested_superclass
    util_top_level
    foo = @top_level.add_module RDoc::NormalModule, 'Foo'

    util_parser "class Bar < Super\nend"

    tk = @parser.get_tk

    @parser.parse_class foo, RDoc::Parser::Ruby::NORMAL, tk, ''

    bar = foo.classes.first
    assert_equal 'Super', bar.superclass
  end

  def test_parse_module
    comment = "##\n# my module\n"

    util_parser "module Foo\nend"

    tk = @parser.get_tk

    @parser.parse_module @top_level, RDoc::Parser::Ruby::NORMAL, tk, comment

    foo = @top_level.modules.first
    assert_equal 'Foo', foo.full_name
    assert_equal 'my module', foo.comment
  end

  def test_parse_module_nodoc
    @top_level.stop_doc

    comment = "##\n# my module\n"

    util_parser "module Foo # :nodoc:\nend"

    tk = @parser.get_tk

    @parser.parse_module @top_level, RDoc::Parser::Ruby::NORMAL, tk, comment

    foo = @top_level.modules.first
    assert_equal 'Foo', foo.full_name
    assert_empty foo.comment
  end

  def test_parse_module_stopdoc
    @top_level.stop_doc

    comment = "##\n# my module\n"

    util_parser "module Foo\nend"

    tk = @parser.get_tk

    @parser.parse_module @top_level, RDoc::Parser::Ruby::NORMAL, tk, comment

    foo = @top_level.modules.first
    assert_equal 'Foo', foo.full_name
    assert_equal 'my module', foo.comment
  end

  def test_parse_class_colon3
    code = <<-CODE
class A
  class ::B
  end
end
    CODE

    util_parser code

    @parser.parse_class @top_level, false, @parser.get_tk, ''

    assert_equal %w[A B],    RDoc::TopLevel.classes.map { |c| c.full_name }
  end

  def test_parse_class_single
    code = <<-CODE
class A
  class << B
  end
  class << d = Object.new
    def foo; end
    alias bar foo
  end
end
    CODE

    util_parser code

    @parser.parse_class @top_level, false, @parser.get_tk, ''

    assert_equal %w[A],    RDoc::TopLevel.classes.map { |c| c.full_name }
    assert_equal %w[A::B A::d], RDoc::TopLevel.modules.map { |c| c.full_name }

    b = RDoc::TopLevel.modules.first
    assert_equal 10, b.offset
    assert_equal 2,  b.line

    # make sure method/alias was not added to enclosing class/module
    a = RDoc::TopLevel.all_classes_hash['A']
    assert_empty a.method_list

    # make sure non-constant-named module will be removed from documentation
    d = RDoc::TopLevel.all_modules_hash['A::d']
    assert d.remove_from_documentation?

  end

  # TODO this is really a Context#add_class test
  def test_parse_class_object
    code = <<-CODE
module A
  class B
  end
  class Object
  end
  class C < Object
  end
end
    CODE

    util_parser code

    @parser.parse_module @top_level, false, @parser.get_tk, ''

    assert_equal %w[A],    RDoc::TopLevel.modules.map { |c| c.full_name }
    assert_equal %w[A::B A::C A::Object], RDoc::TopLevel.classes.map { |c| c.full_name }.sort
    assert_equal 'Object', RDoc::TopLevel.classes_hash['A::B'].superclass
    assert_equal 'Object', RDoc::TopLevel.classes_hash['A::Object'].superclass
    assert_equal 'A::Object', RDoc::TopLevel.classes_hash['A::C'].superclass.full_name
  end

  def test_parse_class_mistaken_for_module
    # The code below is not strictly legal Ruby (Foo must have been defined
    # before Foo::Bar is encountered), but RDoc might encounter Foo::Bar
    # before Foo if they live in different files.

    code = <<-RUBY
class Foo::Bar
end

module Foo::Baz
end

class Foo
end
    RUBY

    util_parser code

    @parser.scan

    assert_equal %w[Foo::Baz], RDoc::TopLevel.modules_hash.keys
    assert_empty @top_level.modules

    foo = @top_level.classes.first
    assert_equal 'Foo', foo.full_name

    bar = foo.classes.first
    assert_equal 'Foo::Bar', bar.full_name

    baz = foo.modules.first
    assert_equal 'Foo::Baz', baz.full_name
  end

  def test_parse_class_definition_encountered_after_class_reference
    # The code below is not strictly legal Ruby (Foo must have been defined
    # before Foo.bar is encountered), but RDoc might encounter Foo.bar before
    # Foo if they live in different files.

    code = <<-EOF
def Foo.bar
end

class Foo < IO
end
    EOF

    util_parser code

    @parser.scan

    assert_empty RDoc::TopLevel.modules_hash
    # HACK why does it fail?
    #assert_empty @top_level.modules

    foo = @top_level.classes.first
    assert_equal 'Foo', foo.full_name
    assert_equal 'IO', foo.superclass

    bar = foo.method_list.first
    assert_equal 'bar', bar.name
  end

  def test_parse_module_relative_to_top_level_namespace
    comment = <<-EOF
#
# Weirdly named module
#
EOF

    code = comment + <<-EOF
module ::Foo
  class Helper
  end
end
EOF

    util_parser code
    @parser.scan()

    foo = @top_level.modules.first
    assert_equal 'Foo', foo.full_name
    assert_equal 'Weirdly named module', foo.comment

    helper = foo.classes.first
    assert_equal 'Foo::Helper', helper.full_name
  end

  def test_parse_comment_attr
    klass = RDoc::NormalClass.new 'Foo'
    klass.parent = @top_level

    comment = "##\n# :attr: foo\n# my attr\n"

    util_parser "\n"

    tk = @parser.get_tk

    @parser.parse_comment klass, tk, comment

    foo = klass.attributes.first
    assert_equal 'foo',      foo.name
    assert_equal 'RW',       foo.rw
    assert_equal 'my attr',  foo.comment
    assert_equal @top_level, foo.file
    assert_equal 0,          foo.offset
    assert_equal 1,          foo.line

    assert_equal nil,        foo.viewer
    assert_equal true,       foo.document_children
    assert_equal true,       foo.document_self
    assert_equal false,      foo.done_documenting
    assert_equal false,      foo.force_documentation
    assert_equal klass,      foo.parent
    assert_equal :public,    foo.visibility
    assert_equal "\n",       foo.text

    assert_equal klass.current_section, foo.section
  end

  def test_parse_comment_attr_stopdoc
    klass = RDoc::NormalClass.new 'Foo'
    klass.parent = @top_level
    klass.stop_doc

    comment = "##\n# :attr: foo\n# my attr\n"

    util_parser "\n"

    tk = @parser.get_tk

    @parser.parse_comment klass, tk, comment

    assert_empty klass.attributes
  end

  def test_parse_comment_method
    klass = RDoc::NormalClass.new 'Foo'
    klass.parent = @top_level

    comment = "##\n# :method: foo\n# my method\n"

    util_parser "\n"

    tk = @parser.get_tk

    @parser.parse_comment klass, tk, comment

    foo = klass.method_list.first
    assert_equal 'foo',       foo.name
    assert_equal 'my method', foo.comment
    assert_equal @top_level,  foo.file
    assert_equal 0,           foo.offset
    assert_equal 1,           foo.line

    assert_equal [],        foo.aliases
    assert_equal nil,       foo.block_params
    assert_equal nil,       foo.call_seq
    assert_equal nil,       foo.is_alias_for
    assert_equal nil,       foo.viewer
    assert_equal true,      foo.document_children
    assert_equal true,      foo.document_self
    assert_equal '',        foo.params
    assert_equal false,     foo.done_documenting
    assert_equal false,     foo.dont_rename_initialize
    assert_equal false,     foo.force_documentation
    assert_equal klass,     foo.parent
    assert_equal false,     foo.singleton
    assert_equal :public,   foo.visibility
    assert_equal "\n",      foo.text
    assert_equal klass.current_section, foo.section

    stream = [
      tk(:COMMENT, 1, 1, nil, "# File #{@top_level.absolute_name}, line 1"),
      RDoc::Parser::Ruby::NEWLINE_TOKEN,
      tk(:SPACE,   1, 1, nil, ''),
    ]

    assert_equal stream, foo.token_stream
  end

  def test_parse_comment_method_stopdoc
    klass = RDoc::NormalClass.new 'Foo'
    klass.parent = @top_level
    klass.stop_doc

    comment = "##\n# :method: foo\n# my method\n"

    util_parser "\n"

    tk = @parser.get_tk

    @parser.parse_comment klass, tk, comment

    assert_empty klass.method_list
  end

  def test_parse_constant
    util_top_level

    klass = @top_level.add_class RDoc::NormalClass, 'Foo'

    util_parser "A = v"

    tk = @parser.get_tk

    @parser.parse_constant klass, tk, ''

    foo = klass.constants.first

    assert_equal 'A', foo.name
    assert_equal @top_level, foo.file
    assert_equal 0, foo.offset
    assert_equal 1, foo.line
  end

  def test_parse_constant_attrasgn
    util_top_level

    klass = @top_level.add_class RDoc::NormalClass, 'Foo'

    util_parser "A[k] = v"

    tk = @parser.get_tk

    @parser.parse_constant klass, tk, ''

    assert klass.constants.empty?
  end

  def test_parse_constant_alias
    util_top_level
    klass = @top_level.add_class RDoc::NormalClass, 'Foo'
    cB = klass.add_class RDoc::NormalClass, 'B'

    util_parser "A = B"

    tk = @parser.get_tk

    @parser.parse_constant klass, tk, ''

    assert_equal cB, klass.find_module_named('A')
  end

  def test_parse_constant_alias_same_name
    foo = @top_level.add_class RDoc::NormalClass, 'Foo'
    top_bar = @top_level.add_class RDoc::NormalClass, 'Bar'
    bar = foo.add_class RDoc::NormalClass, 'Bar'

    assert RDoc::TopLevel.find_class_or_module('::Bar')

    util_parser "A = ::Bar"

    tk = @parser.get_tk

    @parser.parse_constant foo, tk, ''

    assert_equal top_bar, bar.find_module_named('A')
  end

  def test_parse_constant_stopdoc
    util_top_level

    klass = @top_level.add_class RDoc::NormalClass, 'Foo'
    klass.stop_doc

    util_parser "A = v"

    tk = @parser.get_tk

    @parser.parse_constant klass, tk, ''

    assert_empty klass.constants
  end

  def test_parse_include
    klass = RDoc::NormalClass.new 'C'
    klass.parent = @top_level

    comment = "# my include\n"

    util_parser "include I"

    @parser.get_tk # include

    @parser.parse_include klass, comment

    assert_equal 1, klass.includes.length

    incl = klass.includes.first
    assert_equal 'I', incl.name
    assert_equal 'my include', incl.comment
    assert_equal @top_level, incl.file
  end

  def test_parse_meta_method
    klass = RDoc::NormalClass.new 'Foo'
    klass.parent = @top_level

    comment = "##\n# my method\n"

    util_parser "add_my_method :foo, :bar\nadd_my_method :baz"

    tk = @parser.get_tk

    @parser.parse_meta_method klass, RDoc::Parser::Ruby::NORMAL, tk, comment

    foo = klass.method_list.first
    assert_equal 'foo',       foo.name
    assert_equal 'my method', foo.comment
    assert_equal @top_level,  foo.file
    assert_equal 0,           foo.offset
    assert_equal 1,           foo.line

    assert_equal [],      foo.aliases
    assert_equal nil,     foo.block_params
    assert_equal nil,     foo.call_seq
    assert_equal true,    foo.document_children
    assert_equal true,    foo.document_self
    assert_equal false,   foo.done_documenting
    assert_equal false,   foo.dont_rename_initialize
    assert_equal false,   foo.force_documentation
    assert_equal nil,     foo.is_alias_for
    assert_equal '',      foo.params
    assert_equal klass,   foo.parent
    assert_equal false,   foo.singleton
    assert_equal 'add_my_method :foo', foo.text
    assert_equal nil,     foo.viewer
    assert_equal :public, foo.visibility
    assert_equal klass.current_section, foo.section

    stream = [
      tk(:COMMENT,    1, 1,  nil, "# File #{@top_level.absolute_name}, line 1"),
      RDoc::Parser::Ruby::NEWLINE_TOKEN,
      tk(:SPACE,      1, 1,  nil, ''),
      tk(:IDENTIFIER, 1, 0,  'add_my_method', 'add_my_method'),
      tk(:SPACE,      1, 13, nil, ' '),
      tk(:SYMBOL,     1, 14, nil, ':foo'),
      tk(:COMMA,      1, 18, nil, ','),
      tk(:SPACE,      1, 19, nil, ' '),
      tk(:SYMBOL,     1, 20, nil, ':bar'),
      tk(:NL,         1, 24, nil, "\n"),
    ]

    assert_equal stream, foo.token_stream
  end

  def test_parse_meta_method_block
    klass = RDoc::NormalClass.new 'Foo'
    klass.parent = @top_level

    comment = "##\n# my method\n"

    content = <<-CONTENT
inline(:my_method) do |*args|
  "this method causes z to disappear"
end
    CONTENT

    util_parser content

    tk = @parser.get_tk

    @parser.parse_meta_method klass, RDoc::Parser::Ruby::NORMAL, tk, comment

    assert_nil @parser.get_tk
  end

  def test_parse_meta_method_name
    klass = RDoc::NormalClass.new 'Foo'
    klass.parent = @top_level

    comment = "##\n# :method: woo_hoo!\n# my method\n"

    util_parser "add_my_method :foo, :bar\nadd_my_method :baz"

    tk = @parser.get_tk

    @parser.parse_meta_method klass, RDoc::Parser::Ruby::NORMAL, tk, comment

    foo = klass.method_list.first
    assert_equal 'woo_hoo!',  foo.name
    assert_equal 'my method', foo.comment
    assert_equal @top_level,  foo.file
  end

  def test_parse_meta_method_singleton
    klass = RDoc::NormalClass.new 'Foo'
    klass.parent = @top_level

    comment = "##\n# :singleton-method:\n# my method\n"

    util_parser "add_my_method :foo, :bar\nadd_my_method :baz"

    tk = @parser.get_tk

    @parser.parse_meta_method klass, RDoc::Parser::Ruby::NORMAL, tk, comment

    foo = klass.method_list.first
    assert_equal 'foo', foo.name
    assert_equal true, foo.singleton, 'singleton method'
    assert_equal 'my method', foo.comment
    assert_equal @top_level,  foo.file
  end

  def test_parse_meta_method_singleton_name
    klass = RDoc::NormalClass.new 'Foo'
    klass.parent = @top_level

    comment = "##\n# :singleton-method: woo_hoo!\n# my method\n"

    util_parser "add_my_method :foo, :bar\nadd_my_method :baz"

    tk = @parser.get_tk

    @parser.parse_meta_method klass, RDoc::Parser::Ruby::NORMAL, tk, comment

    foo = klass.method_list.first
    assert_equal 'woo_hoo!', foo.name
    assert_equal true, foo.singleton, 'singleton method'
    assert_equal 'my method', foo.comment
    assert_equal @top_level,  foo.file
  end

  def test_parse_meta_method_string_name
    klass = RDoc::NormalClass.new 'Foo'
    comment = "##\n# my method\n"

    util_parser "add_my_method 'foo'"

    tk = @parser.get_tk

    @parser.parse_meta_method klass, RDoc::Parser::Ruby::NORMAL, tk, comment

    foo = klass.method_list.first
    assert_equal 'foo', foo.name
    assert_equal 'my method', foo.comment
    assert_equal @top_level,  foo.file
  end

  def test_parse_meta_method_stopdoc
    klass = RDoc::NormalClass.new 'Foo'
    klass.parent = @top_level
    klass.stop_doc

    comment = "##\n# my method\n"

    util_parser "add_my_method :foo, :bar\nadd_my_method :baz"

    tk = @parser.get_tk

    @parser.parse_meta_method klass, RDoc::Parser::Ruby::NORMAL, tk, comment

    assert_empty klass.method_list
  end

  def test_parse_meta_method_unknown
    klass = RDoc::NormalClass.new 'Foo'
    comment = "##\n# my method\n"

    util_parser "add_my_method ('foo')"

    tk = @parser.get_tk

    @parser.parse_meta_method klass, RDoc::Parser::Ruby::NORMAL, tk, comment

    foo = klass.method_list.first
    assert_equal 'unknown', foo.name
    assert_equal 'my method', foo.comment
    assert_equal @top_level,  foo.file
  end

  def test_parse_method
    klass = RDoc::NormalClass.new 'Foo'
    klass.parent = @top_level

    comment = "##\n# my method\n"

    util_parser "def foo() :bar end"

    tk = @parser.get_tk

    @parser.parse_method klass, RDoc::Parser::Ruby::NORMAL, tk, comment

    foo = klass.method_list.first
    assert_equal 'foo',       foo.name
    assert_equal 'my method', foo.comment
    assert_equal @top_level,  foo.file
    assert_equal 0,           foo.offset
    assert_equal 1,           foo.line

    assert_equal [],        foo.aliases
    assert_equal nil,       foo.block_params
    assert_equal nil,       foo.call_seq
    assert_equal nil,       foo.is_alias_for
    assert_equal nil,       foo.viewer
    assert_equal true,      foo.document_children
    assert_equal true,      foo.document_self
    assert_equal '()',      foo.params
    assert_equal false,     foo.done_documenting
    assert_equal false,     foo.dont_rename_initialize
    assert_equal false,     foo.force_documentation
    assert_equal klass,     foo.parent
    assert_equal false,     foo.singleton
    assert_equal :public,   foo.visibility
    assert_equal 'def foo', foo.text
    assert_equal klass.current_section, foo.section

    stream = [
      tk(:COMMENT,    1, 1,  nil, "# File #{@top_level.absolute_name}, line 1"),
      RDoc::Parser::Ruby::NEWLINE_TOKEN,
      tk(:SPACE,      1, 1,  nil,   ''),
      tk(:DEF,        1, 0,  'def', 'def'),
      tk(:SPACE,      1, 3,  nil,   ' '),
      tk(:IDENTIFIER, 1, 4,  'foo', 'foo'),
      tk(:LPAREN,     1, 7,  nil,   '('),
      tk(:RPAREN,     1, 8,  nil,   ')'),
      tk(:SPACE,      1, 9,  nil,   ' '),
      tk(:COLON,      1, 10, nil,   ':'),
      tk(:IDENTIFIER, 1, 11, 'bar', 'bar'),
      tk(:SPACE,      1, 14, nil,   ' '),
      tk(:END,        1, 15, 'end', 'end'),
    ]

    assert_equal stream, foo.token_stream
  end

  def test_parse_method_alias
    klass = RDoc::NormalClass.new 'Foo'
    klass.parent = @top_level

    util_parser "def m() alias a b; end"

    tk = @parser.get_tk

    @parser.parse_method klass, RDoc::Parser::Ruby::NORMAL, tk, ''

    assert klass.aliases.empty?
  end

  def test_parse_method_false
    util_parser "def false.foo() :bar end"

    tk = @parser.get_tk

    @parser.parse_method @top_level, RDoc::Parser::Ruby::NORMAL, tk, ''

    klass = RDoc::TopLevel.find_class_named 'FalseClass'

    foo = klass.method_list.first
    assert_equal 'foo', foo.name
  end

  def test_parse_method_funky
    klass = RDoc::NormalClass.new 'Foo'
    klass.parent = @top_level

    util_parser "def (blah).foo() :bar end"

    tk = @parser.get_tk

    @parser.parse_method klass, RDoc::Parser::Ruby::NORMAL, tk, ''

    assert klass.method_list.empty?
  end

  def test_parse_method_gvar
    util_parser "def $stdout.foo() :bar end"

    tk = @parser.get_tk

    @parser.parse_method @top_level, RDoc::Parser::Ruby::NORMAL, tk, ''

    assert @top_level.method_list.empty?
  end

  def test_parse_method_internal_gvar
    klass = RDoc::NormalClass.new 'Foo'
    klass.parent = @top_level

    util_parser "def foo() def $blah.bar() end end"

    tk = @parser.get_tk

    @parser.parse_method klass, RDoc::Parser::Ruby::NORMAL, tk, ''

    assert_equal 1, klass.method_list.length
  end

  def test_parse_method_internal_ivar
    klass = RDoc::NormalClass.new 'Foo'
    klass.parent = @top_level

    util_parser "def foo() def @blah.bar() end end"

    tk = @parser.get_tk

    @parser.parse_method klass, RDoc::Parser::Ruby::NORMAL, tk, ''

    assert_equal 1, klass.method_list.length
  end

  def test_parse_method_internal_lvar
    klass = RDoc::NormalClass.new 'Foo'
    klass.parent = @top_level

    util_parser "def foo() def blah.bar() end end"

    tk = @parser.get_tk

    @parser.parse_method klass, RDoc::Parser::Ruby::NORMAL, tk, ''

    assert_equal 1, klass.method_list.length
  end

  def test_parse_method_nil
    util_parser "def nil.foo() :bar end"

    tk = @parser.get_tk

    @parser.parse_method @top_level, RDoc::Parser::Ruby::NORMAL, tk, ''

    klass = RDoc::TopLevel.find_class_named 'NilClass'

    foo = klass.method_list.first
    assert_equal 'foo', foo.name
  end

  def test_parse_method_no_parens
    klass = RDoc::NormalClass.new 'Foo'
    klass.parent = @top_level

    util_parser "def foo arg1, arg2 = {}\nend"

    tk = @parser.get_tk

    @parser.parse_method klass, RDoc::Parser::Ruby::NORMAL, tk, ''

    foo = klass.method_list.first
    assert_equal '(arg1, arg2 = {})', foo.params
    assert_equal @top_level, foo.file
  end

  def test_parse_method_parameters_comment
    klass = RDoc::NormalClass.new 'Foo'
    klass.parent = @top_level

    util_parser "def foo arg1, arg2 # some useful comment\nend"

    tk = @parser.get_tk

    @parser.parse_method klass, RDoc::Parser::Ruby::NORMAL, tk, ''

    foo = klass.method_list.first
    assert_equal '(arg1, arg2)', foo.params
  end

  def test_parse_method_parameters_comment_continue
    klass = RDoc::NormalClass.new 'Foo'
    klass.parent = @top_level

    util_parser "def foo arg1, arg2, # some useful comment\narg3\nend"

    tk = @parser.get_tk

    @parser.parse_method klass, RDoc::Parser::Ruby::NORMAL, tk, ''

    foo = klass.method_list.first
    assert_equal '(arg1, arg2, arg3)', foo.params
  end

  def test_parse_method_stopdoc
    klass = RDoc::NormalClass.new 'Foo'
    klass.parent = @top_level
    klass.stop_doc

    comment = "##\n# my method\n"

    util_parser "def foo() :bar end"

    tk = @parser.get_tk

    @parser.parse_method klass, RDoc::Parser::Ruby::NORMAL, tk, comment

    assert_empty klass.method_list
  end

  def test_parse_method_toplevel
    klass = @top_level

    util_parser "def foo arg1, arg2\nend"

    tk = @parser.get_tk

    @parser.parse_method klass, RDoc::Parser::Ruby::NORMAL, tk, ''

    object = RDoc::TopLevel.find_class_named 'Object'

    foo = object.method_list.first
    assert_equal 'Object#foo', foo.full_name
    assert_equal @top_level, foo.file
  end

  def test_parse_method_toplevel_class
    klass = @top_level

    util_parser "def Object.foo arg1, arg2\nend"

    tk = @parser.get_tk

    @parser.parse_method klass, RDoc::Parser::Ruby::NORMAL, tk, ''

    object = RDoc::TopLevel.find_class_named 'Object'

    foo = object.method_list.first
    assert_equal 'Object::foo', foo.full_name
  end

  def test_parse_method_true
    util_parser "def true.foo() :bar end"

    tk = @parser.get_tk

    @parser.parse_method @top_level, RDoc::Parser::Ruby::NORMAL, tk, ''

    klass = RDoc::TopLevel.find_class_named 'TrueClass'

    foo = klass.method_list.first
    assert_equal 'foo', foo.name
  end

  def test_parse_method_utf8
    klass = RDoc::NormalClass.new 'Foo'
    klass.parent = @top_level

    method = "def ω() end"

    assert_equal Encoding::UTF_8, method.encoding if
      Object.const_defined? :Encoding

    util_parser method

    tk = @parser.get_tk

    @parser.parse_method klass, RDoc::Parser::Ruby::NORMAL, tk, ''

    omega = klass.method_list.first
    assert_equal "def \317\211", omega.text
  end

  def test_parse_statements_class_if
    util_parser <<-CODE
module Foo
  X = if TRUE then
        ''
      end

  def blah
  end
end
    CODE

    @parser.parse_statements @top_level, RDoc::Parser::Ruby::NORMAL, nil, ''

    foo = @top_level.modules.first
    assert_equal 'Foo', foo.full_name, 'module Foo'

    methods = foo.method_list
    assert_equal 1, methods.length
    assert_equal 'Foo#blah', methods.first.full_name
  end

  def test_parse_statements_class_nested
    comment = "##\n# my method\n"

    util_parser "module Foo\n#{comment}class Bar\nend\nend"

    @parser.parse_statements @top_level, RDoc::Parser::Ruby::NORMAL, nil, ''

    foo = @top_level.modules.first
    assert_equal 'Foo', foo.full_name, 'module Foo'

    bar = foo.classes.first
    assert_equal 'Foo::Bar', bar.full_name, 'class Foo::Bar'
    assert_equal 'my method', bar.comment
  end

  def test_parse_statements_encoding
    skip "Encoding not implemented" unless Object.const_defined? :Encoding
    @options.encoding = Encoding::CP852

    content = <<-EOF
class Foo
  ##
  # this is my method
  add_my_method :foo
end
    EOF

    util_parser content

    @parser.parse_statements @top_level

    foo = @top_level.classes.first.method_list.first
    assert_equal 'foo', foo.name
    assert_equal 'this is my method', foo.comment
    assert_equal Encoding::CP852, foo.comment.encoding
  end

  def test_parse_statements_identifier_meta_method
    content = <<-EOF
class Foo
  ##
  # this is my method
  add_my_method :foo
end
    EOF

    util_parser content

    @parser.parse_statements @top_level

    foo = @top_level.classes.first.method_list.first
    assert_equal 'foo', foo.name
  end

  def test_parse_statements_identifier_alias_method
    content = <<-RUBY
class Foo
  def foo() end
  alias_method :foo2, :foo
end
    RUBY

    util_parser content

    @parser.parse_statements @top_level

    foo = @top_level.classes.first.method_list[0]
    assert_equal 'foo', foo.name

    foo2 = @top_level.classes.first.method_list.last
    assert_equal 'foo2', foo2.name
    assert_equal 'foo', foo2.is_alias_for.name
    assert @top_level.classes.first.aliases.empty?
  end

  def test_parse_statements_identifier_alias_method_before_original_method
    # This is not strictly legal Ruby code, but it simulates finding an alias
    # for a method before finding the original method, which might happen
    # to rdoc if the alias is in a different file than the original method
    # and rdoc processes the alias' file first.
    content = <<-EOF
class Foo
  alias_method :foo2, :foo

  alias_method :foo3, :foo

  def foo()
  end

  alias_method :foo4, :foo

  alias_method :foo5, :unknown
end
EOF

    util_parser content

    @parser.parse_statements @top_level

    foo = @top_level.classes.first.method_list[0]
    assert_equal 'foo', foo.name

    foo2 = @top_level.classes.first.method_list[1]
    assert_equal 'foo2', foo2.name
    assert_equal 'foo', foo2.is_alias_for.name

    foo3 = @top_level.classes.first.method_list[2]
    assert_equal 'foo3', foo3.name
    assert_equal 'foo', foo3.is_alias_for.name

    foo4 = @top_level.classes.first.method_list.last
    assert_equal 'foo4', foo4.name
    assert_equal 'foo', foo4.is_alias_for.name

    assert_equal 'unknown', @top_level.classes.first.external_aliases[0].old_name
  end

  def test_parse_statements_identifier_constant

    sixth_constant = <<-EOF
Class.new do
  rule :file do
    all(x, y, z) {
      def value
        find(:require).each {|r| require r.value }
        find(:grammar).map {|g| g.value }
      end
      def min; end
    }
  end
end
    EOF

    content = <<-EOF
class Foo
  FIRST_CONSTANT = 5

  SECOND_CONSTANT = [
     1,
     2,
     3
  ]

  THIRD_CONSTANT = {
     :foo => 'bar',
     :x => 'y'
  }

  FOURTH_CONSTANT = SECOND_CONSTANT.map do |element|
    element + 1
    element + 2
  end

  FIFTH_CONSTANT = SECOND_CONSTANT.map { |element| element + 1 }

  SIXTH_CONSTANT = #{sixth_constant}

  SEVENTH_CONSTANT = proc { |i| begin i end }
end
EOF

    util_parser content

    @parser.parse_statements @top_level

    constants = @top_level.classes.first.constants

    constant = constants[0]
    assert_equal 'FIRST_CONSTANT', constant.name
    assert_equal '5', constant.value
    assert_equal @top_level, constant.file

    constant = constants[1]
    assert_equal 'SECOND_CONSTANT', constant.name
    assert_equal "[\n1,\n2,\n3\n]", constant.value
    assert_equal @top_level, constant.file

    constant = constants[2]
    assert_equal 'THIRD_CONSTANT', constant.name
    assert_equal "{\n:foo => 'bar',\n:x => 'y'\n}", constant.value
    assert_equal @top_level, constant.file

    constant = constants[3]
    assert_equal 'FOURTH_CONSTANT', constant.name
    assert_equal "SECOND_CONSTANT.map do |element|\nelement + 1\nelement + 2\nend", constant.value
    assert_equal @top_level, constant.file

    constant = constants[4]
    assert_equal 'FIFTH_CONSTANT', constant.name
    assert_equal 'SECOND_CONSTANT.map { |element| element + 1 }', constant.value
    assert_equal @top_level, constant.file

    # TODO: parse as class
    constant = constants[5]
    assert_equal 'SIXTH_CONSTANT', constant.name
    assert_equal sixth_constant.lines.map(&:strip).join("\n"), constant.value
    assert_equal @top_level, constant.file

    # TODO: parse as method
    constant = constants[6]
    assert_equal 'SEVENTH_CONSTANT', constant.name
    assert_equal "proc { |i| begin i end }", constant.value
    assert_equal @top_level, constant.file
  end

  def test_parse_statements_identifier_attr
    content = "class Foo\nattr :foo\nend"

    util_parser content

    @parser.parse_statements @top_level

    foo = @top_level.classes.first.attributes.first
    assert_equal 'foo', foo.name
    assert_equal 'R', foo.rw
  end

  def test_parse_statements_identifier_attr_accessor
    content = "class Foo\nattr_accessor :foo\nend"

    util_parser content

    @parser.parse_statements @top_level

    foo = @top_level.classes.first.attributes.first
    assert_equal 'foo', foo.name
    assert_equal 'RW', foo.rw
  end

  def test_parse_statements_identifier_include
    content = "class Foo\ninclude Bar\nend"

    util_parser content

    @parser.parse_statements @top_level

    foo = @top_level.classes.first
    assert_equal 'Foo', foo.name
    assert_equal 1, foo.includes.length
  end

  def test_parse_statements_identifier_module_function
    content = "module Foo\ndef foo() end\nmodule_function :foo\nend"

    util_parser content

    @parser.parse_statements @top_level

    foo, s_foo = @top_level.modules.first.method_list
    assert_equal 'foo',    foo.name,       'instance method name'
    assert_equal :private, foo.visibility, 'instance method visibility'
    assert_equal false,    foo.singleton,  'instance method singleton'

    assert_equal 'foo',   s_foo.name,       'module function name'
    assert_equal :public, s_foo.visibility, 'module function visibility'
    assert_equal true,    s_foo.singleton,  'module function singleton'
  end

  def test_parse_statements_identifier_private
    content = "class Foo\nprivate\ndef foo() end\nend"

    util_parser content

    @parser.parse_statements @top_level

    foo = @top_level.classes.first.method_list.first
    assert_equal 'foo', foo.name
    assert_equal :private, foo.visibility
  end

  def test_parse_statements_identifier_public_class_method
    content = <<-CONTENT
class Date
  def self.now; end
  private_class_method :now
end

class DateTime < Date
  public_class_method :now
end
    CONTENT

    util_parser content

    @parser.parse_statements @top_level

    date, date_time = @top_level.classes

    date_now      = date.method_list.first
    date_time_now = date_time.method_list.first

    assert_equal :private, date_now.visibility
    assert_equal :public,  date_time_now.visibility
  end

  def test_parse_statements_identifier_private_class_method
    content = <<-CONTENT
class Date
  def self.now; end
  public_class_method :now
end

class DateTime < Date
  private_class_method :now
end
    CONTENT

    util_parser content

    @parser.parse_statements @top_level

    date, date_time = @top_level.classes

    date_now      = date.method_list.first
    date_time_now = date_time.method_list.first

    assert_equal :public,  date_now.visibility,      date_now.full_name
    assert_equal :private, date_time_now.visibility, date_time_now.full_name
  end

  def test_parse_statements_identifier_require
    content = "require 'bar'"

    util_parser content

    @parser.parse_statements @top_level

    assert_equal 1, @top_level.requires.length
  end

  def test_parse_statements_stopdoc_TkALIAS
    util_top_level

    klass = @top_level.add_class RDoc::NormalClass, 'Foo'

    util_parser "\n# :stopdoc:\nalias old new"

    @parser.parse_statements klass, RDoc::Parser::Ruby::NORMAL, nil

    assert_empty klass.aliases
    assert_empty klass.unmatched_alias_lists
  end

  def test_parse_statements_stopdoc_TkIDENTIFIER_alias_method
    util_top_level

    klass = @top_level.add_class RDoc::NormalClass, 'Foo'

    util_parser "\n# :stopdoc:\nalias_method :old :new"

    @parser.parse_statements klass, RDoc::Parser::Ruby::NORMAL, nil

    assert_empty klass.aliases
    assert_empty klass.unmatched_alias_lists
  end

  def test_parse_statements_stopdoc_TkIDENTIFIER_metaprogrammed
    util_top_level

    klass = @top_level.add_class RDoc::NormalClass, 'Foo'

    util_parser "\n# :stopdoc:\n# attr :meta"

    @parser.parse_statements klass, RDoc::Parser::Ruby::NORMAL, nil

    assert_empty klass.method_list
    assert_empty klass.attributes
  end

  def test_parse_statements_stopdoc_TkCONSTANT
    util_top_level

    klass = @top_level.add_class RDoc::NormalClass, 'Foo'

    util_parser "\n# :stopdoc:\nA = v"

    @parser.parse_statements klass, RDoc::Parser::Ruby::NORMAL, nil

    assert_empty klass.constants
  end

  def test_parse_statements_stopdoc_TkDEF
    util_top_level

    klass = @top_level.add_class RDoc::NormalClass, 'Foo'

    util_parser "\n# :stopdoc:\ndef m\n end"

    @parser.parse_statements klass, RDoc::Parser::Ruby::NORMAL, nil

    assert_empty klass.method_list
  end

  def test_parse_statements_while_begin
    util_parser <<-RUBY
class A
  def a
    while begin a; b end
    end
  end

  def b
  end
end
    RUBY

    @parser.parse_statements @top_level

    c_a = @top_level.classes.first
    assert_equal 'A', c_a.full_name

    assert_equal 1, @top_level.classes.length

    m_a = c_a.method_list.first
    m_b = c_a.method_list.last

    assert_equal 'A#a', m_a.full_name
    assert_equal 'A#b', m_b.full_name
  end

  def test_parse_symbol_in_arg
    util_parser ':blah "blah" "#{blah}" blah'

    assert_equal 'blah', @parser.parse_symbol_in_arg

    @parser.skip_tkspace

    assert_equal 'blah', @parser.parse_symbol_in_arg

    @parser.skip_tkspace

    assert_equal nil, @parser.parse_symbol_in_arg

    @parser.skip_tkspace

    assert_equal nil, @parser.parse_symbol_in_arg
  end

  def test_parse_statements_alias_method
    content = <<-CONTENT
class A
  alias_method :a, :[] unless c
end

B = A

class C
end
    CONTENT

    util_parser content

    @parser.parse_statements @top_level

    # HACK where are the assertions?
  end

  def test_parse_top_level_statements_stopdoc
    @top_level.stop_doc
    content = "# this is the top-level comment"

    util_parser content

    @parser.parse_top_level_statements @top_level

    assert_empty @top_level.comment
  end

  def test_parse_top_level_statements_stopdoc_integration
    content = <<-CONTENT
# :stopdoc:

class Example
  def method_name
  end
end
    CONTENT

    util_parser content

    @parser.parse_top_level_statements @top_level

    assert_equal 1, @top_level.classes.length
    assert_empty @top_level.modules

    assert @top_level.find_module_named('Example').ignored?
  end

  # This tests parse_comment
  def test_parse_top_level_statements_constant_nodoc_integration
    content = <<-CONTENT
class A
  C = A # :nodoc:
end
    CONTENT

    util_parser content

    @parser.parse_top_level_statements @top_level

    klass = @top_level.find_module_named('A')

    c = klass.constants.first

    assert_nil c.document_self, 'C should not be documented'
  end

  def test_parse_yield_in_braces_with_parens
    klass = RDoc::NormalClass.new 'Foo'
    klass.parent = @top_level

    util_parser "def foo\nn.times { |i| yield nth(i) }\nend"

    tk = @parser.get_tk

    @parser.parse_method klass, RDoc::Parser::Ruby::NORMAL, tk, ''

    foo = klass.method_list.first
    assert_equal 'nth(i)', foo.block_params
  end

  def test_sanity_integer
    util_parser '1'
    assert_equal '1', @parser.get_tk.text

    util_parser '1.0'
    assert_equal '1.0', @parser.get_tk.text
  end

  def test_sanity_interpolation
    last_tk = nil
    util_parser 'class A; B = "#{c}"; end'

    while tk = @parser.get_tk do last_tk = tk end

    assert_equal "\n", last_tk.text
  end

  # If you're writing code like this you're doing it wrong

  def test_sanity_interpolation_crazy
    util_parser '"#{"#{"a")}" if b}"'

    assert_equal '"#{"#{"a")}" if b}"', @parser.get_tk.text
    assert_equal RDoc::RubyToken::TkNL, @parser.get_tk.class
  end

  def test_sanity_interpolation_curly
    util_parser '%{ #{} }'

    assert_equal '%{ #{} }', @parser.get_tk.text
    assert_equal RDoc::RubyToken::TkNL, @parser.get_tk.class
  end

  def test_sanity_interpolation_format
    util_parser '"#{stftime("%m-%d")}"'

    while @parser.get_tk do end
  end

  def test_sanity_symbol_interpolation
    util_parser ':"#{bar}="'

    while @parser.get_tk do end
  end

  def test_scan_cr
    content = <<-CONTENT
class C\r
  def m\r
    a=\\\r
      123\r
  end\r
end\r
    CONTENT

    util_parser content

    @parser.scan

    c = @top_level.classes.first

    assert_equal 1, c.method_list.length
  end

  def test_scan_block_comment
    content = <<-CONTENT
=begin rdoc
Foo comment
=end

class Foo

=begin
m comment
=end

  def m() end
end
    CONTENT

    util_parser content

    @parser.scan

    foo = @top_level.classes.first

    assert_equal 'Foo comment', foo.comment

    m = foo.method_list.first

    assert_equal 'm comment', m.comment
  end

  def test_scan_block_comment_nested # Issue #41
    content = <<-CONTENT
require 'something'
=begin rdoc
findmeindoc
=end
module Foo
    class Bar
    end
end
    CONTENT

    util_parser content

    @parser.scan

    foo = @top_level.modules.first

    assert_equal 'Foo', foo.full_name
    assert_equal 'findmeindoc', foo.comment

    bar = foo.classes.first

    assert_equal 'Foo::Bar', bar.full_name
    assert_equal '', bar.comment
  end

  def test_scan_block_comment_notflush
  ##
  #
  # The previous test assumes that between the =begin/=end blocs that there is
  # only one line, or minima formatting directives. This test tests for those
  # who use the =begin bloc with longer / more advanced formatting within.
  #
  ##
    content = <<-CONTENT
=begin rdoc

= DESCRIPTION

This is a simple test class

= RUMPUS

Is a silly word

=end
class StevenSimpleClass
  # A band on my iPhone as I wrote this test
  FRUIT_BATS="Make nice music"

=begin rdoc
A nice girl
=end

  def lauren
    puts "Summoning Lauren!"
  end
end
    CONTENT
    util_parser content

    @parser.scan

    foo = @top_level.classes.first

    assert_equal "= DESCRIPTION\n\nThis is a simple test class\n\n= RUMPUS\n\nIs a silly word",
      foo.comment

    m = foo.method_list.first

    assert_equal 'A nice girl', m.comment
  end

  def test_scan_meta_method_block
    content = <<-CONTENT
class C

  ##
  #  my method

  inline(:my_method) do |*args|
    "this method used to cause z to disappear"
  end

  def z
  end
    CONTENT

    util_parser content

    @parser.scan

    assert_equal 2, @top_level.classes.first.method_list.length
  end

  def test_stopdoc_after_comment
    util_parser <<-EOS
      module Bar
        # hello
        module Foo
          # :stopdoc:
        end
        # there
        class Baz
          # :stopdoc:
        end
      end
    EOS

    @parser.parse_statements @top_level

    foo = @top_level.modules.first.modules.first
    assert_equal 'Foo', foo.name
    assert_equal 'hello', foo.comment

    baz = @top_level.modules.first.classes.first
    assert_equal 'Baz', baz.name
    assert_equal 'there', baz.comment
  end

  def tk(klass, line, char, name, text)
    klass = RDoc::RubyToken.const_get "Tk#{klass.to_s.upcase}"

    token = if klass.instance_method(:initialize).arity == 3 then
              raise ArgumentError, "name not used for #{klass}" unless name.nil?
              klass.new nil, line, char
            else
              klass.new nil, line, char, name
            end

    token.set_text text

    token
  end

  def util_parser(content)
    @parser = RDoc::Parser::Ruby.new @top_level, @filename, content, @options,
                                     @stats
  end

  def util_two_parsers(first_file_content, second_file_content)
    util_parser first_file_content

    @parser2 = RDoc::Parser::Ruby.new @top_level2, @filename,
                                      second_file_content, @options, @stats
  end

  def util_top_level
    RDoc::TopLevel.reset
    @top_level = RDoc::TopLevel.new @filename
    @top_level2 = RDoc::TopLevel.new @filename2
  end

end

