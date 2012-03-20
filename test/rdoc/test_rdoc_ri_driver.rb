require 'pp'
require 'rubygems'
require 'minitest/autorun'
require 'tmpdir'
require 'fileutils'
require 'stringio'
require 'rdoc/ri/driver'
require 'rdoc/rdoc'

class TestRDocRIDriver < MiniTest::Unit::TestCase

  def setup
    @RM = RDoc::Markup

    @tmpdir = File.join Dir.tmpdir, "test_rdoc_ri_driver_#{$$}"
    @home_ri = File.join @tmpdir, 'dot_ri'

    FileUtils.mkdir_p @tmpdir
    FileUtils.mkdir_p @home_ri

    @orig_ri = ENV['RI']
    @orig_home = ENV['HOME']
    ENV['HOME'] = @tmpdir
    ENV.delete 'RI'

    @options = RDoc::RI::Driver.process_args []
    @options[:home] = @tmpdir
    @options[:use_stdout] = true
    @options[:formatter] = @RM::ToRdoc
    @driver = RDoc::RI::Driver.new @options
  end

  def teardown
    ENV['HOME'] = @orig_home
    ENV['RI'] = @orig_ri
    FileUtils.rm_rf @tmpdir
  end

  DUMMY_PAGER = ":;\n"

  def with_dummy_pager
    pager_env, ENV['RI_PAGER'] = ENV['RI_PAGER'], DUMMY_PAGER
    yield
  ensure
    ENV['RI_PAGER'] = pager_env
  end

  def mu_pp(obj)
    s = ''
    s = PP.pp obj, s
    s = s.force_encoding(Encoding.default_external) if defined? Encoding
    s.chomp
  end

  def test_self_dump
    util_store

    out, = capture_io do
      RDoc::RI::Driver.dump @store.cache_path
    end

    assert_match %r%:class_methods%,    out
    assert_match %r%:modules%,          out
    assert_match %r%:instance_methods%, out
    assert_match %r%:ancestors%,        out
  end

  def test_add_also_in_empty
    out = @RM::Document.new

    @driver.add_also_in out, []

    assert_empty out
  end

  def test_add_also_in
    util_multi_store
    @store1.type = :system
    @store2.type = :home

    out = @RM::Document.new

    @driver.add_also_in out, [@store1, @store2]

    expected = @RM::Document.new(
      @RM::Rule.new(1),
      @RM::Paragraph.new('Also found in:'),
      @RM::Verbatim.new("ruby core\n",
                        "~/.ri\n"))

    assert_equal expected, out
  end

  def test_add_class
    util_multi_store

    out = @RM::Document.new

    @driver.add_class out, 'Bar', [@cBar]

    expected = @RM::Document.new(
      @RM::Heading.new(1, 'Bar < Foo'),
      @RM::BlankLine.new)

    assert_equal expected, out
  end

  def test_add_from
    util_store
    @store.type = :system

    out = @RM::Document.new

    @driver.add_from out, @store

    expected = @RM::Document.new @RM::Paragraph.new("(from ruby core)")

    assert_equal expected, out
  end

  def test_add_includes_empty
    out = @RM::Document.new

    @driver.add_includes out, []

    assert_empty out
  end

  def test_add_includes_many
    util_store

    out = @RM::Document.new

    enum = RDoc::Include.new 'Enumerable', nil
    @cFoo.add_include enum

    @driver.add_includes out, [[[@cFooInc, enum], @store]]

    expected = @RM::Document.new(
      @RM::Rule.new(1),
      @RM::Heading.new(1, "Includes:"),
      @RM::Paragraph.new("(from #{@store.friendly_path})"),
      @RM::BlankLine.new,
      @RM::Paragraph.new("Inc"),
      @RM::BlankLine.new,
      @RM::Paragraph.new("Include thingy"),
      @RM::BlankLine.new,
      @RM::Verbatim.new("Enumerable\n"))

    assert_equal expected, out
  end

  def test_add_includes_many_no_doc
    util_store

    out = @RM::Document.new

    enum = RDoc::Include.new 'Enumerable', nil
    @cFoo.add_include enum
    @cFooInc.instance_variable_set :@comment, ''

    @driver.add_includes out, [[[@cFooInc, enum], @store]]

    expected = @RM::Document.new(
      @RM::Rule.new(1),
      @RM::Heading.new(1, "Includes:"),
      @RM::Paragraph.new("(from #{@store.friendly_path})"),
      @RM::Verbatim.new("Inc\n",
                        "Enumerable\n"))

    assert_equal expected, out
  end

  def test_add_includes_one
    util_store

    out = @RM::Document.new

    @driver.add_includes out, [[[@cFooInc], @store]]

    expected = @RM::Document.new(
      @RM::Rule.new(1),
      @RM::Heading.new(1, "Includes:"),
      @RM::Paragraph.new("Inc (from #{@store.friendly_path})"),
      @RM::BlankLine.new,
      @RM::Paragraph.new("Include thingy"),
      @RM::BlankLine.new)

    assert_equal expected, out
  end

  def test_add_method_list
    out = @RM::Document.new

    @driver.add_method_list out, %w[new parse], 'Class methods'

    expected = @RM::Document.new(
      @RM::Heading.new(1, 'Class methods:'),
      @RM::BlankLine.new,
      @RM::Verbatim.new('new'),
      @RM::Verbatim.new('parse'),
      @RM::BlankLine.new)

    assert_equal expected, out
  end

  def test_add_method_list_interative
    @options[:interactive] = true
    driver = RDoc::RI::Driver.new @options

    out = @RM::Document.new

    driver.add_method_list out, %w[new parse], 'Class methods'

    expected = @RM::Document.new(
      @RM::Heading.new(1, 'Class methods:'),
      @RM::BlankLine.new,
      @RM::IndentedParagraph.new(2, 'new, parse'),
      @RM::BlankLine.new)

    assert_equal expected, out
  end

  def test_add_method_list_none
    out = @RM::Document.new

    @driver.add_method_list out, [], 'Class'

    assert_equal @RM::Document.new, out
  end

  def test_ancestors_of
    util_ancestors_store

    assert_equal %w[X Mixin Object Foo], @driver.ancestors_of('Foo::Bar')
  end

  def test_classes
    util_multi_store

    expected = {
      'Ambiguous' => [@store1, @store2],
      'Bar'       => [@store2],
      'Foo'       => [@store1],
      'Foo::Bar'  => [@store1],
      'Foo::Baz'  => [@store1, @store2],
      'Inc'       => [@store1],
    }

    assert_equal expected, @driver.classes
  end

  def test_class_document
    util_store

    tl1 = RDoc::TopLevel.new 'one.rb'
    tl2 = RDoc::TopLevel.new 'two.rb'

    @cFoo.add_comment 'one', tl1
    @cFoo.add_comment 'two', tl2
    @store.save_class @cFoo

    found = [
      [@store, @store.load_class(@cFoo.full_name)]
    ]

    out = @driver.class_document @cFoo.full_name, found, [], []

    expected = @RM::Document.new
    @driver.add_class expected, 'Foo', []
    @driver.add_includes expected, []
    @driver.add_from expected, @store
    expected << @RM::Rule.new(1)

    doc = @RM::Document.new(@RM::Paragraph.new('one'))
    doc.file = 'one.rb'
    expected.push doc
    expected << @RM::BlankLine.new
    doc = @RM::Document.new(@RM::Paragraph.new('two'))
    doc.file = 'two.rb'
    expected.push doc

    expected << @RM::Rule.new(1)
    expected << @RM::Heading.new(1, 'Instance methods:')
    expected << @RM::BlankLine.new
    expected << @RM::Verbatim.new('inherit')
    expected << @RM::Verbatim.new('override')
    expected << @RM::BlankLine.new

    assert_equal expected, out
  end

  def test_complete
    store = RDoc::RI::Store.new @home_ri
    store.cache[:ancestors] = {
      'Foo'      => %w[Object],
      'Foo::Bar' => %w[Object],
    }
    store.cache[:class_methods] = {
      'Foo' => %w[bar]
    }
    store.cache[:instance_methods] = {
      'Foo' => %w[Bar]
    }
    store.cache[:modules] = %w[
      Foo
      Foo::Bar
    ]

    @driver.stores = [store]

    assert_equal %w[Foo         ], @driver.complete('F')
    assert_equal %w[    Foo::Bar], @driver.complete('Foo::B')

    assert_equal %w[Foo#Bar],           @driver.complete('Foo#'),   'Foo#'
    assert_equal %w[Foo#Bar  Foo::bar], @driver.complete('Foo.'),   'Foo.'
    assert_equal %w[Foo::Bar Foo::bar], @driver.complete('Foo::'),  'Foo::'

    assert_equal %w[         Foo::bar], @driver.complete('Foo::b'), 'Foo::b'
  end

  def test_complete_ancestor
    util_ancestors_store

    assert_equal %w[Foo::Bar#i_method], @driver.complete('Foo::Bar#')

    assert_equal %w[Foo::Bar#i_method Foo::Bar::c_method Foo::Bar::new],
                 @driver.complete('Foo::Bar.')
  end

  def test_complete_classes
    util_store

    assert_equal %w[                       ], @driver.complete('[')
    assert_equal %w[                       ], @driver.complete('[::')
    assert_equal %w[Foo                    ], @driver.complete('F')
    assert_equal %w[Foo:: Foo::Bar Foo::Baz], @driver.complete('Foo::')
    assert_equal %w[      Foo::Bar Foo::Baz], @driver.complete('Foo::B')
  end

  def test_complete_multistore
    util_multi_store

    assert_equal %w[Bar], @driver.complete('B')
    assert_equal %w[Foo], @driver.complete('F')
    assert_equal %w[Foo::Bar Foo::Baz], @driver.complete('Foo::B')
  end

  def test_display
    doc = @RM::Document.new(
            @RM::Paragraph.new('hi'))

    out, = capture_io do
      @driver.display doc
    end

    assert_equal "hi\n", out
  end

  def test_display_class
    util_store

    out, = capture_io do
      @driver.display_class 'Foo::Bar'
    end

    assert_match %r%^= Foo::Bar%, out
    assert_match %r%^\(from%, out

    assert_match %r%^= Class methods:%, out
    assert_match %r%^  new%, out
    assert_match %r%^= Instance methods:%, out
    assert_match %r%^  blah%, out
    assert_match %r%^= Attributes:%, out
    assert_match %r%^  attr_accessor attr%, out

    assert_equal 1, out.scan(/-\n/).length
  end

  def test_display_class_ambiguous
    util_multi_store

    out, = capture_io do
      @driver.display_class 'Ambiguous'
    end

    assert_match %r%^= Ambiguous < Object$%, out
  end

  def test_display_class_multi_no_doc
    util_multi_store

    out, = capture_io do
      @driver.display_class 'Foo::Baz'
    end

    assert_match %r%^= Foo::Baz%, out
    assert_match %r%-\n%, out
    assert_match %r%Also found in:%, out
    assert_match %r%#{Regexp.escape @home_ri}%, out
    assert_match %r%#{Regexp.escape @home_ri2}%, out
  end

  def test_display_class_superclass
    util_multi_store

    out, = capture_io do
      @driver.display_class 'Bar'
    end

    assert_match %r%^= Bar < Foo%, out
  end

  def test_display_class_module
    util_store

    out, = capture_io do
      @driver.display_class 'Inc'
    end

    assert_match %r%^= Inc$%, out
  end

  def test_display_method
    util_store

    out, = capture_io do
      @driver.display_method 'Foo::Bar#blah'
    end

    assert_match %r%Foo::Bar#blah%, out
    assert_match %r%blah.5%,        out
    assert_match %r%blah.6%,        out
  end

  def test_display_method_attribute
    util_store

    out, = capture_io do
      @driver.display_method 'Foo::Bar#attr'
    end

    assert_match %r%Foo::Bar#attr%, out
    refute_match %r%Implementation from%, out
  end

  def test_display_method_inherited
    util_multi_store

    out, = capture_io do
      @driver.display_method 'Bar#inherit'
    end

    assert_match %r%^= Bar#inherit%, out
    assert_match %r%^=== Implementation from Foo%, out
  end

  def test_display_method_overriden
    util_multi_store

    out, = capture_io do
      @driver.display_method 'Bar#override'
    end

    refute_match %r%must not be displayed%, out
  end

  def test_display_name_not_found_class
    util_store

    out, = capture_io do
      assert_equal false, @driver.display_name('Foo::B')
    end

    expected = <<-EXPECTED
Foo::B not found, maybe you meant:

Foo::Bar
Foo::Baz
    EXPECTED

    assert_equal expected, out
  end

  def test_display_name_not_found_method
    util_store

    out, = capture_io do
      assert_equal false, @driver.display_name('Foo::Bar#b')
    end

    expected = <<-EXPECTED
Foo::Bar#b not found, maybe you meant:

Foo::Bar#blah
Foo::Bar#bother
    EXPECTED

    assert_equal expected, out
  end

  def test_display_method_params
    util_store

    out, = capture_io do
      @driver.display_method 'Foo::Bar#bother'
    end

    assert_match %r%things.*stuff%, out
  end

  def test_expand_class
    util_store

    assert_equal 'Foo',       @driver.expand_class('F')
    assert_equal 'Foo::Bar',  @driver.expand_class('F::Bar')

    assert_raises RDoc::RI::Driver::NotFoundError do
      @driver.expand_class 'F::B'
    end
  end

  def test_expand_name
    util_store

    assert_equal '.b',        @driver.expand_name('b')
    assert_equal 'Foo',       @driver.expand_name('F')
    assert_equal 'Foo::Bar#', @driver.expand_name('F::Bar#')

    e = assert_raises RDoc::RI::Driver::NotFoundError do
      @driver.expand_name 'Z'
    end

    assert_equal 'Z', e.name
  end

  def test_find_methods
    util_store

    items = []

    @driver.find_methods 'Foo::Bar.' do |store, klass, ancestor, types, method|
      items << [store, klass, ancestor, types, method]
    end

    expected = [
      [@store, 'Foo::Bar', 'Foo::Bar', :both, nil],
    ]

    assert_equal expected, items
  end

  def test_find_methods_method
    util_store

    items = []

    @driver.find_methods '.blah' do |store, klass, ancestor, types, method|
      items << [store, klass, ancestor, types, method]
    end

    expected = [
      [@store, 'Ambiguous', 'Ambiguous', :both, 'blah'],
      [@store, 'Foo',       'Foo',       :both, 'blah'],
      [@store, 'Foo::Bar',  'Foo::Bar',  :both, 'blah'],
      [@store, 'Foo::Baz',  'Foo::Baz',  :both, 'blah'],
      [@store, 'Inc',       'Inc',       :both, 'blah'],
    ]

    assert_equal expected, items
  end

  def test_filter_methods
    util_multi_store

    name = 'Bar#override'

    found = @driver.load_methods_matching name

    sorted = @driver.filter_methods found, name

    expected = [[@store2, [@override]]]

    assert_equal expected, sorted
  end

  def test_filter_methods_not_found
    util_multi_store

    name = 'Bar#inherit'

    found = @driver.load_methods_matching name

    sorted = @driver.filter_methods found, name

    assert_equal found, sorted
  end

  def test_formatter
    tty = Object.new
    def tty.tty?() true; end

    driver = RDoc::RI::Driver.new

    assert_instance_of @RM::ToAnsi, driver.formatter(tty)

    assert_instance_of @RM::ToBs, driver.formatter(StringIO.new)

    driver.instance_variable_set :@paging, true

    assert_instance_of @RM::ToBs, driver.formatter(StringIO.new)

    driver.instance_variable_set :@formatter_klass, @RM::ToHtml

    assert_instance_of @RM::ToHtml, driver.formatter(tty)
  end

  def test_in_path_eh
    path = ENV['PATH']

    refute @driver.in_path?('/nonexistent')

    ENV['PATH'] = File.expand_path '..', __FILE__

    assert @driver.in_path?(File.basename(__FILE__))
  ensure
    ENV['PATH'] = path
  end

  def test_method_type
    assert_equal :both,     @driver.method_type(nil)
    assert_equal :both,     @driver.method_type('.')
    assert_equal :instance, @driver.method_type('#')
    assert_equal :class,    @driver.method_type('::')
  end

  def test_name_regexp
    assert_equal %r%^RDoc::AnyMethod#new$%,
                 @driver.name_regexp('RDoc::AnyMethod#new')

    assert_equal %r%^RDoc::AnyMethod::new$%,
                 @driver.name_regexp('RDoc::AnyMethod::new')

    assert_equal %r%^RDoc::AnyMethod(#|::)new$%,
                 @driver.name_regexp('RDoc::AnyMethod.new')

    assert_equal %r%^Hash(#|::)\[\]$%,
                 @driver.name_regexp('Hash.[]')

    assert_equal %r%^Hash::\[\]$%,
                 @driver.name_regexp('Hash::[]')
  end

  def test_list_known_classes
    util_store

    out, = capture_io do
      @driver.list_known_classes
    end

    assert_equal "Ambiguous\nFoo\nFoo::Bar\nFoo::Baz\nInc\n", out
  end

  def test_list_known_classes_name
    util_store

    out, = capture_io do
      @driver.list_known_classes %w[F I]
    end

    assert_equal "Foo\nFoo::Bar\nFoo::Baz\nInc\n", out
  end

  def test_list_methods_matching
    util_store

    assert_equal %w[
        Foo::Bar#attr
        Foo::Bar#blah
        Foo::Bar#bother
        Foo::Bar::new
      ],
      @driver.list_methods_matching('Foo::Bar.').sort
  end

  def test_list_methods_matching_inherit
    util_multi_store

    assert_equal %w[
        Bar#baz
        Bar#inherit
        Bar#override
      ],
      @driver.list_methods_matching('Bar.').sort
  end

  def test_list_methods_matching_regexp
    util_store

    index = RDoc::AnyMethod.new nil, '[]'
    index.record_location @top_level
    @cFoo.add_method index
    @store.save_method @cFoo, index

    c_index = RDoc::AnyMethod.new nil, '[]'
    c_index.singleton = true
    c_index.record_location @top_level
    @cFoo.add_method c_index
    @store.save_method @cFoo, c_index

    @store.save_cache

    assert_equal %w[Foo#[]], @driver.list_methods_matching('Foo#[]')
    assert_equal %w[Foo::[]], @driver.list_methods_matching('Foo::[]')
  end

  def test_load_method
    util_store

    method = @driver.load_method(@store, :instance_methods, 'Foo', '#',
                                 'inherit')

    assert_equal @inherit, method
  end

  def test_load_method_inherited
    util_multi_store

    method = @driver.load_method(@store2, :instance_methods, 'Bar', '#',
                                 'inherit')

    assert_equal nil, method
  end

  def test_load_methods_matching
    util_store

    expected = [[@store, [@inherit]]]

    assert_equal expected, @driver.load_methods_matching('Foo#inherit')

    expected = [[@store, [@blah]]]

    assert_equal expected, @driver.load_methods_matching('.blah')

    assert_empty @driver.load_methods_matching('.b')
  end

  def test_load_methods_matching_inherited
    util_multi_store

    expected = [[@store1, [@inherit]]]

    assert_equal expected, @driver.load_methods_matching('Bar#inherit')
  end

  def _test_page # this test doesn't do anything anymore :(
    @driver.use_stdout = false

    with_dummy_pager do
      @driver.page do |io|
        skip "couldn't find a standard pager" if io == $stdout

        assert @driver.paging?
      end
    end

    refute @driver.paging?
  end

  def test_page_stdout
    @driver.use_stdout = true

    @driver.page do |io|
      assert_equal $stdout, io
    end

    refute @driver.paging?
  end

  def test_parse_name_method
    klass, type, meth = @driver.parse_name 'foo'

    assert_equal '',    klass, 'foo class'
    assert_equal '.',   type,  'foo type'
    assert_equal 'foo', meth,  'foo method'

    klass, type, meth = @driver.parse_name '#foo'

    assert_equal '',    klass, '#foo class'
    assert_equal '#',   type,  '#foo type'
    assert_equal 'foo', meth,  '#foo method'

    klass, type, meth = @driver.parse_name '::foo'

    assert_equal '',    klass, '::foo class'
    assert_equal '::',  type,  '::foo type'
    assert_equal 'foo', meth,  '::foo method'
  end

  def test_parse_name_single_class
    klass, type, meth = @driver.parse_name 'Foo'

    assert_equal 'Foo', klass, 'Foo class'
    assert_equal nil,   type,  'Foo type'
    assert_equal nil,   meth,  'Foo method'

    klass, type, meth = @driver.parse_name 'Foo#'

    assert_equal 'Foo', klass, 'Foo# class'
    assert_equal '#',   type,  'Foo# type'
    assert_equal nil,   meth,  'Foo# method'

    klass, type, meth = @driver.parse_name 'Foo::'

    assert_equal 'Foo', klass, 'Foo:: class'
    assert_equal '::',  type,  'Foo:: type'
    assert_equal nil,   meth,  'Foo:: method'

    klass, type, meth = @driver.parse_name 'Foo.'

    assert_equal 'Foo', klass, 'Foo. class'
    assert_equal '.',   type,  'Foo. type'
    assert_equal nil,   meth,  'Foo. method'

    klass, type, meth = @driver.parse_name 'Foo#Bar'

    assert_equal 'Foo', klass, 'Foo#Bar class'
    assert_equal '#',   type,  'Foo#Bar type'
    assert_equal 'Bar', meth,  'Foo#Bar method'

    klass, type, meth = @driver.parse_name 'Foo.Bar'

    assert_equal 'Foo', klass, 'Foo.Bar class'
    assert_equal '.',   type,  'Foo.Bar type'
    assert_equal 'Bar', meth,  'Foo.Bar method'

    klass, type, meth = @driver.parse_name 'Foo::bar'

    assert_equal 'Foo', klass, 'Foo::bar class'
    assert_equal '::',  type,  'Foo::bar type'
    assert_equal 'bar', meth,  'Foo::bar method'
  end

  def test_parse_name_namespace
    klass, type, meth = @driver.parse_name 'Foo::Bar'

    assert_equal 'Foo::Bar', klass, 'Foo::Bar class'
    assert_equal nil,        type,  'Foo::Bar type'
    assert_equal nil,        meth,  'Foo::Bar method'

    klass, type, meth = @driver.parse_name 'Foo::Bar#'

    assert_equal 'Foo::Bar', klass, 'Foo::Bar# class'
    assert_equal '#',        type,  'Foo::Bar# type'
    assert_equal nil,        meth,  'Foo::Bar# method'

    klass, type, meth = @driver.parse_name 'Foo::Bar#baz'

    assert_equal 'Foo::Bar', klass, 'Foo::Bar#baz class'
    assert_equal '#',        type,  'Foo::Bar#baz type'
    assert_equal 'baz',      meth,  'Foo::Bar#baz method'
  end

  def test_parse_name_special
    specials = %w[
      %
      &
      *
      +
      +@
      -
      -@
      /
      <
      <<
      <=
      <=>
      ==
      ===
      =>
      =~
      >
      >>
      []
      []=
      ^
      `
      |
      ~
      ~@
    ]

    specials.each do |special|
      parsed = @driver.parse_name special

      assert_equal ['', '.', special], parsed
    end
  end

  def _test_setup_pager # this test doesn't do anything anymore :(
    @driver.use_stdout = false

    pager = with_dummy_pager do @driver.setup_pager end

    skip "couldn't find a standard pager" unless pager

    assert @driver.paging?
  ensure
    pager.close if pager
  end

  def util_ancestors_store
    store1 = RDoc::RI::Store.new @home_ri
    store1.cache[:ancestors] = {
      'Foo'      => %w[Object],
      'Foo::Bar' => %w[Foo],
    }
    store1.cache[:class_methods] = {
      'Foo'      => %w[c_method new],
      'Foo::Bar' => %w[new],
    }
    store1.cache[:instance_methods] = {
      'Foo' => %w[i_method],
    }
    store1.cache[:modules] = %w[
      Foo
      Foo::Bar
    ]

    store2 = RDoc::RI::Store.new @home_ri
    store2.cache[:ancestors] = {
      'Foo'    => %w[Mixin Object],
      'Mixin'  => %w[],
      'Object' => %w[X Object],
      'X'      => %w[Object],
    }
    store2.cache[:class_methods] = {
      'Foo'    => %w[c_method new],
      'Mixin'  => %w[],
      'X'      => %w[],
      'Object' => %w[],
    }
    store2.cache[:instance_methods] = {
      'Foo'   => %w[i_method],
      'Mixin' => %w[],
    }
    store2.cache[:modules] = %w[
      Foo
      Mixin
      Object
      X
    ]

    @driver.stores = store1, store2
  end

  def util_multi_store
    util_store

    @store1 = @store

    @top_level = RDoc::TopLevel.new 'file.rb'

    @home_ri2 = "#{@home_ri}2"
    @store2 = RDoc::RI::Store.new @home_ri2

    # as if seen in a namespace like class Ambiguous::Other
    @mAmbiguous = @top_level.add_module RDoc::NormalModule, 'Ambiguous'

    @cFoo = @top_level.add_class RDoc::NormalClass, 'Foo'

    @cBar = @top_level.add_class RDoc::NormalClass, 'Bar', 'Foo'
    @cFoo_Baz = @cFoo.add_class RDoc::NormalClass, 'Baz'

    @baz = @cBar.add_method RDoc::AnyMethod.new(nil, 'baz')
    @baz.record_location @top_level

    @override = @cBar.add_method RDoc::AnyMethod.new(nil, 'override')
    @override.comment = 'must be displayed'
    @override.record_location @top_level

    @store2.save_class @mAmbiguous
    @store2.save_class @cBar
    @store2.save_class @cFoo_Baz

    @store2.save_method @cBar, @override
    @store2.save_method @cBar, @baz

    @store2.save_cache

    @driver.stores = [@store1, @store2]

    RDoc::RDoc.reset
  end

  def util_store
    @store = RDoc::RI::Store.new @home_ri

    @top_level = RDoc::TopLevel.new 'file.rb'

    @cFoo = @top_level.add_class RDoc::NormalClass, 'Foo'
    @mInc = @top_level.add_module RDoc::NormalModule, 'Inc'
    @cAmbiguous = @top_level.add_class RDoc::NormalClass, 'Ambiguous'

    doc = @RM::Document.new @RM::Paragraph.new('Include thingy')
    @cFooInc = @cFoo.add_include RDoc::Include.new('Inc', doc)
    @cFooInc.record_location @top_level

    @cFoo_Bar = @cFoo.add_class RDoc::NormalClass, 'Bar'

    @blah = @cFoo_Bar.add_method RDoc::AnyMethod.new(nil, 'blah')
    @blah.call_seq = "blah(5) => 5\nblah(6) => 6\n"
    @blah.record_location @top_level

    @bother = @cFoo_Bar.add_method RDoc::AnyMethod.new(nil, 'bother')
    @bother.block_params = "stuff"
    @bother.params = "(things)"
    @bother.record_location @top_level

    @new = @cFoo_Bar.add_method RDoc::AnyMethod.new nil, 'new'
    @new.record_location @top_level
    @new.singleton = true

    @attr = @cFoo_Bar.add_attribute RDoc::Attr.new nil, 'attr', 'RW', ''
    @attr.record_location @top_level

    @cFoo_Baz = @cFoo.add_class RDoc::NormalClass, 'Baz'
    @cFoo_Baz.record_location @top_level

    @inherit = @cFoo.add_method RDoc::AnyMethod.new(nil, 'inherit')
    @inherit.record_location @top_level

    # overriden by Bar in multi_store
    @overriden = @cFoo.add_method RDoc::AnyMethod.new(nil, 'override')
    @overriden.comment = 'must not be displayed'
    @overriden.record_location @top_level

    @store.save_class @cFoo
    @store.save_class @cFoo_Bar
    @store.save_class @cFoo_Baz
    @store.save_class @mInc
    @store.save_class @cAmbiguous

    @store.save_method @cFoo_Bar, @blah
    @store.save_method @cFoo_Bar, @bother
    @store.save_method @cFoo_Bar, @new
    @store.save_method @cFoo_Bar, @attr

    @store.save_method @cFoo, @inherit
    @store.save_method @cFoo, @overriden

    @store.save_cache

    @driver.stores = [@store]

    RDoc::RDoc.reset
  end

end

