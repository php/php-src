require 'stringio'
require 'tempfile'
require 'rubygems'
require 'minitest/autorun'
require 'rdoc/options'
require 'rdoc/parser/c'

=begin
  TODO: test call-seq parsing

/*
 *  call-seq:
 *     ARGF.readlines(sep=$/)     -> array
 *     ARGF.readlines(limit)      -> array
 *     ARGF.readlines(sep, limit) -> array
 *
 *     ARGF.to_a(sep=$/)     -> array
 *     ARGF.to_a(limit)      -> array
 *     ARGF.to_a(sep, limit) -> array
 *
 *  Reads +ARGF+'s current file in its entirety, returning an +Array+ of its
 *  lines, one line per element. Lines are assumed to be separated by _sep_.
 *
 *     lines = ARGF.readlines
 *     lines[0]                #=> "This is line one\n"
 */

assert call-seq did not stop at first empty line

/*
 * call-seq:
 *
 *  flt ** other  ->  float
 *
 * Raises <code>float</code> the <code>other</code> power.
 *
 *    2.0**3      #=> 8.0
 */

assert call-seq correct (bug: was empty)

/* call-seq: flt ** other  ->  float */

assert call-seq correct

=end

class RDoc::Parser::C
  attr_accessor :classes

  public :do_classes, :do_constants
end

class TestRDocParserC < MiniTest::Unit::TestCase

  def setup
    @tempfile = Tempfile.new self.class.name
    filename = @tempfile.path

    @top_level = RDoc::TopLevel.new filename
    @fn = filename
    @options = RDoc::Options.new
    @stats = RDoc::Stats.new 0

    RDoc::Parser::C.reset
    RDoc::TopLevel.reset
  end

  def teardown
    @tempfile.close
  end

  def test_class_can_parse
    c_parser = RDoc::Parser::C

    assert_equal c_parser, c_parser.can_parse('file.C')
    assert_equal c_parser, c_parser.can_parse('file.CC')
    assert_equal c_parser, c_parser.can_parse('file.H')
    assert_equal c_parser, c_parser.can_parse('file.HH')
    assert_equal c_parser, c_parser.can_parse('file.c')
    assert_equal c_parser, c_parser.can_parse('file.cc')
    assert_equal c_parser, c_parser.can_parse('file.cpp')
    assert_equal c_parser, c_parser.can_parse('file.cxx')
    assert_equal c_parser, c_parser.can_parse('file.h')
    assert_equal c_parser, c_parser.can_parse('file.hh')
    assert_equal c_parser, c_parser.can_parse('file.y')
  end

  def test_do_attr_rb_attr
    content = <<-EOF
void Init_Blah(void) {
  cBlah = rb_define_class("Blah", rb_cObject);

  /*
   * This is an accessor
   */
  rb_attr(cBlah, rb_intern("accessor"), 1, 1, Qfalse);

  /*
   * This is a reader
   */
  rb_attr(cBlah, rb_intern("reader"), 1, 0, Qfalse);

  /*
   * This is a writer
   */
  rb_attr(cBlah, rb_intern("writer"), 0, 1, Qfalse);
}
    EOF

    klass = util_get_class content, 'cBlah'

    attrs = klass.attributes
    assert_equal 3, attrs.length, attrs.inspect

    accessor = attrs.shift
    assert_equal 'accessor',            accessor.name
    assert_equal 'RW',                  accessor.rw
    assert_equal 'This is an accessor', accessor.comment
    assert_equal @top_level,            accessor.file

    reader = attrs.shift
    assert_equal 'reader',           reader.name
    assert_equal 'R',                reader.rw
    assert_equal 'This is a reader', reader.comment

    writer = attrs.shift
    assert_equal 'writer',           writer.name
    assert_equal 'W',                writer.rw
    assert_equal 'This is a writer', writer.comment
  end

  def test_do_attr_rb_define_attr
    content = <<-EOF
void Init_Blah(void) {
  cBlah = rb_define_class("Blah", rb_cObject);

  /*
   * This is an accessor
   */
  rb_define_attr(cBlah, "accessor", 1, 1);
}
    EOF

    klass = util_get_class content, 'cBlah'

    attrs = klass.attributes
    assert_equal 1, attrs.length, attrs.inspect

    accessor = attrs.shift
    assert_equal 'accessor',            accessor.name
    assert_equal 'RW',                  accessor.rw
    assert_equal 'This is an accessor', accessor.comment
    assert_equal @top_level,            accessor.file
  end

  def test_do_aliases
    content = <<-EOF
/*
 * This should show up as an alias with documentation
 */
VALUE blah(VALUE klass, VALUE year) {
}

void Init_Blah(void) {
  cDate = rb_define_class("Date", rb_cObject);

  rb_define_method(cDate, "blah", blah, 1);

  rb_define_alias(cDate, "bleh", "blah");
}
    EOF

    klass = util_get_class content, 'cDate'

    methods = klass.method_list
    assert_equal 2,      methods.length
    assert_equal 'bleh', methods.last.name
    assert_equal 'blah', methods.last.is_alias_for.name

    assert_equal @top_level, methods.last.is_alias_for.file
    assert_equal @top_level, methods.last.file
  end

  def test_do_aliases_singleton
    content = <<-EOF
/*
 * This should show up as a method with documentation
 */
VALUE blah(VALUE klass, VALUE year) {
}

void Init_Blah(void) {
  cDate = rb_define_class("Date", rb_cObject);
  sDate = rb_singleton_class(cDate);

  rb_define_method(sDate, "blah", blah, 1);

  /*
   * This should show up as an alias
   */
  rb_define_alias(sDate, "bleh", "blah");
}
    EOF

    klass = util_get_class content, 'cDate'

    methods = klass.method_list

    assert_equal 2,      methods.length
    assert_equal 'bleh', methods.last.name
    assert               methods.last.singleton
    assert_equal 'blah', methods.last.is_alias_for.name
    assert_equal 'This should show up as an alias', methods.last.comment
  end

  def test_do_classes_boot_class
    content = <<-EOF
/* Document-class: Foo
 * this is the Foo boot class
 */
VALUE cFoo = boot_defclass("Foo", rb_cObject);
    EOF

    klass = util_get_class content, 'cFoo'
    assert_equal "this is the Foo boot class", klass.comment
    assert_equal 'Object', klass.superclass
  end

  def test_do_classes_boot_class_nil
    content = <<-EOF
/* Document-class: Foo
 * this is the Foo boot class
 */
VALUE cFoo = boot_defclass("Foo", 0);
    EOF

    klass = util_get_class content, 'cFoo'
    assert_equal "this is the Foo boot class", klass.comment
    assert_equal nil, klass.superclass
  end

  def test_do_aliases_missing_class
    content = <<-EOF
void Init_Blah(void) {
  rb_define_alias(cDate, "b", "a");
}
    EOF

    _, err = capture_io do
      refute util_get_class(content, 'cDate')
    end

    assert_equal "Enclosing class/module \"cDate\" for alias b a not known\n",
                 err
  end

  def test_do_classes_class
    content = <<-EOF
/* Document-class: Foo
 * this is the Foo class
 */
VALUE cFoo = rb_define_class("Foo", rb_cObject);
    EOF

    klass = util_get_class content, 'cFoo'
    assert_equal "this is the Foo class", klass.comment
  end

  def test_do_classes_class_under
    content = <<-EOF
/* Document-class: Kernel::Foo
 * this is the Foo class under Kernel
 */
VALUE cFoo = rb_define_class_under(rb_mKernel, "Foo", rb_cObject);
    EOF

    klass = util_get_class content, 'cFoo'
    assert_equal 'Kernel::Foo', klass.full_name
    assert_equal "this is the Foo class under Kernel", klass.comment
  end

  def test_do_classes_class_under_rb_path2class
    content = <<-EOF
/* Document-class: Kernel::Foo
 * this is Kernel::Foo < A::B
 */
VALUE cFoo = rb_define_class_under(rb_mKernel, "Foo", rb_path2class("A::B"));
    EOF

    klass = util_get_class content, 'cFoo'

    assert_equal 'Kernel::Foo', klass.full_name
    assert_equal 'A::B', klass.superclass
    assert_equal 'this is Kernel::Foo < A::B', klass.comment
  end

  def test_do_classes_singleton
    content = <<-EOF
VALUE cFoo = rb_define_class("Foo", rb_cObject);
VALUE cFooS = rb_singleton_class(cFoo);
    EOF

    util_get_class content, 'cFooS'

    assert_equal 'Foo', @parser.singleton_classes['cFooS']
  end

  def test_do_classes_module
    content = <<-EOF
/* Document-module: Foo
 * this is the Foo module
 */
VALUE mFoo = rb_define_module("Foo");
    EOF

    klass = util_get_class content, 'mFoo'
    assert_equal "this is the Foo module", klass.comment
  end

  def test_do_classes_module_under
    content = <<-EOF
/* Document-module: Kernel::Foo
 * this is the Foo module under Kernel
 */
VALUE mFoo = rb_define_module_under(rb_mKernel, "Foo");
    EOF

    klass = util_get_class content, 'mFoo'
    assert_equal "this is the Foo module under Kernel", klass.comment
  end

  def test_do_constants
    content = <<-EOF
#include <ruby.h>

void Init_foo(){
   VALUE cFoo = rb_define_class("Foo", rb_cObject);

   /* 300: The highest possible score in bowling */
   rb_define_const(cFoo, "PERFECT", INT2FIX(300));

   /* Huzzah!: What you cheer when you roll a perfect game */
   rb_define_const(cFoo, "CHEER", rb_str_new2("Huzzah!"));

   /* TEST\:TEST: Checking to see if escaped colon works */
   rb_define_const(cFoo, "TEST", rb_str_new2("TEST:TEST"));

   /* \\: The file separator on MS Windows */
   rb_define_const(cFoo, "MSEPARATOR", rb_str_new2("\\"));

   /* /: The file separator on Unix */
   rb_define_const(cFoo, "SEPARATOR", rb_str_new2("/"));

   /* C:\\Program Files\\Stuff: A directory on MS Windows */
   rb_define_const(cFoo, "STUFF", rb_str_new2("C:\\Program Files\\Stuff"));

   /* Default definition */
   rb_define_const(cFoo, "NOSEMI", INT2FIX(99));

   rb_define_const(cFoo, "NOCOMMENT", rb_str_new2("No comment"));

   /*
    * Multiline comment goes here because this comment spans multiple lines.
    * Multiline comment goes here because this comment spans multiple lines.
    */
   rb_define_const(cFoo, "MULTILINE", INT2FIX(1));

   /*
    * 1: Multiline comment goes here because this comment spans multiple lines.
    * Multiline comment goes here because this comment spans multiple lines.
    */
   rb_define_const(cFoo, "MULTILINE_VALUE", INT2FIX(1));

   /* Multiline comment goes here because this comment spans multiple lines.
    * Multiline comment goes here because this comment spans multiple lines.
    */
   rb_define_const(cFoo, "MULTILINE_NOT_EMPTY", INT2FIX(1));

}
    EOF

    @parser = util_parser content

    @parser.do_classes
    @parser.do_constants

    klass = @parser.classes['cFoo']
    assert klass

    constants = klass.constants
    assert !klass.constants.empty?

    assert_equal @top_level, constants.first.file

    constants = constants.map { |c| [c.name, c.value, c.comment] }

    assert_equal ['PERFECT', '300', 'The highest possible score in bowling   '],
                 constants.shift
    assert_equal ['CHEER', 'Huzzah!',
                  'What you cheer when you roll a perfect game   '],
                 constants.shift
    assert_equal ['TEST', 'TEST:TEST',
                  'Checking to see if escaped colon works   '],
                 constants.shift
    assert_equal ['MSEPARATOR', '\\',
                  'The file separator on MS Windows   '],
                 constants.shift
    assert_equal ['SEPARATOR', '/',
                  'The file separator on Unix   '],
                 constants.shift
    assert_equal ['STUFF', 'C:\\Program Files\\Stuff',
                  'A directory on MS Windows   '],
                 constants.shift
    assert_equal ['NOSEMI', 'INT2FIX(99)',
                  'Default definition   '],
                 constants.shift
    assert_equal ['NOCOMMENT', 'rb_str_new2("No comment")', ''],
                 constants.shift

    comment = <<-EOF.chomp
Multiline comment goes here because this comment spans multiple lines.
Multiline comment goes here because this comment spans multiple lines.
    EOF
    assert_equal ['MULTILINE',           'INT2FIX(1)', comment], constants.shift
    assert_equal ['MULTILINE_VALUE',     '1',          comment], constants.shift
    assert_equal ['MULTILINE_NOT_EMPTY', 'INT2FIX(1)', comment], constants.shift

    assert constants.empty?, constants.inspect
  end

  def test_do_constants_curses
    content = <<-EOF
void Init_curses(){
  mCurses = rb_define_module("Curses");

  /*
   * Document-const: Curses::COLOR_BLACK
   *
   * Value of the color black
   */
  rb_curses_define_const(COLOR_BLACK);
}
    EOF

    @parser = util_parser content

    @parser.do_classes
    @parser.do_constants

    klass = @parser.classes['mCurses']

    constants = klass.constants
    refute_empty klass.constants

    assert_equal 'COLOR_BLACK', constants.first.name
    assert_equal 'UINT2NUM(COLOR_BLACK)', constants.first.value
    assert_equal 'Value of the color black', constants.first.comment
  end

  def test_do_includes
    content = <<-EOF
Init_foo() {
   VALUE cFoo = rb_define_class("Foo", rb_cObject);
   VALUE mInc = rb_define_module("Inc");

   rb_include_module(cFoo, mInc);
}
    EOF

    klass = util_get_class content, 'cFoo'

    incl = klass.includes.first
    assert_equal 'Inc',      incl.name
    assert_equal '',         incl.comment
    assert_equal @top_level, incl.file
  end

  # HACK parsing warning instead of setting up in file
  def test_do_methods_in_c
    content = <<-EOF
VALUE blah(VALUE klass, VALUE year) {
}

void Init_Blah(void) {
  cDate = rb_define_class("Date", rb_cObject);

  rb_define_method(cDate, "blah", blah, 1); /* in blah.c */
}
    EOF

    klass = nil

    _, err = capture_io do
      klass = util_get_class content, 'cDate'
    end

    assert_match ' blah.c ', err
  end

  # HACK parsing warning instead of setting up in file
  def test_do_methods_in_cpp
    content = <<-EOF
VALUE blah(VALUE klass, VALUE year) {
}

void Init_Blah(void) {
  cDate = rb_define_class("Date", rb_cObject);

  rb_define_method(cDate, "blah", blah, 1); /* in blah.cpp */
}
    EOF

    klass = nil

    _, err = capture_io do
      klass = util_get_class content, 'cDate'
    end

    assert_match ' blah.cpp ', err
  end

  # HACK parsing warning instead of setting up in file
  def test_do_methods_in_y
    content = <<-EOF
VALUE blah(VALUE klass, VALUE year) {
}

void Init_Blah(void) {
  cDate = rb_define_class("Date", rb_cObject);

  rb_define_method(cDate, "blah", blah, 1); /* in blah.y */
}
    EOF

    klass = nil

    _, err = capture_io do
      klass = util_get_class content, 'cDate'
    end

    assert_match ' blah.y ', err
  end

  def test_do_methods_singleton_class
    content = <<-EOF
VALUE blah(VALUE klass, VALUE year) {
}

void Init_Blah(void) {
  cDate = rb_define_class("Date", rb_cObject);
  sDate = rb_singleton_class(cDate);

  rb_define_method(sDate, "blah", blah, 1);
}
    EOF

    klass = util_get_class content, 'cDate'

    methods = klass.method_list
    assert_equal 1,      methods.length
    assert_equal 'blah', methods.first.name
    assert               methods.first.singleton
  end

  def test_find_alias_comment
    parser = util_parser ''

    comment = parser.find_alias_comment 'C', '[]', 'index'

    assert_equal '', comment

    parser = util_parser <<-C
/*
 * comment
 */

rb_define_alias(C, "[]", "index");
    C

    comment = parser.find_alias_comment 'C', '[]', 'index'

    assert_equal "/*\n * comment\n */\n\n", comment
  end

  def test_find_class_comment_include
    @options.rdoc_include << File.dirname(__FILE__)

    content = <<-EOF
/*
 * a comment for class Foo
 *
 * :include: test.txt
 */
void
Init_Foo(void) {
  VALUE foo = rb_define_class("Foo", rb_cObject);
}
    EOF

    klass = util_get_class content, 'foo'

    assert_equal "a comment for class Foo\n\ntest file", klass.comment
  end

  def test_find_class_comment_init
    content = <<-EOF
/*
 * a comment for class Foo
 */
void
Init_Foo(void) {
  VALUE foo = rb_define_class("Foo", rb_cObject);
}
    EOF

    klass = util_get_class content, 'foo'

    assert_equal "a comment for class Foo", klass.comment
  end

  def test_find_class_comment_define_class
    content = <<-EOF
/*
 * a comment for class Foo
 */
VALUE foo = rb_define_class("Foo", rb_cObject);
    EOF

    klass = util_get_class content, 'foo'

    assert_equal "a comment for class Foo", klass.comment
  end

  def test_find_class_comment_define_class_Init_Foo
    content = <<-EOF
/*
 * a comment for class Foo on Init
 */
void
Init_Foo(void) {
    /*
     * a comment for class Foo on rb_define_class
     */
    VALUE foo = rb_define_class("Foo", rb_cObject);
}
    EOF

    klass = util_get_class content, 'foo'

    assert_equal "a comment for class Foo on Init", klass.comment
  end

  def test_find_class_comment_define_class_Init_Foo_no_void
    content = <<-EOF
/*
 * a comment for class Foo on Init
 */
void
Init_Foo() {
    /*
     * a comment for class Foo on rb_define_class
     */
    VALUE foo = rb_define_class("Foo", rb_cObject);
}
    EOF

    klass = util_get_class content, 'foo'

    assert_equal "a comment for class Foo on Init", klass.comment
  end

  def test_find_class_comment_define_class_bogus_comment
    content = <<-EOF
/*
 * a comment for other_function
 */
void
other_function() {
}

void
Init_Foo(void) {
    VALUE foo = rb_define_class("Foo", rb_cObject);
}
    EOF

    klass = util_get_class content, 'foo'

    assert_equal '', klass.comment
  end

  def test_find_const_comment_rb_define
    content = <<-EOF
/*
 * A comment
 */
rb_define_const(cFoo, "CONST", value);
    EOF

    parser = util_parser content

    comment = parser.find_const_comment 'const', 'CONST'

    assert_equal "/*\n * A comment\n */\n", comment
  end

  def test_find_const_comment_document_const
    content = <<-EOF
/*
 * Document-const: CONST
 *
 * A comment
 */
    EOF

    parser = util_parser content

    comment = parser.find_const_comment nil, 'CONST'

    assert_equal " *\n * A comment\n */", comment
  end

  def test_find_const_comment_document_const_full_name
    content = <<-EOF
/*
 * Document-const: Foo::CONST
 *
 * A comment
 */
    EOF

    parser = util_parser content

    comment = parser.find_const_comment nil, 'CONST', 'Foo'

    assert_equal " *\n * A comment\n */", comment
  end

  def test_find_body
    content = <<-EOF
/*
 * a comment for other_function
 */
VALUE
other_function() {
}

void
Init_Foo(void) {
    VALUE foo = rb_define_class("Foo", rb_cObject);

    rb_define_method(foo, "my_method", other_function, 0);
}
    EOF

    klass = util_get_class content, 'foo'
    other_function = klass.method_list.first

    assert_equal 'my_method', other_function.name
    assert_equal "a comment for other_function",
                 other_function.comment
    assert_equal '()', other_function.params

    code = other_function.token_stream.first.text

    assert_equal "VALUE\nother_function() {\n}", code
  end

  def test_find_body_2
    content = <<-CONTENT
/* Copyright (C) 2010  Sven Herzberg
 *
 * This file is free software; the author(s) gives unlimited
 * permission to copy and/or distribute it, with or without
 * modifications, as long as this notice is preserved.
 */

#include <ruby.h>

static VALUE
wrap_initialize (VALUE  self)
{
  return self;
}

/* */
static VALUE
wrap_shift (VALUE self,
            VALUE arg)
{
  return self;
}

void
init_gi_repository (void)
{
  VALUE mTest = rb_define_module ("Test");
  VALUE cTest = rb_define_class_under (mTest, "Test", rb_cObject);

  rb_define_method (cTest, "initialize", wrap_initialize, 0);
  rb_define_method (cTest, "shift", wrap_shift, 0);
}
    CONTENT

    klass = util_get_class content, 'cTest'
    assert_equal 2, klass.method_list.length
  end

  def test_find_body_define
    content = <<-EOF
#define something something_else

#define other_function rb_other_function

/*
 * a comment for rb_other_function
 */
VALUE
rb_other_function() {
}

void
Init_Foo(void) {
    VALUE foo = rb_define_class("Foo", rb_cObject);

    rb_define_method(foo, "my_method", other_function, 0);
}
    EOF

    klass = util_get_class content, 'foo'
    other_function = klass.method_list.first

    assert_equal 'my_method', other_function.name
    assert_equal 'a comment for rb_other_function', other_function.comment
    assert_equal '()', other_function.params
    assert_equal 118, other_function.offset
    assert_equal 8, other_function.line

    code = other_function.token_stream.first.text

    assert_equal "VALUE\nrb_other_function() {\n}", code
  end

  def test_find_body_define_comment
    content = <<-EOF
/*
 * a comment for other_function
 */
#define other_function rb_other_function

/* */
VALUE
rb_other_function() {
}

void
Init_Foo(void) {
    VALUE foo = rb_define_class("Foo", rb_cObject);

    rb_define_method(foo, "my_method", other_function, 0);
}
    EOF

    klass = util_get_class content, 'foo'
    other_function = klass.method_list.first

    assert_equal 'my_method', other_function.name
    assert_equal 'a comment for other_function', other_function.comment
    assert_equal '()', other_function.params
    assert_equal 39, other_function.offset
    assert_equal 4, other_function.line

    code = other_function.token_stream.first.text

    assert_equal "#define other_function rb_other_function", code
  end

  def test_find_body_document_method
    content = <<-EOF
/*
 * Document-method: bar
 * Document-method: baz
 *
 * a comment for bar
 */
VALUE
bar() {
}

void
Init_Foo(void) {
    VALUE foo = rb_define_class("Foo", rb_cObject);

    rb_define_method(foo, "bar", bar, 0);
    rb_define_method(foo, "baz", bar, 0);
}
    EOF

    klass = util_get_class content, 'foo'
    assert_equal 2, klass.method_list.length

    methods = klass.method_list.sort

    bar = methods.first
    assert_equal 'Foo#bar', bar.full_name
    assert_equal "a comment for bar", bar.comment

    baz = methods.last
    assert_equal 'Foo#baz', baz.full_name
    assert_equal "a comment for bar", baz.comment
  end

  def test_find_body_document_method_equals
    content = <<-EOF
/*
 * Document-method: Zlib::GzipFile#mtime=
 *
 * A comment
 */
static VALUE
rb_gzfile_set_mtime(VALUE obj, VALUE mtime)
{

void
Init_zlib() {
    mZlib = rb_define_module("Zlib");
    cGzipFile = rb_define_class_under(mZlib, "GzipFile", rb_cObject);
    cGzipWriter = rb_define_class_under(mZlib, "GzipWriter", cGzipFile);
    rb_define_method(cGzipWriter, "mtime=", rb_gzfile_set_mtime, 1);
}
    EOF

    klass = util_get_class content, 'cGzipWriter'
    assert_equal 1, klass.method_list.length

    methods = klass.method_list.sort

    bar = methods.first
    assert_equal 'Zlib::GzipWriter#mtime=', bar.full_name
    assert_equal 'A comment', bar.comment
  end

  def test_find_body_document_method_same
    content = <<-EOF
VALUE
s_bar() {
}

VALUE
bar() {
}

/*
 * Document-method: Foo::bar
 *
 * a comment for Foo::bar
 */

/*
 * Document-method: Foo#bar
 *
 * a comment for Foo#bar
 */

void
Init_Foo(void) {
    VALUE foo = rb_define_class("Foo", rb_cObject);

    rb_define_singleton_method(foo, "bar", s_bar, 0);
    rb_define_method(foo, "bar", bar, 0);
}
    EOF

    klass = util_get_class content, 'foo'
    assert_equal 2, klass.method_list.length

    methods = klass.method_list.sort

    s_bar = methods.first
    assert_equal 'Foo::bar', s_bar.full_name
    assert_equal "a comment for Foo::bar", s_bar.comment

    bar = methods.last
    assert_equal 'Foo#bar', bar.full_name
    assert_equal "a comment for Foo#bar", bar.comment
  end

  def test_find_modifiers_call_seq
    comment = <<-COMMENT
/* call-seq:
 *   commercial() -> Date <br />
 *   commercial(cwyear, cweek=41, cwday=5, sg=nil) -> Date [ruby 1.8] <br />
 *   commercial(cwyear, cweek=1, cwday=1, sg=nil) -> Date [ruby 1.9]
 *
 * If no arguments are given:
 * * ruby 1.8: returns a +Date+ for 1582-10-15 (the Day of Calendar Reform in
 *   Italy)
 * * ruby 1.9: returns a +Date+ for julian day 0
 *
 * Otherwise, returns a +Date+ for the commercial week year, commercial week,
 * and commercial week day given. Ignores the 4th argument.
 */

    COMMENT

    parser = util_parser ''
    method_obj = RDoc::AnyMethod.new nil, 'blah'

    parser.find_modifiers comment, method_obj

    expected = <<-CALL_SEQ.chomp
commercial() -> Date <br />
commercial(cwyear, cweek=41, cwday=5, sg=nil) -> Date [ruby 1.8] <br />
commercial(cwyear, cweek=1, cwday=1, sg=nil) -> Date [ruby 1.9]
 
    CALL_SEQ

    assert_equal expected, method_obj.call_seq
  end

  def test_find_modifiers_call_seq_no_blank
    comment = <<-COMMENT
/* call-seq:
 *   commercial() -> Date <br />
 *   commercial(cwyear, cweek=41, cwday=5, sg=nil) -> Date [ruby 1.8] <br />
 *   commercial(cwyear, cweek=1, cwday=1, sg=nil) -> Date [ruby 1.9]
 */

    COMMENT

    parser = util_parser ''
    method_obj = RDoc::AnyMethod.new nil, 'blah'

    parser.find_modifiers comment, method_obj

    expected = <<-CALL_SEQ.chomp
commercial() -> Date <br />
commercial(cwyear, cweek=41, cwday=5, sg=nil) -> Date [ruby 1.8] <br />
commercial(cwyear, cweek=1, cwday=1, sg=nil) -> Date [ruby 1.9]
 
    CALL_SEQ

    assert_equal expected, method_obj.call_seq
  end

  def test_find_modifiers_nodoc
    comment = <<-COMMENT
/* :nodoc:
 *
 * Blah
 */

    COMMENT

    parser = util_parser ''
    method_obj = RDoc::AnyMethod.new nil, 'blah'

    parser.find_modifiers comment, method_obj

    assert_equal nil, method_obj.document_self
  end

  def test_find_modifiers_yields
    comment = <<-COMMENT
/* :yields: a, b
 *
 * Blah
 */

    COMMENT

    parser = util_parser ''
    method_obj = RDoc::AnyMethod.new nil, 'blah'

    parser.find_modifiers comment, method_obj

    assert_equal 'a, b', method_obj.block_params

    expected = <<-EXPECTED
/*
 *
 * Blah
 */

    EXPECTED

    assert_equal expected, comment
  end

  def test_handle_method_args_minus_1
    parser = util_parser "Document-method: Object#m\n blah */"

    parser.content = <<-BODY
VALUE
rb_other(VALUE obj) {
  rb_funcall(obj, rb_intern("other"), 0);
  return rb_str_new2("blah, blah, blah");
}

VALUE
rb_m(int argc, VALUE *argv, VALUE obj) {
  VALUE o1, o2;
  rb_scan_args(argc, argv, "1", &o1, &o2);
}
    BODY

    parser.handle_method 'method', 'rb_cObject', 'm', 'rb_m', -1

    m = @top_level.find_module_named('Object').method_list.first

    assert_equal 'm', m.name
    assert_equal @top_level, m.file
    assert_equal 115, m.offset
    assert_equal 7, m.line

    assert_equal '(p1)', m.params
  end

  def test_handle_method_args_0
    parser = util_parser "Document-method: BasicObject#==\n blah */"

    parser.handle_method 'method', 'rb_cBasicObject', '==', 'rb_obj_equal', 0

    bo = @top_level.find_module_named 'BasicObject'

    assert_equal 1, bo.method_list.length

    equals2 = bo.method_list.first

    assert_equal '()', equals2.params
  end

  def test_handle_method_args_1
    parser = util_parser "Document-method: BasicObject#==\n blah */"

    parser.handle_method 'method', 'rb_cBasicObject', '==', 'rb_obj_equal', 1

    bo = @top_level.find_module_named 'BasicObject'

    assert_equal 1, bo.method_list.length

    equals2 = bo.method_list.first

    assert_equal '(p1)', equals2.params
  end

  def test_handle_method_args_2
    parser = util_parser "Document-method: BasicObject#==\n blah */"

    parser.handle_method 'method', 'rb_cBasicObject', '==', 'rb_obj_equal', 2

    bo = @top_level.find_module_named 'BasicObject'

    assert_equal 1, bo.method_list.length

    equals2 = bo.method_list.first

    assert_equal '(p1, p2)', equals2.params
  end

  # test_handle_args_minus_1 handled by test_handle_method

  def test_handle_method_args_minus_2
    parser = util_parser "Document-method: BasicObject#==\n blah */"

    parser.handle_method 'method', 'rb_cBasicObject', '==', 'rb_obj_equal', -2

    bo = @top_level.find_module_named 'BasicObject'

    assert_equal 1, bo.method_list.length

    equals2 = bo.method_list.first

    assert_equal '(*args)', equals2.params
  end

  def test_handle_method_initialize
    parser = util_parser "Document-method: BasicObject::new\n blah */"

    parser.handle_method('private_method', 'rb_cBasicObject',
                         'initialize', 'rb_obj_dummy', -1)

    bo = @top_level.find_module_named 'BasicObject'

    assert_equal 1, bo.method_list.length

    new = bo.method_list.first

    assert_equal 'new',   new.name
    assert_equal :public, new.visibility
  end

  def test_handle_singleton
    parser = util_parser <<-SINGLE
void Init_Blah(void) {
  cDate = rb_define_class("Date", rb_cObject);
  sDate = rb_singleton_class(cDate);
}
    SINGLE

    parser.scan

    assert_equal 'Date', parser.known_classes['sDate']
    assert_equal 'Date', parser.singleton_classes['sDate']
  end

  def test_look_for_directives_in
    parser = util_parser ''

    comment = "# :markup: not_rdoc\n"

    parser.look_for_directives_in @top_level, comment

    assert_equal "# :markup: not_rdoc\n", comment
    assert_equal 'not_rdoc', @top_level.metadata['markup']
  end

  def test_define_method
    content = <<-EOF
/*Method Comment! */
static VALUE
rb_io_s_read(argc, argv, io)
    int argc;
    VALUE *argv;
    VALUE io;
{
}

void
Init_IO(void) {
    /*
     * a comment for class Foo on rb_define_class
     */
    VALUE rb_cIO = rb_define_class("IO", rb_cObject);
    rb_define_singleton_method(rb_cIO, "read", rb_io_s_read, -1);
}
    EOF

    klass = util_get_class content, 'rb_cIO'
    read_method = klass.method_list.first
    assert_equal "read", read_method.name
    assert_equal "Method Comment!   ", read_method.comment
    assert_equal "rb_io_s_read", read_method.c_function
    assert read_method.singleton
  end

  def test_define_method_with_prototype
    content = <<-EOF
static VALUE rb_io_s_read(int, VALUE*, VALUE);

/* Method Comment! */
static VALUE
rb_io_s_read(argc, argv, io)
    int argc;
    VALUE *argv;
    VALUE io;
{
}

void
Init_IO(void) {
    /*
     * a comment for class Foo on rb_define_class
     */
    VALUE rb_cIO = rb_define_class("IO", rb_cObject);
    rb_define_singleton_method(rb_cIO, "read", rb_io_s_read, -1);
}
    EOF

    klass = util_get_class content, 'rb_cIO'
    read_method = klass.method_list.first
    assert_equal "read", read_method.name
    assert_equal "Method Comment!   ", read_method.comment
    assert_equal "rb_io_s_read", read_method.c_function
    assert read_method.singleton
  end

  def test_define_method_private
    content = <<-EOF
/*Method Comment! */
static VALUE
rb_io_s_read(argc, argv, io)
    int argc;
    VALUE *argv;
    VALUE io;
{
}

void
Init_IO(void) {
    /*
     * a comment for class Foo on rb_define_class
     */
    VALUE rb_cIO = rb_define_class("IO", rb_cObject);
    rb_define_private_method(rb_cIO, "read", rb_io_s_read, -1);
}
    EOF

    klass = util_get_class content, 'rb_cIO'
    read_method = klass.method_list.first
    assert_equal 'IO#read', read_method.full_name
    assert_equal :private, read_method.visibility
    assert_equal "Method Comment!   ", read_method.comment
  end

  def test_define_method_private_singleton
    content = <<-EOF
/*Method Comment! */
static VALUE
rb_io_s_read(argc, argv, io)
    int argc;
    VALUE *argv;
    VALUE io;
{
}

void
Init_IO(void) {
    /*
     * a comment for class Foo on rb_define_class
     */
    VALUE rb_cIO = rb_define_class("IO", rb_cObject);
    VALUE rb_cIO_s = rb_singleton_class(rb_cIO);
    rb_define_private_method(rb_cIO_s, "read", rb_io_s_read, -1);
}
    EOF

    klass = util_get_class content, 'rb_cIO'
    read_method = klass.method_list.first
    assert_equal "read", read_method.name
    assert_equal "Method Comment!   ", read_method.comment
    assert_equal :private, read_method.visibility
    assert read_method.singleton
  end

  def test_define_method_singleton
    content = <<-EOF
/*Method Comment! */
static VALUE
rb_io_s_read(argc, argv, io)
    int argc;
    VALUE *argv;
    VALUE io;
{
}

void
Init_IO(void) {
    /*
     * a comment for class Foo on rb_define_class
     */
    VALUE rb_cIO = rb_define_class("IO", rb_cObject);
    VALUE rb_cIO_s = rb_singleton_class(rb_cIO);
    rb_define_method(rb_cIO_s, "read", rb_io_s_read, -1);
}
    EOF

    klass = util_get_class content, 'rb_cIO'
    read_method = klass.method_list.first
    assert_equal "read", read_method.name
    assert_equal "Method Comment!   ", read_method.comment
    assert read_method.singleton
  end

  def test_rb_scan_args
    parser = util_parser ''

    assert_equal '(p1)',
                 parser.rb_scan_args('rb_scan_args(a, b, "1",)')
    assert_equal '(p1, p2)',
                 parser.rb_scan_args('rb_scan_args(a, b, "2",)')

    assert_equal '(p1 = v1)',
                 parser.rb_scan_args('rb_scan_args(a, b, "01",)')
    assert_equal '(p1 = v1, p2 = v2)',
                 parser.rb_scan_args('rb_scan_args(a, b, "02",)')

    assert_equal '(p1, p2 = v2)',
                 parser.rb_scan_args('rb_scan_args(a, b, "11",)')

    assert_equal '(p1, *args)',
                 parser.rb_scan_args('rb_scan_args(a, b, "1*",)')
    assert_equal '(p1, p2 = {})',
                 parser.rb_scan_args('rb_scan_args(a, b, "1:",)')
    assert_equal '(p1, &block)',
                 parser.rb_scan_args('rb_scan_args(a, b, "1&",)')

    assert_equal '(p1, p2)',
                 parser.rb_scan_args('rb_scan_args(a, b, "101",)')

    assert_equal '(p1, p2 = v2, p3)',
                 parser.rb_scan_args('rb_scan_args(a, b, "111",)')

    assert_equal '(p1, *args, p3)',
                 parser.rb_scan_args('rb_scan_args(a, b, "1*1",)')

    assert_equal '(p1, p2 = v2, *args)',
                 parser.rb_scan_args('rb_scan_args(a, b, "11*",)')
    assert_equal '(p1, p2 = v2, p3 = {})',
                 parser.rb_scan_args('rb_scan_args(a, b, "11:",)')
    assert_equal '(p1, p2 = v2, &block)',
                 parser.rb_scan_args('rb_scan_args(a, b, "11&",)')

    assert_equal '(p1, p2 = v2, *args, p4, p5 = {}, &block)',
                 parser.rb_scan_args('rb_scan_args(a, b, "11*1:&",)')

    # The following aren't valid according to spec but are according to the
    # implementation.
    assert_equal '(*args)',
                 parser.rb_scan_args('rb_scan_args(a, b, "*",)')
    assert_equal '(p1 = {})',
                 parser.rb_scan_args('rb_scan_args(a, b, ":",)')
    assert_equal '(&block)',
                 parser.rb_scan_args('rb_scan_args(a, b, "&",)')

    assert_equal '(*args, p2 = {})',
                 parser.rb_scan_args('rb_scan_args(a, b, "*:",)')
    assert_equal '(p1 = {}, &block)',
                 parser.rb_scan_args('rb_scan_args(a, b, ":&",)')
    assert_equal '(*args, p2 = {}, &block)',
                 parser.rb_scan_args('rb_scan_args(a, b, "*:&",)')
  end

  def util_get_class(content, name)
    @parser = util_parser content
    @parser.scan
    @parser.classes[name]
  end

  def util_parser(content)
    RDoc::Parser::C.new @top_level, @fn, content, @options, @stats
  end

end

